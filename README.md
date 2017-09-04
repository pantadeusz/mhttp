# phttp


### by Tadeusz Puźniakowski, 2017

Very simple implementation of HTTP server allowing you to write your code in C++11.
The intention of this project is to provide lightweight server for 
Internet of Things applications, where there is need to access hardware
directly via C or C++ code.

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
}
```


### donations

I accept donations in BTC at 13CfATaTXvh5aVn4cfcgGHEo67rtuwVqnF
