var server=require('node-http-server');

console.log(server);

server.deploy(
    {
        port:8000,
        root:'../'
    }
);
