var directives = angular.module('EPGViewer.directives', ['EPGViewer.services']);

directives.directive('descriptionPopup', ['$rootScope', 'ProgramGuide', 'Navigator', function ($rootScope, program_guide, navigator) {
    return {
        restrict: 'E',
        template: '<span class="name" ng-bind="description_popup_content.name"></span><p class="description" ng-bind="description_popup_content.description"></p>',
        link: function (scope, element, attrs) {
            scope.show = false;

            var last_focused_event = program_guide.cursor;

            function toggle_description() {
                if (scope.show &&
                    last_focused_event.service_index === program_guide.cursor.service_index &&
                    last_focused_event.event_index === program_guide.cursor.event_index) {
                        scope.show = false;

                        return;
                }

                scope.description_popup_content = program_guide.get_description();

                var event_DOM_element = $('#events').children().eq(program_guide.cursor.service_index).children().eq(program_guide.cursor.event_index);

                element.css({
                    top: event_DOM_element.offset().top + 70,
                    left: event_DOM_element.offset().left + 70
                });
                if (parseFloat(element.css('top')) + element.height() > $('#main').offset().top + $('#main').height()) {
                    element.css({
                        top: event_DOM_element.offset().top - element.height()
                    });
                }
                if (parseFloat(element.css('left')) + element.width() > $('#main').offset().left + $('#main').width()) {
                    element.css({
                        left: event_DOM_element.offset().left - element.width()
                    });
                }

                last_focused_event = program_guide.cursor;
                scope.show = true;
            }

            $rootScope.$on('NavigatorMsg', function () {
                switch (navigator.keycode) {
                    case navigator.KEY_CODE.UP:
                    case navigator.KEY_CODE.DOWN:
                    case navigator.KEY_CODE.LEFT:
                    case navigator.KEY_CODE.RIGHT:
                        scope.show = false;
                        break;
                    case navigator.KEY_CODE.OK:
                        toggle_description();
                        break;
                }
            });
        }
    };
}]);
