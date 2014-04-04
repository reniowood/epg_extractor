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
