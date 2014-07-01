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

app.controller('ShowEPGCtrl', ['$scope', 'EPGData', 'EPG', 
    function ($scope, EPGData, EPG) {
        var start_date = new Date(EPGData.start_date);
        var end_date = new Date(EPGData.end_date);

        $scope.selected_date = {};
        $scope.selected_date.year = start_date.getFullYear();
        $scope.selected_date.month = start_date.getMonth();
        $scope.selected_date.day = start_date.getDate();

        $scope.services = EPGData.services;
        
        $scope.show_EPG = function () {
            var selected_date = new Date(Number($scope.selected_date.year), Number($scope.selected_date.month), Number($scope.selected_date.day));

            $scope.EPG_dates = EPG.get_dates_between(selected_date, selected_date);
            $scope.EPG_hours = EPG.get_hours_between(0, 24);

            EPG.generate_EPG($scope.services, selected_date);
        };

        available_dates = EPG.get_available_date(start_date, end_date);

        $scope.available_years = available_dates.available_years;
        $scope.available_months = available_dates.available_months;
        $scope.available_days = available_dates.available_days;

        $scope.show_EPG();
    }
]);
