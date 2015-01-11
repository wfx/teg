
var express = require('express')
    , app = express()
    , server = require('http').createServer(app)
    , io = require('socket.io').listen(server)
    , conf = require('./config.json');

// Webserver
server.listen(conf.port);

app.configure(function(){
    app.use(express.static(__dirname + '/client'));
});

// Path only
app.get('/', function (req, res) {
    res.sendfile(__dirname + '/client/index.html');
});

// Webserver info
console.log('Server run at http://127.0.0.1:' + conf.port + '/');
