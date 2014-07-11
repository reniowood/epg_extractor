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
        $scope.show_description_popup = false;
        $scope.description_popup_content = program_guide.get_description();

        var last_focused_event = program_guide.cursor;

        function toggle_description() {
            if ($scope.show_description_popup &&
                last_focused_event.service_index === program_guide.cursor.service_index &&
                last_focused_event.event_index === program_guide.cursor.event_index) {
                    $scope.show_description_popup = false;

                    return;
                }

            $scope.description_popup_content = program_guide.get_description();

            var event_DOM_element = $('#events').children().eq(program_guide.cursor.service_index).children().eq(program_guide.cursor.event_index);
            var description_popup = $('description-popup');

            description_popup.css({
                top: event_DOM_element.offset().top + 70,
                left: event_DOM_element.offset().left + 70
            });
            if (parseFloat(description_popup.css('top')) + description_popup.height() > $('#main').offset().top + $('#main').height()) {
                description_popup.css({
                    top: event_DOM_element.offset().top - description_popup.height()
                });
            }
            if (parseFloat(description_popup.css('left')) + description_popup.width() > $('#main').offset().left + $('#main').width()) {
                description_popup.css({
                    left: event_DOM_element.offset().left - description_popup.width()
                });
            }

            $scope.show_description_popup = true;
            last_focused_event = program_guide.cursor;
        }

        $scope.$on('NavigatorMsg', function () {
            switch (navigator.keycode) {
                case navigator.KEY_CODE.UP:
                    program_guide.navigate(program_guide.DIRECTION.UP);
                    $scope.show_description_popup = false;
                    break;
                case navigator.KEY_CODE.DOWN:
                    program_guide.navigate(program_guide.DIRECTION.DOWN);
                    $scope.show_description_popup = false;
                    break;
                case navigator.KEY_CODE.LEFT:
                    program_guide.navigate(program_guide.DIRECTION.LEFT);
                    $scope.show_description_popup = false;
                    break;
                case navigator.KEY_CODE.RIGHT:
                    program_guide.navigate(program_guide.DIRECTION.RIGHT);
                    $scope.show_description_popup = false;
                    break;
                case navigator.KEY_CODE.OK:
                    toggle_description();
                    break;
            }

            $scope.description_popup_content = program_guide.get_description();
        });
    }
]);
