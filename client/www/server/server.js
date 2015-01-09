var server=require( 'node-http-server' );

console.log( 'local teg server: 127.0.0.1:8000' );

server.deploy(
    {
        port:8000,
        root:'../'
    }
);
