var app = angular.module('EPGViewer', ['ngRoute', 'EPGViewer.services']);

app.config(['$routeProvider', '$locationProvider', function ($routeProvider, $locationProvider) {
    $routeProvider.
    when('/', {
        controller: 'LoadTSFileCtrl',
        templateUrl: '/templates/upload_ts.html'
    }).
    when('/showEPG', {
        controller: 'ShowEPGCtrl',
        templateUrl: '/templates/show_epg.html'
    }).
    otherwise({
        redirectTo: '/'
    });

    $locationProvider.html5Mode(true);
}]);

app.controller('LoadTSFileCtrl', ['$scope', '$location', '$http', 'EPG',
    function ($scope, $location, $http, EPG) {
        $scope.TSFileName = '';

        $scope.loadTSFile = function () {
            $http.get('/getEPG/' + $scope.TSFileName).then(function (response) {
                EPG.services = response.data;
                console.log(EPG.services);

                $location.path('/showEPG');
            });
        };
    }
]);

app.controller('ShowEPGCtrl', ['$scope', 'EPG',
    function ($scope, EPG) {
        $scope.services = EPG.services;
        console.log($scope.services);
    }
]);
