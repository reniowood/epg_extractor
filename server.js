var express         = require('express'),
    morgan          = require('morgan'),
    cp              = require('child_process');
var app             = express();

app.use(express.static(__dirname + '/web_interface'));
app.use(morgan('dev'));

app.get('/showEPG/:TSFileName', function (req, res) {
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

app.listen(8888);
console.log("App listening on port 8888");
