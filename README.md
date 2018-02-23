# mhttp

[![Build Status](https://travis-ci.org/pantadeusz/mhttp.svg?branch=master)](https://travis-ci.org/pantadeusz/mhttp)

### by Tadeusz Puźniakowski, 2017

Very simple implementation of HTTP server allowing you to write your code in C++11.
The intention of this project is to provide lightweight server for 
Internet of Things applications, where there is need to access hardware
directly via C or C++ code.

## Example projects

  [samples](samples)


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


### sample server

```c++
#include <iostream>
#include <sstream>
#include <string>
#include <http.hpp>
#include <chrono>
#include <thread>
#include <fstream>

using namespace tp::http;
using namespace std;

int main ( int argc, char **argv ) {
    Http srv( "localhost", 8090, true );

    srv.filter_GET( "/", [&]( Request & req )->void {
        req.queryString = "/index.html";
    } );

    srv.GET( "/hello", []( Request & req )->Response {
        std::stringstream ss;
        ss << "<p>Hello world!!</p><table>";
        for ( auto p : req.getParams() ) {
            ss << "<tr><td>" << p.first << "</td><td>" << p.second << "<td></tr>\r\n";
        }
        ss << "</table>";
        return ResponseFactory::response( ss.str(), 200, "OK" );
    } );

    srv.GET( "/(.*)", getStaticFileHandler( "", true ) );

    std::thread t( [&]() {
        srv.start();
    } );
    t.detach();

    std::cout << "---server " << VER << " started. type some text to exit --" << std::endl;
    std::string i;
    std::cin >> i;
    std::cout << "Closing server" << std::endl;
    srv.stop();
    std::cout << "Closing server - OK" << std::endl;
    return 0;
}
```



### C++14 features

You can use C++14 features, for shorter notation of lambdas:
```c++
    srv.filter_sGET("/", [&]( auto &req, auto & )->auto {
        req.queryString = "/index.html";
    });
```


### donations

I accept donations in BTC at bc1q2rs40jcs9849hyph97am48xewzdf8ctfr6hsmx
