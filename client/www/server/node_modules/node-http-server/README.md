Node http server
================
v2.0.0 - stable
----

Simple to use stand alone node HTTP Server you can spin up from node apps, bash scripts, the commandline, C or python apps etc.

    npm install node-http-server
[![alt node-http-server npm stats](https://nodei.co/npm/node-http-server.png?downloads=true&downloadRank=true&stars=true)](https://npmjs.org/package/node-http-server)
[![alt node-http-server npm downloads](https://nodei.co/npm-dl/node-http-server.png "number of times the node-http-server package has been downloaded from npm")](https://npmjs.org/package/node-http-server)

This work is licenced via the [DBAD Public Licence](http://www.dbad-license.org/). 

----


## Defaults
---
#### currently modifiable via any interface, commandline, bash, node etc.

    port        : 8080
    root        : Current Working Directory (where you execute the command from)
    domain      : 0.0.0.0
    index       : index.html
    verbose     : false
    noCache     : true
    log         : false
    logFunction : serverLogging

|key|description|
|---|-----|
|port| the port on which the server should run  |
|root| the absolute location to the root dir for the public file system  |
|domain| the domain which this server applies to. You can add more servers via the node `` domains ``  implementation described below than you can via bash or commandline. If you want to accept incoming requests for ***ANY Applicable Domain*** use `` 0.0.0.0 `` this will allow any request that is pointed at this machine on the specified port to use this server config.  |
|index| the default file to look for in a dir. if not found a **404** will be displayed   |
|verbose| should the server display detailed info about what it is doing  |
|noCache| should the server prevent caching    |
|log| full path to log file, if specified file is not present it will be created, however the dir must be there. ie. /tmp/server.log It is recommended that you timestamp this file name with a time stamp like : `` '~/serverLogs/domain-'+new Date().getTime()+'.log' `` this will create a new log file each time the server is started/restarted/reboot etc...  |
|logFunction| this defaults to append timestamp to headers object and log as JSON in the `` log `` file. However, you can overwrite this and do whatever you like with the JSON data if you so choose. It accepts a javascript Object as the first argument for parsing. If you manually log to the default function. If you overwrite the function for custom logging, you must accept a javascript object as the first argument for the default log requests to function.  |

---
#### currently modifiable via node

    domains     :   {}
    
    contentType :   {
        html    : 'text/html',
        css     : 'text/css',
        js      : 'text/javascript',
        json    : 'application/json',
        txt     : 'text/plain',
        jpeg    : 'image/jpeg',
        jpg     : 'image/jpeg',
        png     : 'image/png',
        gif     : 'image/gif',
        ico     : 'image/x-icon',
        appcache: 'text/cache-manifest'
    }
    
    restrictedType: {}
    
    errors  :   {
        headers : {
            'Content-Type' : 'text/plain'
        },
        404: '404 MIA',
        415: '415 File type not supported',
        403: '403 Access Denied',
        500: '500 {{err}}'
    }

|key|description|
|---|-----------|
|domains| this is a mapping of hostname to path. It can be used for multiple different domains, or for subdomains.|
|contentType| mapping of file extension to header content type.  |
|restrictedType| extensions to which external access will be denied.  |
|errors| error headers and error strings, these can be anything you like from html to text etc. just make sure| they all can use the same headers. The **500** error will replace `` {{err}} `` in the specified value with the actual error message from the server.  |

---

## Commandline / bash use
`` launch `` is an argument that specifies to launch the server now with the provided arguments and defaults

    node ~/git/node-http-server/server/http.js root=~/myApp/ port=9999 launch=now

you can specify any of the variables frpom the ***currently modifiable via any interface, commandline, bash, node etc.*** section above. The order does not matter.

    node ~/git/node-http-server/server/http.js root=~/myApp/ port=8888 verbose=true launch=now

---

## node app use

    var server=require('node-http-server');

`` server `` has 2 methods, `` deploy `` and `` configTemplate ``

|Server Method| description |
|-------------|-------------|
|server.configTemplate| will generate a complete config file based off of the default values and arguments passed in when launching the app. **DO NOT USE launch=now** as an argument for a node app. This will result in launching 2 servers, the one you specify with the arguments passed and then the one the node app launches too.|
|server.deploy|will accept any config params and merge them with a fresh configTemplate, so passing a modified config based off of `` server.configTemplate() `` will result in using only the values from the modified config passed when deploying as it will override all of the defaults. ***The passed config object only merges to one level deep*** so if you pass a multi level object like `` contentTypes `` it will overwrite the default config with what you sent for that object rather than merging your object with the default.|

---

#### node examples
can be found in the examples folder

#### basic
this app could be launched as  
`` node basicApp.js verbose=true  ``  
to force verbose terminal output. This can be helpful if you have many servers in a single app and want them all to be verbose right now for debugging or testing purposes.
    
    var server=require('node-http-server');

    console.log(server);
    
    server.deploy(
        {
            port:8000,
            root:'~/myApp/'
        }
    ); 
    
---
#### verbose

    var server=require('node-http-server');

    console.log(server);
    
    server.deploy(
        {
            verbose:true,
            port:8001,
            root:'~/myApp/'
        }
    );
    
---
#### advanced
    
    var server=require('node-http-server');

    console.log(server);
    
    var config=server.configTemplate();
    config.errors['404']    = 'These are not the files you are looking for...';
    config.contentType.mp4  = 'video/mp4';
    config.port             = 8005;
    config.verbose          = true;
    config.root             = '~/myApp/'
    
    server.deploy(config);

---
#### multiple domains or subdomains

    var server=require('node-http-server');

    console.log(server);
    
    server.deploy(
        {
            verbose:true,
            port:8010,
            root:process.env.HOME+'/myApp/',
            domain:'myapp',
            domains:{
                'a.myapp':process.env.HOME+'/myApp/mySubdomain/',
                'yourapp.com':process.env.HOME+'/www/yourApp/'
            }
        }
    );    

---

## Starting with forever
*It is helpful especially when running multiple servers to label them*  with `` --uid `` for easy to remember process names

*when starting the same server many times, **like every time the system boots** you will want to append to the same log file* so use `` -a ``. Without `` -a `` forever will throw an error stating that the log file for the `` --uid `` already exists.

    forever --uid nodeServer -a start ~/git/node-http-server/server/http.js root=~/myApp/ port=9999 launch=now
    
This can be set as a ``.profile`` command or a ``.bash_rc`` command as well if you want to launch the server every time the computer boots up.
