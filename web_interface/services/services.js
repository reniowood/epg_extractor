var services = angular.module('EPGViewer.services', ['ngResource']);

services.factory('EPGResource', ['$resource', function ($resource) {
    return $resource("/showEPG/:TSFileName", {TSFileName: "@TSFileName"});
}]);

services.factory('EPGLoader', ['EPGResource', '$route', '$q', function (EPGResource, $route, $q) {
    return function () {
        var delay = $q.defer();

        EPGResource.get({TSFileName: $route.current.params.TSFileName}, function (recipe) {
            delay.resolve(recipe);
        }, function () {
            delay.reject('Unable to fetch EPG data from ' + $route.current.params.TSFileName);
        });

        return delay.promise;
    };
}]);

services.factory('EPG', function () {
    var get_dates_between = function (start_date, end_date) {
        var date = start_date;
        var dates = [];

        while (date <= end_date) {
            dates.push((date.getMonth() + 1).toString() + "/" + date.getDate());

            date = new Date(date);
            date.setDate(date.getDate() + 1);
        }

        return dates;
    };
    var get_hours_between = function (start_hour, end_hour) {
        var hours = [];

        for (var i=start_hour; i<end_hour; ++i) {
            hours.push(i);
        }

        return hours;
    };
    var get_available_dates = function (start_date, end_date) {
        available_years = [];
        available_months = [];
        available_days = [];

        for (var year=start_date.getFullYear(); year<=end_date.getFullYear(); ++year)
            available_years.push(year);
        if (available_years.length == 1) {
            for (var month=start_date.getMonth(); month<=end_date.getMonth(); ++month)
                available_months.push(month);
        } else {
            for (var month=start_date.getMonth(); month<12; ++month)
                available_months.push(month);
            for (var month=0; month<=end_date.getMonth(); ++month)
                available_months.push(month);
        }
        if (available_months.length == 1) {
            for (var day=start_date.getDate(); day<=end_date.getDate(); ++day)
                available_days.push(day);
        } else {
            for (var day=1; day<32; ++day)
                available_days.push(day);
        }

        return {
            'available_years': available_years,
            'available_months': available_months,
            'available_days': available_days
        };
    };
    var generate_EPG = function (services, now_date) {
        var duration, duration_minutes;
        var start, start_minutes;

        for (var i=0; i<services.length; ++i) {
            for (var j=0; j<services[i].events.length; ++j) {
                var event = services[i].events[j];
                var event_date = new Date(event.time.date);

                if (event_date.getTime() === now_date.getTime()) {
                    duration = event.time.duration.split(':');
                    duration_minutes = parseInt(duration[0]) * 60 + parseInt(duration[1]);
                    start = event.time.start.split(':');
                    start_minutes = parseInt(start[0]) * 60 + parseInt(start[1]);

                    event.width = 400.0 / 60 * duration_minutes - 7;
                    event.left = 212 + (new Date(event.time.date).getTime() - now_date.getTime()) / (1000.0*60*60*24) * (400 * 24) + (400 * (start_minutes / 60) - 2);
                    event.show = true;
                } else {
                    event.show = false;
                }
            }
        }
    };

    return {
        get_dates_between: get_dates_between, 
        get_hours_between: get_hours_between,
        get_available_date: get_available_dates,
        generate_EPG: generate_EPG
    };
});
