var app = angular.module('EPGViewer', ['ngRoute', 'EPGViewer.services']);

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

app.controller('LoadTSFileCtrl', ['$scope', '$location', 'EPGResource',
    function ($scope, $location, EPGResource, EPG) {
        $scope.TSFileName = '';

        $scope.loadTSFile = function () {
            $location.path('/showEPG/' + $scope.TSFileName);
        };
    }
]);

app.controller('ShowEPGCtrl', ['$scope', 'EPGData', 'EPG', 
    function ($scope, EPGData, EPG) {
        var get_time_labels = function (start_date, end_date) {
            var labels = [];

            var date = new Date(start_date);
            while (date < end_date) {
                var label = (date.getHours().toString().length == 2 ? date.getHours() : ('0' + date.getHours())) + ':' + (date.getMinutes().toString().length == 2 ? data.getMinutes() : ('0' + date.getMinutes()));
                var next_hour = new Date(date.getFullYear(), date.getMonth(), date.getDate(), date.getHours() + 1);
                if (end_date < next_hour)
                    next_hour = end_date;

                labels.push({
                    'label': label,
                    'width': (next_hour - date) / (1000 * 60 * 60),
                });

                date.setHours(date.getHours() + 1);
                date.setMinutes(0);
            }

            return labels;
        };
        var set_EPG_style = function (EPG) {
            for (var key in EPG) {
                if (EPG.hasOwnProperty(key)) {
                    for (var j=0; j<EPG[key].events.length; ++j) {
                        var event = EPG[key].events[j];
                        var hour_from_EPG_start = (event.start_date.getTime() - $scope.EPG_start_date.getTime()) / (1000 * 60 * 60);

                        event.left = hour_from_EPG_start;
                        if (event.start_date.getTime() < $scope.EPG_start_date.getTime())
                            event.left = 0;
                        event.width = event.duration;
                        if (event.start_date.getTime() < $scope.EPG_start_date.getTime())
                            event.width -= ($scope.EPG_start_date.getTime() - event.start_date.getTime()) / (1000 * 60 * 60);
                        if (event.end_date.getTime() > $scope.EPG_end_date.getTime())
                            event.width -= (event.end_date.getTime() - $scope.EPG_end_date.getTime()) / (1000 * 60 * 60);
                    }
                }
            }
        };
        var init_EPG = function () {
            $scope.EPG_length = 6; // hour(s) 
            $scope.EPG_start_date = new Date(EPGData.start_date);
            $scope.EPG_end_date = new Date(EPGData.start_date);
            $scope.EPG_end_date.setHours($scope.EPG_end_date.getHours() + $scope.EPG_length);
            $scope.EPG_hour_width = 200;
            $scope.EPG_now_date = ($scope.EPG_start_date.getMonth() + 1) + '/' + $scope.EPG_start_date.getDate();

            $scope.update_EPG();
        };

        $scope.update_EPG = function () {
            $scope.EPG_time_labels = get_time_labels($scope.EPG_start_date, $scope.EPG_end_date);
            $scope.EPG = EPG.generate_EPG(EPGData.services, $scope.EPG_start_date, $scope.EPG_end_date);
            set_EPG_style($scope.EPG);
        };
        $scope.navigate_EPG = function ($event) {
            console.log('haha');
        };
        $scope.back = function () {
            $scope.EPG_start_date.setHours($scope.EPG_start_date.getHours() - 1);
            $scope.EPG_end_date.setHours($scope.EPG_end_date.getHours() - 1);
            $scope.EPG_now_date = ($scope.EPG_start_date.getMonth() + 1) + '/' + $scope.EPG_start_date.getDate();
        };
        $scope.forward = function () {
            $scope.EPG_start_date.setHours($scope.EPG_start_date.getHours() + 1);
            $scope.EPG_end_date.setHours($scope.EPG_end_date.getHours() + 1);
            $scope.EPG_now_date = ($scope.EPG_start_date.getMonth() + 1) + '/' + $scope.EPG_start_date.getDate();
        };

        init_EPG();
    }
]);
