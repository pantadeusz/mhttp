# mhttp

[![Build Status](https://travis-ci.org/pantadeusz/mhttp.svg?branch=master)](https://travis-ci.org/pantadeusz/mhttp)

### by Tadeusz Puźniakowski, 2017

Very simple implementation of HTTP server allowing you to write your code in C++11.
The intention of this project is to provide lightweight server for 
Internet of Things applications, where there is need to access hardware
directly via C or C++ code.




## Compilation

For ninja build:
```sh
./configure
cd buildninja
ninja
```

For make
```sh
./configure
cd buildmake
make
```


## Example usage

You can compile your programs with the following command
```sh
g++ -Isrc -std=c++11 -fopenmp main.cpp -L. -lmhttp
```

and then run with:
```sh
LD_LIBRARY_PATH=. ./a.out
```


### server for static contents

```c++
#include <http.hpp>
#include <string>

using namespace tp::http;
using namespace std;

int main ( int argc, char **argv ) {
    Http srv( "localhost", 8090, true );
    srv.GET("/(.*)",getStaticFileHandler("",true));
    srv.start();
    return 0;
}
```

### server for static contents with filter that will redirect / to /index.html

```c++
#include <http.hpp>
#include <string>

using namespace tp::http;
using namespace std;

int main ( int argc, char **argv ) {
    Http srv( "localhost", 8090, true );
    srv.filter_GET("/", [&]( Request &req )->void {
        req.queryString = "/index.html";
    });
    srv.GET("/(.*)",getStaticFileHandler("",true));
    srv.start();
    return 0;
}
```

### server with dynamic /hello page

```c++
#include <http.hpp>
#include <string>

using namespace tp::http;
using namespace std;

int main ( int argc, char **argv ) {
    Http srv( "localhost", 8090, true );
    srv.GET( "/hello", [&]( Request &req )->t_Response {
        return ResponseFactory::response("Hello server");
    } );
    srv.start();
    return 0;
}
```

### server /hello page that returns list of http arguments

```c++
#include <http.hpp>
#include <string>
#include <sstream>

using namespace tp::http;
using namespace std;

int main ( int argc, char **argv ) {
    Http srv( "localhost", 8090, true );
    auto handler = []( Request &req )->t_Response {
        std::stringstream ss;
        ss << "<p>Hello world!!</p><table>";
        for (auto p : req.getParams()) ss << "<tr><td>" << p.first << "</td><td>" << p.second << "<td></tr>\r\n";
        ss << "</table>";
        return ResponseFactory::response(ss.str(), 200, "OK");
    };
    srv.GET( "/hello", handler );
    srv.POST( "/hello", handler );
    srv.start();
    return 0;
}
```

### Session handling

Filter with session:
```c++
    srv.filter_sGET("/", [&]( Request &req, Session & )->void {
        req.queryString = "/index.html";
    });
```

Request handle with session:


```c++
    srv.sGET( "/session", [&]( Request &, Session &session )->t_Response {
        if (session.getSessionData() == NULL) {
            session.setSessionData(new StringSessionData());
        }
        auto sd = session.getSessionData();
        sd->fromString(sd->toString() + "x "); // append x to session data string
        return ResponseFactory::response("Session ID = " + sd->toString());
    } );
```


### C++14 features

You can use C++14 features, for shorter notation of lambdas:
```c++
    srv.filter_sGET("/", [&]( auto &req, auto & )->auto {
        req.queryString = "/index.html";
    });
```


### donations

I accept donations in BTC at 1cezmsed5u3cWR2JsjzPp9gvHjzrArDiA
