var server=require('../server/http.js');

console.log(server);

server.deploy(
    {
        port:8000,
        root:'/home/brandon/myApp/'
    }
);
