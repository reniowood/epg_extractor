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

app.controller('Navigator', ['$scope', 'NavigatorService', 
    function ($scope, navigator_service) {
        $scope.KEY_CODE = navigator_service.KEY_CODE;

        $scope.navigate = function (keycode) {
            navigator_service.navigate(keycode);
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

app.controller('ShowEPGCtrl', ['$scope', 'EPGData', 'EPG', 'NavigatorService', 
    function ($scope, EPGData, EPG, navigator_service) {
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
        var set_EPG_style = function (EPG, start_date, end_date, cursor) {
            for (var i=0; i<EPG.services.length; ++i) {
                var service = EPG.services[i];
                if (i >= $scope.EPG_first_service_shown && i < $scope.EPG_first_service_shown + $scope.EPG_max_services) {
                    service.show = true;
                } else {
                    service.show = false;
                }

                if (cursor.service === i) {
                    service.highlight = true;
                } else {
                    service.highlight = false;
                }

                for (var j=0; j<EPG.services[i].events.length; ++j) {
                    var event = EPG.services[i].events[j];
                    var event_start_date = new Date(event.start_date);
                    var event_end_date = new Date(event.end_date);

                    if (!(event_start_date.getTime() <= start_date.getTime() && event_end_date.getTime() <= start_date.getTime()) && !(event_start_date.getTime() >= end_date.getTime() && event_end_date.getTime() >= end_date.getTime())) {
                        event.show = true;

                        var hour_from_EPG_start = (event.start_date.getTime() - $scope.EPG_start_date.getTime()) / (1000 * 60 * 60);

                        event.left = hour_from_EPG_start;
                        if (event.start_date.getTime() < $scope.EPG_start_date.getTime())
                            event.left = 0;
                        event.width = event.duration;
                        if (event.start_date.getTime() < $scope.EPG_start_date.getTime())
                            event.width -= ($scope.EPG_start_date.getTime() - event.start_date.getTime()) / (1000 * 60 * 60);
                        if (event.end_date.getTime() > $scope.EPG_end_date.getTime())
                            event.width -= (event.end_date.getTime() - $scope.EPG_end_date.getTime()) / (1000 * 60 * 60);

                        if (cursor.service === i && cursor.event === j) {
                            event.highlight = true;
                        } else {
                            event.highlight = false;
                        }
                    } else {
                        event.show = false;
                    }
                }
            }
        };
        var init_EPG = function () {
            $scope.EPG_length = 4; // hour(s) 
            $scope.EPG_hour_width = 300;
            $scope.EPG_max_services = 5;

            $scope.EPG_start_date = new Date(EPGData.start_date);
            $scope.EPG_end_date = new Date(EPGData.start_date);
            $scope.EPG_end_date.setHours($scope.EPG_end_date.getHours() + $scope.EPG_length);
            $scope.EPG_now_date = ($scope.EPG_start_date.getMonth() + 1) + '/' + $scope.EPG_start_date.getDate();
            $scope.EPG_first_service_shown = 0;

            $scope.EPG_time_labels = get_time_labels($scope.EPG_start_date, $scope.EPG_end_date);
            $scope.EPG = EPG.generate_EPG(EPGData.services, EPGData.start_date, EPGData.end_date);

            $scope.EPG_cursor = {
                service: 0,
                event: 0
            };

            $scope.update_EPG();
        };

        $scope.update_EPG = function () {
            $scope.EPG_time_labels = get_time_labels($scope.EPG_start_date, $scope.EPG_end_date);
            set_EPG_style($scope.EPG, $scope.EPG_start_date, $scope.EPG_end_date, $scope.EPG_cursor);
        };
        $scope.backward = function () {
            var focused_event = $scope.EPG.services[$scope.EPG_cursor.service].events[$scope.EPG_cursor.event];
            if (focused_event.start_date.getTime() < $scope.EPG_start_date.getTime()) {
                $scope.EPG_start_date.setHours($scope.EPG_start_date.getHours() - $scope.EPG_length);
                if (focused_event.start_date.getTime() >= $scope.EPG_start_date.getTime()) {
                    $scope.EPG_start_date = new Date(focused_event.start_date);
                }
                $scope.EPG_start_date.setMinutes(0);
                $scope.EPG_end_date = new Date($scope.EPG_start_date);
                $scope.EPG_end_date.setHours($scope.EPG_end_date.getHours() + $scope.EPG_length);
            } else {
                $scope.EPG_cursor.event--;
                if ($scope.EPG_cursor.event < 0) {
                    $scope.EPG_cursor.event = 0;
                }

                focused_event = $scope.EPG.services[$scope.EPG_cursor.service].events[$scope.EPG_cursor.event];
                if (focused_event.start_date.getTime() < $scope.EPG_start_date.getTime() && focused_event.end_date.getTime() <= $scope.EPG_start_date.getTime()) {
                    $scope.EPG_start_date = new Date(focused_event.start_date);
                    $scope.EPG_start_date.setMinutes(0);
                    $scope.EPG_end_date = new Date($scope.EPG_start_date);
                    $scope.EPG_end_date.setHours($scope.EPG_end_date.getHours() + $scope.EPG_length);
                }
                if ($scope.EPG_start_date.getTime() < $scope.EPG.start_date.getTime()) {
                    $scope.EPG_start_date = new Date($scope.EPG.start_date);
                    $scope.EPG_start_date.setMinutes(0);
                    $scope.EPG_end_date = new Date($scope.EPG_start_date);
                    $scope.EPG_end_date.setHours($scope.EPG_end_date.getHours() + $scope.EPG_length);
                }
            }
            $scope.EPG_now_date = ($scope.EPG_start_date.getMonth() + 1) + '/' + $scope.EPG_start_date.getDate();

            $scope.update_EPG();
        };
        $scope.forward = function () {
            var focused_event = $scope.EPG.services[$scope.EPG_cursor.service].events[$scope.EPG_cursor.event];
            if (focused_event.end_date.getTime() > $scope.EPG_end_date.getTime()) {
                $scope.EPG_start_date = new Date($scope.EPG_end_date);
                $scope.EPG_start_date.setMinutes(0);
                $scope.EPG_end_date = new Date($scope.EPG_start_date);
                $scope.EPG_end_date.setHours($scope.EPG_end_date.getHours() + $scope.EPG_length);
            } else {
                $scope.EPG_cursor.event++;
                if ($scope.EPG_cursor.event === $scope.EPG.services[$scope.EPG_cursor.service].events.length) {
                    $scope.EPG_cursor.event = $scope.EPG.services[$scope.EPG_cursor.service].events.length - 1;
                }

                focused_event = $scope.EPG.services[$scope.EPG_cursor.service].events[$scope.EPG_cursor.event];
                if (focused_event.start_date.getTime() >= $scope.EPG_end_date.getTime()) {
                    $scope.EPG_start_date = new Date(focused_event.start_date);
                    $scope.EPG_start_date.setMinutes(0);
                    $scope.EPG_end_date = new Date($scope.EPG_start_date);
                    $scope.EPG_end_date.setHours($scope.EPG_end_date.getHours() + $scope.EPG_length);
                }
                if ($scope.EPG_end_date.getTime() > $scope.EPG.end_date.getTime()) {
                    $scope.EPG_end_date = new Date($scope.EPG.end_date);
                    $scope.EPG_end_date.setMinutes(0);
                    $scope.EPG_start_date = new Date($scope.EPG_end_date);
                    $scope.EPG_start_date.setHours($scope.EPG_start_date.getHours() - $scope.EPG_length);
                }
            }
            $scope.EPG_now_date = ($scope.EPG_start_date.getMonth() + 1) + '/' + $scope.EPG_start_date.getDate();

            $scope.update_EPG();
        };
        $scope.prev_service = function () {
            var focused_event = $scope.EPG.services[$scope.EPG_cursor.service].events[$scope.EPG_cursor.event];
            var focused_event_start_time = focused_event.start_date.getTime() > $scope.EPG_start_date.getTime() ? focused_event.start_date.getTime() : $scope.EPG_start_date.getTime();
            var focused_event_end_time = focused_event.end_date.getTime();

            while (true) {
                if ($scope.EPG_cursor.service === 0) {
                    break;
                }

                var previous_service = $scope.EPG.services[$scope.EPG_cursor.service - 1];
                var event_index = 0;
                var fastest_start_covered_event_time = Number.MAX_VALUE;
                var fastest_start_covered_event_index = -1;
                var fastest_start_event_time = Number.MAX_VALUE;
                var fastest_start_event_index = -1;
                var cover_event_index = -1;
                for (; event_index<previous_service.events.length; ++event_index) {
                    var event_candidate = previous_service.events[event_index];
                    var event_candidate_start_time = event_candidate.start_date.getTime() > $scope.EPG_start_date.getTime() ? event_candidate.start_date.getTime() : $scope.EPG_start_date.getTime();
                    var event_candidate_end_time = event_candidate.end_date.getTime();

                    if (event_candidate_end_time <= $scope.EPG_start_date.getTime() || event_candidate_start_time >= $scope.EPG_end_date.getTime()) {
                        continue;
                    }

                    if (event_candidate_start_time < focused_event_start_time && event_candidate_end_time >= focused_event_end_time) {
                        cover_event_index = event_index;
                    }

                    if (event_candidate_end_time <= focused_event_start_time || event_candidate_start_time >= focused_event_end_time) {
                        continue;
                    }

                    if (event_candidate_start_time < fastest_start_event_time) {
                        fastest_start_event_time = event_candidate_start_time;
                        fastest_start_event_index = event_index;
                    }

                    if (event_candidate_start_time < focused_event_start_time || event_candidate_end_time > focused_event_end_time) {
                        continue;
                    }

                    if (event_candidate_start_time < fastest_start_covered_event_time) {
                        fastest_start_covered_event_time = event_candidate_start_time;
                        fastest_start_covered_event_index = event_index;
                    }
                }

                $scope.EPG_cursor.service--;
                if (fastest_start_covered_event_index !== -1) {
                    $scope.EPG_cursor.event = fastest_start_covered_event_index;

                    break;
                } else if (fastest_start_event_index !== -1) {
                    $scope.EPG_cursor.event = fastest_start_event_index;

                    break;
                } else if (cover_event_index !== -1) {
                    $scope.EPG_cursor.event = cover_event_index;

                    break;
                }
            }

            focused_event = $scope.EPG.services[$scope.EPG_cursor.service].events[$scope.EPG_cursor.event];
            if (focused_event.start_date.getTime() < $scope.EPG_start_date.getTime() && focused_event.end_date.getTime() < $scope.EPG_start_date.getTime()) {
                $scope.EPG_start_date = new Date(focused_event.start_date);
                $scope.EPG_start_date.setMinutes(0);
                $scope.EPG_end_date = new Date($scope.EPG_start_date);
                $scope.EPG_end_date.setHours($scope.EPG_end_date.getHours() + $scope.EPG_length);
            }
            if ($scope.EPG_start_date.getTime() < $scope.EPG.start_date.getTime()) {
                $scope.EPG_start_date = new Date($scope.EPG.start_date);
                $scope.EPG_start_date.setMinutes(0);
                $scope.EPG_end_date = new Date($scope.EPG_start_date);
                $scope.EPG_end_date.setHours($scope.EPG_end_date.getHours() + $scope.EPG_length);
            }
            if (focused_event.start_date.getTime() >= $scope.EPG_end_date.getTime()) {
                $scope.EPG_start_date = new Date(focused_event.start_date);
                $scope.EPG_start_date.setMinutes(0);
                $scope.EPG_end_date = new Date($scope.EPG_start_date);
                $scope.EPG_end_date.setHours($scope.EPG_end_date.getHours() + $scope.EPG_length);
            }
            if ($scope.EPG_end_date.getTime() > $scope.EPG.end_date.getTime()) {
                $scope.EPG_end_date = new Date($scope.EPG.end_date);
                $scope.EPG_end_date.setMinutes(0);
                $scope.EPG_start_date = new Date($scope.EPG_end_date);
                $scope.EPG_start_date.setHours($scope.EPG_start_date.getHours() - $scope.EPG_length);
            }
            $scope.EPG_now_date = ($scope.EPG_start_date.getMonth() + 1) + '/' + $scope.EPG_start_date.getDate();

            if ($scope.EPG_cursor.service < $scope.EPG_first_service_shown) {
                $scope.EPG_first_service_shown = $scope.EPG_cursor.service;
            }
            if ($scope.EPG_first_service_shown < 0) {
                $scope.EPG_first_service_shown = 0;
            }

            $scope.update_EPG();
        };
        $scope.next_service = function () {
            var focused_event = $scope.EPG.services[$scope.EPG_cursor.service].events[$scope.EPG_cursor.event];
            var focused_event_start_time = focused_event.start_date.getTime() > $scope.EPG_start_date.getTime() ? focused_event.start_date.getTime() : $scope.EPG_start_date.getTime();
            var focused_event_end_time = focused_event.end_date.getTime() > $scope.EPG_end_date.getTime() ? $scope.EPG_end_date.getTime() : focused_event.end_date.getTime();

            while (true) {
                if ($scope.EPG_cursor.service === $scope.EPG.services.length - 1) {
                    break;
                }

                var previous_service = $scope.EPG.services[$scope.EPG_cursor.service + 1];
                var event_index = 0;
                var fastest_start_covered_event_time = Number.MAX_VALUE;
                var fastest_start_covered_event_index = -1;
                var fastest_start_event_time = Number.MAX_VALUE;
                var fastest_start_event_index = -1;
                var cover_event_index = -1;
                for (; event_index<previous_service.events.length; ++event_index) {
                    var event_candidate = previous_service.events[event_index];
                    var event_candidate_start_time = event_candidate.start_date.getTime() > $scope.EPG_start_date.getTime() ? event_candidate.start_date.getTime() : $scope.EPG_start_date.getTime();
                    var event_candidate_end_time = event_candidate.end_date.getTime() > $scope.EPG_end_date.getTime() ? $scope.EPG_end_date.getTime() : event_candidate.end_date.getTime();

                    if (event_candidate_end_time <= $scope.EPG_start_date.getTime() || event_candidate_start_time >= $scope.EPG_end_date.getTime()) {
                        continue;
                    }

                    if (event_candidate_start_time < focused_event_start_time && event_candidate_end_time >= focused_event_end_time) {
                        cover_event_index = event_index;
                    }

                    if (event_candidate_end_time <= focused_event_start_time || event_candidate_start_time >= focused_event_end_time) {
                        continue;
                    }

                    if (event_candidate_start_time < fastest_start_event_time) {
                        fastest_start_event_time = event_candidate_start_time;
                        fastest_start_event_index = event_index;
                    }

                    if (event_candidate_start_time < focused_event_start_time || event_candidate_end_time > focused_event_end_time) {
                        continue;
                    }

                    if (event_candidate_start_time < fastest_start_covered_event_time) {
                        fastest_start_covered_event_time = event_candidate_start_time;
                        fastest_start_covered_event_index = event_index;
                    }
                }

                $scope.EPG_cursor.service++;
                if (fastest_start_covered_event_index !== -1) {
                    $scope.EPG_cursor.event = fastest_start_covered_event_index;

                    break;
                } else if (fastest_start_event_index !== -1) {
                    $scope.EPG_cursor.event = fastest_start_event_index;

                    break;
                } else if (cover_event_index !== -1) {
                    $scope.EPG_cursor.event = cover_event_index;

                    break;
                }
            }

            focused_event = $scope.EPG.services[$scope.EPG_cursor.service].events[$scope.EPG_cursor.event];
            if (focused_event.start_date.getTime() < $scope.EPG_start_date.getTime() && focused_event.end_date.getTime() < $scope.EPG_start_date.getTime()) {
                $scope.EPG_start_date = new Date(focused_event.start_date);
                $scope.EPG_start_date.setMinutes(0);
                $scope.EPG_end_date = new Date($scope.EPG_start_date);
                $scope.EPG_end_date.setHours($scope.EPG_end_date.getHours() + $scope.EPG_length);
            }
            if ($scope.EPG_start_date.getTime() < $scope.EPG.start_date.getTime()) {
                $scope.EPG_start_date = new Date($scope.EPG.start_date);
                $scope.EPG_start_date.setMinutes(0);
                $scope.EPG_end_date = new Date($scope.EPG_start_date);
                $scope.EPG_end_date.setHours($scope.EPG_end_date.getHours() + $scope.EPG_length);
            }
            if (focused_event.start_date.getTime() >= $scope.EPG_end_date.getTime()) {
                $scope.EPG_start_date = new Date(focused_event.start_date);
                $scope.EPG_start_date.setMinutes(0);
                $scope.EPG_end_date = new Date($scope.EPG_start_date);
                $scope.EPG_end_date.setHours($scope.EPG_end_date.getHours() + $scope.EPG_length);
            }
            if ($scope.EPG_end_date.getTime() > $scope.EPG.end_date.getTime()) {
                $scope.EPG_end_date = new Date($scope.EPG.end_date);
                $scope.EPG_end_date.setMinutes(0);
                $scope.EPG_start_date = new Date($scope.EPG_end_date);
                $scope.EPG_start_date.setHours($scope.EPG_start_date.getHours() - $scope.EPG_length);
            }
            $scope.EPG_now_date = ($scope.EPG_start_date.getMonth() + 1) + '/' + $scope.EPG_start_date.getDate();

            if ($scope.EPG_cursor.service >= $scope.EPG_first_service_shown + $scope.EPG_max_services) {
                $scope.EPG_first_service_shown = $scope.EPG_cursor.service;
            }
            if ($scope.EPG_first_service_shown === $scope.EPG.services.length - $scope.EPG_max_services + 1) {
                $scope.EPG_first_service_shown = $scope.EPG.services.length - $scope.EPG_max_services;
            }

            $scope.update_EPG();
        };

        $scope.$on('NavigatorMsg', function () {
            switch (navigator_service.keycode) {
                case navigator_service.KEY_CODE.LEFT:
                    console.log('LEFT');
                    $scope.backward();
                    break;
                case navigator_service.KEY_CODE.RIGHT:
                    console.log('RIGHT');
                    $scope.forward();
                    break;
                case navigator_service.KEY_CODE.UP:
                    console.log('UP');
                    $scope.prev_service();
                    break;
                case navigator_service.KEY_CODE.DOWN:
                    console.log('DOWN');
                    $scope.next_service();
                    break;
            }
        });

        init_EPG();
    }
]);
