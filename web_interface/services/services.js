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
        services: [],
        /*
         * start와 end 사이에 방송되는 event들을 모두 구한다.
         */
        generate_EPG : function (EPG_data) {
            for (var i=0; i<EPG_data.services.length; i+=1) {
                var service = {};
                var events = [];

                for (var j=0; j<EPG_data.services[i].events.length; j+=1) {
                    var event = EPG_data.services[i].events[j];
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

                service.name = EPG_data.services[i].name;
                service.show = true;
                service.events = events;

                this.services.push(service);
            }

            this.start_date = new Date(EPG_data.start_date);
            this.end_date = new Date(EPG_data.end_date);
        },
        get_start_date: function () {
            return this.start_date;
        },
        get_end_date: function () {
            return this.start_date;
        },
        get_service_number: function () {
            return this.services.length;
        },
        get_service: function (service_index) {
            return this.services[service_index];
        },
        get_event_number: function (service_index) {
            return this.services[service_index].events.length;
        },
        get_event: function (service_index, event_index) {
            return this.services[service_index].events[event_index];
        }
    };
});

services.factory('ProgramGuide', ['EPG', function (EPG) {
    return {
        /*
         * constants
         */
        LENGTH_IN_HOUR: 4,
        MAX_SERVICES: 5,
        DIRECTION: {
            UP: 0,
            DOWN: 1,
            LEFT: 2,
            RIGHT: 3
        },
        /*
         * variables
         */
        EPG: EPG,
        /*
         * private functions
         */
        _go_previous_event: function (focused_event) {
            if (focused_event.start_date.getTime() < this.start_date.getTime()) {
                this.start_date.setHours(this.start_date.getHours() - this.LENGTH_IN_HOUR);
                if (focused_event.start_date.getTime() >= this.start_date.getTime()) {
                    this.start_date = new Date(focused_event.start_date);
                }
                this.start_date.setMinutes(0);

                this.end_date = new Date(this.start_date);
                this.end_date.setHours(this.end_date.getHours() + this.LENGTH_IN_HOUR);
            } else {
                this.cursor.event_index--;
                if (this.cursor.event_index < 0) {
                    this.cursor.event_index = 0;
                }

                var previous_event = this.EPG.get_event(this.cursor.service_index, this.cursor.event_index);
                if (previous_event.start_date.getTime() < this.start_date.getTime() && previous_event.end_date.getTime() <= this.start_date.getTime()) {
                    this.start_date = new Date(previous_event.start_date);
                    this.start_date.setMinutes(0);

                    this.end_date = new Date(this.start_date);
                    this.end_date.setHours(this.end_date.getHours() + this.LENGTH_IN_HOUR);
                }
                if (this.start_date.getTime() < this.EPG.get_start_date().getTime()) {
                    this.start_date = new Date(this.EPG.get_start_date());
                    this.start_date.setMinutes(0);

                    this.end_date = new Date(this.start_date);
                    this.end_date.setHours(this.end_date.getHours() + this.LENGTH_IN_HOUR);
                }
            }
        },
        _go_next_event: function (focused_event) {
            if (focused_event.end_date.getTime() > this.end_date.getTime()) {
                this.start_date = new Date(this.end_date);
                this.start_date.setMinutes(0);

                this.end_date = new Date(this.start_date);
                this.end_date.setHours(this.end_date.getHours() + this.LENGTH_IN_HOUR);
            } else {
                this.cursor.event_index += 1;
                if (this.cursor.event_index === this.EPG.get_event_number(this.cursor.service_index)) {
                    this.cursor.event_index = this.EPG.get_event_number(this.cursor.service_index) - 1;
                }

                focused_event = this.EPG.get_event(this.cursor.service_index, this.cursor.event_index);
                if (focused_event.start_date.getTime() >= this.end_date.getTime()) {
                    this.start_date = new Date(focused_event.start_date);
                    this.start_date.setMinutes(0);

                    this.end_date = new Date(this.start_date);
                    this.end_date.setHours(this.end_date.getHours() + this.LENGTH_IN_HOUR);
                }
            }
        },
        _go_previous_service: function (focused_event) {
            var focused_event_start_time = focused_event.start_date.getTime() > this.start_date.getTime() ? focused_event.start_date.getTime() : this.start_date.getTime();
            var focused_event_end_time = focused_event.end_date.getTime();

            while (true) {
                if (this.cursor.service_index === 0) {
                    break;
                }

                var previous_service = this.EPG.get_service(this.cursor.service_index - 1);
                var fastest_start_covered_event_time = Number.MAX_VALUE;
                var fastest_start_covered_event_index = -1;
                var fastest_start_event_time = Number.MAX_VALUE;
                var fastest_start_event_index = -1;
                var cover_event_index = -1;

                var event_index = 0;
                for (; event_index<previous_service.events.length; event_index+=1) {
                    var event_candidate = previous_service.events[event_index];
                    var event_candidate_start_time = event_candidate.start_date.getTime() > this.start_date.getTime() ? event_candidate.start_date.getTime() : this.start_date.getTime();
                    var event_candidate_end_time = event_candidate.end_date.getTime();

                    if (event_candidate_end_time <= this.start_date.getTime() || event_candidate_start_time >= this.end_date.getTime()) {
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

                this.cursor.service_index--;
                if (fastest_start_covered_event_index !== -1) {
                    this.cursor.event_index = fastest_start_covered_event_index;

                    break;
                } else if (fastest_start_event_index !== -1) {
                    this.cursor.event_index = fastest_start_event_index;

                    break;
                } else if (cover_event_index !== -1) {
                    this.cursor.event_index = cover_event_index;

                    break;
                }
            }

            var previous_event = this.EPG.get_event(this.cursor.service_index, this.cursor.event_index);
            if (previous_event.start_date.getTime() < this.start_date.getTime() && previous_event.end_date.getTime() < this.start_date.getTime()) {
                this.start_date = new Date(previous_event.start_date);
                this.start_date.setMinutes(0);

                this.end_date = new Date(this.start_date);
                this.end_date.setHours(this.end_date.getHours() + this.LENGTH_IN_HOUR);
            }
            if (previous_event.start_date.getTime() >= this.end_date.getTime()) {
                this.start_date = new Date(previous_event.start_date);
                this.start_date.setMinutes(0);

                this.end_date = new Date(this.start_date);
                this.end_date.setHours(this.end_date.getHours() + this.LENGTH_IN_HOUR);
            }

            if (this.cursor.service_index < this.first_service_index) {
                this.first_service_index = this.cursor.service_index - this.MAX_SERVICES + 1;
            }
            if (this.first_service_index < 0) {
                this.first_service_index = 0;
            }
        },
        _go_next_service: function (focused_event) {
            var focused_event_start_time = focused_event.start_date.getTime() > this.start_date.getTime() ? focused_event.start_date.getTime() : this.start_date.getTime();
            var focused_event_end_time = focused_event.end_date.getTime() > this.end_date.getTime() ? this.end_date.getTime() : focused_event.end_date.getTime();

            while (true) {
                if (this.cursor.service_index === this.EPG.get_service_number() - 1) {
                    break;
                }

                var previous_service = this.EPG.get_service(this.cursor.service_index + 1);
                var fastest_start_covered_event_time = Number.MAX_VALUE;
                var fastest_start_covered_event_index = -1;
                var fastest_start_event_time = Number.MAX_VALUE;
                var fastest_start_event_index = -1;
                var cover_event_index = -1;

                var event_index = 0;
                for (; event_index<previous_service.events.length; event_index+=1) {
                    var event_candidate = previous_service.events[event_index];
                    var event_candidate_start_time = event_candidate.start_date.getTime() > this.start_date.getTime() ? event_candidate.start_date.getTime() : this.start_date.getTime();
                    var event_candidate_end_time = event_candidate.end_date.getTime() > this.end_date.getTime() ? this.end_date.getTime() : event_candidate.end_date.getTime();

                    if (event_candidate_end_time <= this.start_date.getTime() || event_candidate_start_time >= this.end_date.getTime()) {
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

                this.cursor.service_index += 1;
                if (fastest_start_covered_event_index !== -1) {
                    this.cursor.event_index = fastest_start_covered_event_index;

                    break;
                } else if (fastest_start_event_index !== -1) {
                    this.cursor.event_index = fastest_start_event_index;

                    break;
                } else if (cover_event_index !== -1) {
                    this.cursor.event_index = cover_event_index;

                    break;
                }
            }

            next_event = this.EPG.get_event(this.cursor.service_index, this.cursor.event_index);
            if (next_event.start_date.getTime() < this.start_date.getTime() && next_event.end_date.getTime() < this.start_date.getTime()) {
                this.start_date = new Date(next_event.start_date);
                this.start_date.setMinutes(0);

                this.end_date = new Date(this.start_date);
                this.end_date.setHours(this.end_date.getHours() + this.LENGTH_IN_HOUR);
            }
            if (next_event.start_date.getTime() >= this.end_date.getTime()) {
                this.start_date = new Date(next_event.start_date);
                this.start_date.setMinutes(0);

                this.end_date = new Date(this.start_date);
                this.end_date.setHours(this.end_date.getHours() + this.LENGTH_IN_HOUR);
            }

            if (this.cursor.service_index >= this.first_service_index + this.MAX_SERVICES) {
                this.first_service_index = this.cursor.service_index;
            }
            if (this.first_service_index === this.EPG.get_service_number() - this.MAX_SERVICES + 1) {
                this.first_service_index = this.EPG.get_service_number() - this.MAX_SERVICES;
            }
        },
        _update: function () {
            for (var i=0; i<this.EPG.get_service_number(); i+=1) {
                var service = this.EPG.get_service(i);

                service.show = false;
                service.highlight = false;
                service.continued_up = false;
                service.continued_down = false;

                var is_first_service_on_page = (i === this.first_service_index);
                var is_first_service = (i === 0);
                var is_last_service_on_page = (i === this.first_service_index + this.MAX_SERVICES - 1);
                var is_last_service = (i === this.EPG.get_service_number() - 1);

                if ((i >= this.first_service_index) && (i < this.first_service_index + this.MAX_SERVICES)) {
                    service.show = true;
                }

                if (i === this.cursor.service_index) {
                    service.highlight = true;
                }

                if (is_first_service_on_page && !is_first_service) {
                    service.continued_up = true;
                }
                if (is_last_service_on_page && !is_last_service) {
                    service.continued_down = true;
                }

                for (var j=0; j<this.EPG.get_event_number(i); j+=1) {
                    var event = this.EPG.get_event(i, j);
                    var event_start_date = new Date(event.start_date);
                    var event_end_date = new Date(event.end_date);

                    var is_event_before_EPG = (event_end_date.getTime() <= this.start_date.getTime());
                    var is_event_after_EPG = (event_start_date.getTime() >= this.end_date.getTime());

                    event.show = false;
                    event.highlight = false;
                    event.continued_left = false;
                    event.continued_right = false;
                    event.continued_both = false;

                    if (!is_event_before_EPG && !is_event_after_EPG) {
                        event.show = true;

                        if (this.cursor.service_index === i && this.cursor.event_index === j) {
                            event.highlight = true;
                        }

                        event.hour_from_EPG_start = (event.start_date.getTime() - this.start_date.getTime()) / (1000 * 60 * 60);
                        if (event.start_date.getTime() < this.start_date.getTime()) {
                            event.hour_from_EPG_start = 0;
                        }

                        event.width = event.duration;
                        if (event.start_date.getTime() < this.start_date.getTime()) {
                            event.width -= (this.start_date.getTime() - event.start_date.getTime()) / (1000 * 60 * 60);
                            event.continued_left = true;
                        }
                        if (event.end_date.getTime() > this.end_date.getTime()) {
                            event.width -= (event.end_date.getTime() - this.end_date.getTime()) / (1000 * 60 * 60);
                            event.continued_right = true;
                        }
                        if (event.continued_left && event.continued_right) {
                            event.continued_both = true;
                            event.continued_left = false;
                            event.continued_right = false;
                        }
                    }
                }
            }
        },
        /*
         * API
         */
        init: function (EPG_data) {
            this.EPG.generate_EPG(EPG_data);

            this.start_date = new Date(this.EPG.get_start_date());
            this.end_date = new Date(this.EPG.get_start_date());
            this.end_date.setHours(this.end_date.getHours() + this.LENGTH_IN_HOUR);
            this.first_service_index = 0;

            this.cursor = {
                service_index: 0,
                event_index: 0
            };


            this._update();
        },
        navigate: function (direction) {
            var focused_event = this.EPG.get_event(this.cursor.service_index, this.cursor.event_index);

            switch (direction) {
                case this.DIRECTION.UP:
                    this._go_previous_service(focused_event);
                    break;
                case this.DIRECTION.DOWN:
                    this._go_next_service(focused_event);
                    break;
                case this.DIRECTION.LEFT:
                    this._go_previous_event(focused_event);
                    break;
                case this.DIRECTION.RIGHT:
                    this._go_next_event(focused_event);
                    break;
            }

            this._update();
        },
        get_EPG: function () {
            return this.EPG;
        },
        get_description: function () {
            var focused_event = this.EPG.get_event(this.cursor.service_index, this.cursor.event_index);

            return {
                name: focused_event.name,
                description: focused_event.description
            };
        }
    };
}]);

services.factory('Navigator', ['$rootScope', function ($rootScope) {
    var navigator_service = {};

    navigator_service.keycode = '';
    navigator_service.KEY_CODE = {
        UP: 0,
        DOWN: 1,
        LEFT: 2,
        RIGHT: 3,
        OK: 4
    };
    navigator_service.navigate = function (keycode) {
        this.keycode = keycode;

        console.log('Navigator: navigate(keycode = ' + keycode + ')');

        $rootScope.$broadcast('NavigatorKeyPress');
    };
    navigator_service.init_navigator = function () {
        $rootScope.$broadcast('NavigatorInit');
    };
    navigator_service.uninit_navigator = function () {
        $rootScope.$broadcast('NavigatorUninit');
    };


    return navigator_service;
}]);
