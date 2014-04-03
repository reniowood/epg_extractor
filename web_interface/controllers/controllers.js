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
            EPG: function(EPGLoader) {
                return EPGLoader();
            }
        },
        templateUrl: '/templates/show_epg.html'
    }).
    otherwise({
        redirectTo: '/'
    });
}]);

app.controller('LoadTSFileCtrl', ['$scope', '$location', '$http', 'EPG',
    function ($scope, $location, $http, EPG) {
        $scope.TSFileName = '';

        $scope.loadTSFile = function () {
            $scope.services = EPG.query({TSFileName: $scope.TSFileName}, function (EPG) {
                $location.path('/showEPG/' + $scope.TSFileName);
            });
        };
    }
]);

app.controller('ShowEPGCtrl', ['$scope', 'EPG',
    function ($scope, EPG) {
        $scope.services = EPG;
        console.log($scope.services);
    }
]);
