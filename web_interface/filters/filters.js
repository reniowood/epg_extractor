var filters = angular.module('EPGViewer.filters', []);

filters.filter('hourMinute', function () {
    return function (date) {
        var time_label = (date.getHours().toString().length == 2 ? date.getHours() : ('0' + date.getHours())) + ':' + (date.getMinutes().toString().length == 2 ? data.getMinutes() : ('0' + date.getMinutes()));

        return time_label;
    };
});

filters.filter('monthDay', function () {
    return function (date) {
        var month_day = (date.getMonth() + 1) + '/' + date.getDate();

        return escape(month_day);
    };
});
