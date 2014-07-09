var services = angular.module('EPGViewer.services', ['ngResource']);

services.factory('EPGResource', ['$resource', function ($resource) {
    return $resource("/showEPG/:TSFileName", {TSFileName: "@TSFileName"});
}]);

services.factory('EPGLoader', ['EPGResource', '$route', '$q', function (EPGResource, $route, $q) {
    return function () {
        var delay = $q.defer();

        EPGResource.get({TSFileName: $route.current.params.TSFileName}, function (EPGData) {
            delay.resolve(EPGData);
        }, function () {
            delay.reject('Unable to fetch EPG data from ' + $route.current.params.TSFileName);
        });

        return delay.promise;
    };
}]);

services.factory('EPG', function () {
    return {
        /*
         * start와 end 사이에 방송되는 event들을 모두 구한다.
         */
        generate_EPG : function (EPG_data, start_date, end_date) {
            var EPG = {};
            var services = [];

            for (var i=0; i<EPG_data.length; ++i) {
                var service = {};
                var events = [];

                for (var j=0; j<EPG_data[i].events.length; ++j) {
                    var event = EPG_data[i].events[j];
                    var event_start_date = new Date(event.time.start_date);
                    var event_end_date = new Date(event.time.end_date);

                    var duration = event.time.duration.split(':');
                    var duration_in_hour = parseInt(duration[0]) + parseInt(duration[1]) / 60;

                    var available_event = {
                        'name': event.name,
                        'start_date': event_start_date,
                        'end_date': event_end_date,
                        'duration': duration_in_hour,
                        'description': event.description,
                        'show': true
                    };

                    events.push(available_event);
                }

                service.name = EPG_data[i].name;
                service.show = true;
                service.events = events;

                services.push(service);
            }

            EPG.start_date = new Date(start_date);
            EPG.end_date = new Date(end_date);
            EPG.services = services;

            return EPG;
        }
    };
});

services.factory('NavigatorService', function ($rootScope) {
    var navigator_service = {};

    navigator_service.keycode = '';
    navigator_service.KEY_CODE = {
        LEFT: 0,
        DOWN: 1,
        UP: 2,
        RIGHT: 3,
        OK: 4
    };
    navigator_service.navigate = function (keycode) {
        this.keycode = keycode;

        console.log('NavigatorService: navigate(keycode = ' + keycode + ')');

        $rootScope.$broadcast('NavigatorMsg');
    };

    return navigator_service;
});
