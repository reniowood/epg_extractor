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
            EPGData: function(EPGLoader) {
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

app.controller('ShowEPGCtrl', ['$scope', 'EPGData',
    function ($scope, EPGData) {
        var duration, duration_minutes;
        var start, start_minutes;

        $scope.start_date = new Date(EPGData.start_date);
        $scope.end_date = new Date(EPGData.end_date);
        $scope.services = EPGData.services;

        for (var i in $scope.services) {
            for (var j in $scope.services[i].events) {
                var event = $scope.services[i].events[j];
                duration = event.time.duration.split(':');
                duration_minutes = parseInt(duration[0]) * 60 + parseInt(duration[1]);
                start = event.time.start.split(':');
                start_minutes = parseInt(start[0]) * 60 + parseInt(start[1]);

                event.width = 400.0 / 60 * duration_minutes - 7;
                event.left = 212 + (new Date(event.time.date).getTime() - $scope.start_date.getTime()) / (1000.0*60*60*24) * (400 * 24) + (400 * (start_minutes / 60) - 2);
            }
        }

        var date = $scope.start_date;
        $scope.dates = [];
        while (date <= $scope.end_date) {
            console.log(date);
            $scope.dates.push((date.getMonth() + 1).toString() + "-" + date.getDate());

            date = new Date(date);
            date.setDate(date.getDate() + 1);
        }

        $scope.hours = [];
        for (var i=0; i<24; ++i) {
            $scope.hours.push(i);
        }
    }
]);
