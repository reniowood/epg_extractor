var services = angular.module('EPGViewer.services', ['ngResource']);

services.factory('EPG', ['$resource', function ($resource) {
    return $resource("/showEPG/:TSFileName", {TSFileName: "@TSFileName"}, {method: 'query', isArray: false});
}]);

services.factory('EPGLoader', ['EPG', '$route', '$q', function (EPG, $route, $q) {
    return function () {
        var delay = $q.defer();

        EPG.query({TSFileName: $route.current.params.TSFileName}, function (recipe) {
            delay.resolve(recipe);
        }, function () {
            delay.reject('Unable to fetch EPG data from ' + $route.current.params.TSFileName);
        });

        return delay.promise;
    };
}]);
