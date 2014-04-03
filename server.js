var express = require('express'),
    fs      = require('fs'),
    cp      = require('child_process');
var app     = express();

app.configure(function () {
    app.use(express.static(__dirname + '/web_interface'));
    app.use(express.logger('dev'));
    app.use(express.methodOverride());
    app.use(express.bodyParser());
    app.use(app.router);
});

app.get('/getEPG/:TSFileName', function (req, res) {
    var extractor = cp.spawn('extractor/extractor.out', [req.params.TSFileName]);
    var EPGJSONData = '';

    extractor.stdout.on('data', function (data) {
        EPGJSONData += data;
    });

    extractor.stderr.on('err', function (err) {
        console.log('Error: ' + err);
    });

    extractor.on('close', function (code) {
        res.send(JSON.parse(EPGJSONData));
    });
});

app.get('/', function (req, res) {
    res.redirect('/viewer.html');
});

app.listen(8888);
console.log("App listening on port 8888");
