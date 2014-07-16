var app = angular.module('EPGViewer', ['ngRoute', 'EPGViewer.services', 'EPGViewer.directives', 'EPGViewer.filters']);

app.config(['$routeProvider', '$locationProvider', function ($routeProvider, $locationProvider) {
    $routeProvider.
    when('/', {
        controller: 'LoadTSFileCtrl',
        templateUrl: '/templates/upload_ts.html'
    }).
    when('/showEPG/:TSFileName', {
        controller: 'ShowEPGCtrl',
        resolve: {
            EPGData: function (EPGLoader) {
                return EPGLoader();
            }
        },
        templateUrl: '/templates/show_epg.html'
    }).
    otherwise({
        redirectTo: '/'
    });
}]);

app.controller('LoadTSFileCtrl', ['$scope', '$location', 'EPGResource', 'Navigator',
    function ($scope, $location, EPGResource, navigator) {
        $scope.TSFileName = '';

        navigator.uninit_navigator();

        $scope.loadTSFile = function () {
            $location.path('/showEPG/' + $scope.TSFileName);
        };
    }
]);

app.controller('ShowEPGCtrl', ['$scope', 'EPGData', 'ProgramGuide', 'Navigator', 
    function ($scope, EPG_data, program_guide, navigator) {
        function update_time_labels() {
            $scope.now_date = program_guide.start_date;
            $scope.time_labels = [];

            var date = new Date(program_guide.start_date);
            while (date < program_guide.end_date) {
                var next_hour = new Date(date.getFullYear(), date.getMonth(), date.getDate(), date.getHours() + 1);
                if (program_guide.end_date < next_hour)
                    next_hour = program_guide.end_date;

                $scope.time_labels.push(next_hour);

                date.setHours(date.getHours() + 1);
                date.setMinutes(0);
            }
        }

        program_guide.init(EPG_data);

        $scope.EPG = program_guide.get_EPG();
        $scope.length_in_hour = program_guide.LENGTH_IN_HOUR;
        $scope.event_width_per_hour = 300;

        navigator.init_navigator();
        update_time_labels();

        $scope.$on('NavigatorKeyPress', function () {
            switch (navigator.keycode) {
                case navigator.KEY_CODE.UP:
                    program_guide.navigate(program_guide.DIRECTION.UP);
                    update_time_labels();
                    break;
                case navigator.KEY_CODE.DOWN:
                    program_guide.navigate(program_guide.DIRECTION.DOWN);
                    update_time_labels();
                    break;
                case navigator.KEY_CODE.LEFT:
                    program_guide.navigate(program_guide.DIRECTION.LEFT);
                    update_time_labels();
                    break;
                case navigator.KEY_CODE.RIGHT:
                    program_guide.navigate(program_guide.DIRECTION.RIGHT);
                    update_time_labels();
                    break;
            }
        });
    }
]);
