var app = angular.module('EPGViewer', ['ngRoute', 'EPGViewer.services', 'EPGViewer.directives']);

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

app.controller('Navigator', ['$scope', 'Navigator', 
    function ($scope, navigator) {
        $scope.KEY_CODE = navigator.KEY_CODE;

        $scope.navigate = function (keycode) {
            navigator.navigate(keycode);
        };
    }
]);

app.controller('LoadTSFileCtrl', ['$scope', '$location', 'EPGResource',
    function ($scope, $location, EPGResource, EPG) {
        $scope.TSFileName = '';

        $scope.loadTSFile = function () {
            $location.path('/showEPG/' + $scope.TSFileName);
        };
    }
]);

app.controller('ShowEPGCtrl', ['$scope', 'EPGData', 'EPG', 'ProgramGuide', 'Navigator', 
    function ($scope, EPG_data, EPG, program_guide, navigator) {
        program_guide.init(EPG_data);

        $scope.EPG = program_guide.get_EPG();
        $scope.length_in_hour = program_guide.LENGTH_IN_HOUR;
        $scope.event_width_per_hour = 300;

        $scope.$on('NavigatorMsg', function () {
            switch (navigator.keycode) {
                case navigator.KEY_CODE.UP:
                    program_guide.navigate(program_guide.DIRECTION.UP);
                    break;
                case navigator.KEY_CODE.DOWN:
                    program_guide.navigate(program_guide.DIRECTION.DOWN);
                    break;
                case navigator.KEY_CODE.LEFT:
                    program_guide.navigate(program_guide.DIRECTION.LEFT);
                    break;
                case navigator.KEY_CODE.RIGHT:
                    program_guide.navigate(program_guide.DIRECTION.RIGHT);
                    break;
            }
        });
    }
]);
