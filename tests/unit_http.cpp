/*

    Copyright (C) 2017 Tadeusz Puźniakowski

    This file is part of mhttp.

	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	you can contact me via http://www.puzniakowski.pl

*/

#include <http.hpp>
#include <catch.hpp>
#include <fakeit.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>


using namespace fakeit;
using namespace tp::http;

TEST_CASE( "http server full requests with ports and so on", "[mhttp][http]" ) {
    errlog = [](const std::string & ){};
    stdlog = [](const std::string & ){};
    static int port = 9990;
    port++;
	Http srv( "localhost", port, true );

    srv.filter_GET("/", [&]( Request &req )->void {
        req.queryString = "/hello";
    });
    srv.GET( "/hello", []( Request &req )->Response {
        return ResponseFactory::response("Hello in my world");
    } );
    srv.GET( "/t", []( Request &req )->Response {
        auto params = req.getParams();
        return ResponseFactory::response("Hello in my world " + params["name"]);
    } );
    srv.POST( "/t", []( Request &req )->Response {
        auto params = req.getParams();
        return ResponseFactory::response("Hello in my world " + params["a"] + " i " + params["lala"]);
    } );
    srv.GET("/(.*)",getStaticFileHandler("tests/",true));


    std::thread t([&](){srv.start();});
    t.detach();

    SECTION("simple get request for using filter /hello") {
        Request req;
        req.method = "GET";
        req.proto = "HTTP/1.1";
        req.queryString = "/";
        req.remoteAddress = "localhost:" + std::to_string(port);
        Response res = Http::doHttpQuery(req);
        std::stringstream ss;
        ss << res;
        REQUIRE(ss.str()=="Hello in my world");
    }

    SECTION("simple get request for /hello") {
        Request req;
        req.method = "GET";
        req.proto = "HTTP/1.1";
        req.queryString = "/hello";
        req.remoteAddress = "localhost:" + std::to_string(port);
        Response res = Http::doHttpQuery(req);
        std::stringstream ss;
        ss << res;
        REQUIRE(ss.str()=="Hello in my world");
    }
    
    SECTION("simple get request for /units.cpp") {
        Request req;
        req.method = "GET";
        req.proto = "HTTP/1.1";
        req.queryString = "/units.cpp";
        req.remoteAddress = "localhost:" + std::to_string(port);
        Response res = Http::doHttpQuery(req);
        std::stringstream ss;
        ss << res;
        std::ifstream t("tests/units.cpp");
        std::string teststr((std::istreambuf_iterator<char>(t)),
                         std::istreambuf_iterator<char>());
        REQUIRE(ss.str()==teststr);
    }

    SECTION("get /t with params") {
        Request req;
        req.method = "GET";
        req.proto = "HTTP/1.1";
        req.queryString = "/t?name=gerwazy";
        req.remoteAddress = "localhost:" + std::to_string(port);
        Response res = Http::doHttpQuery(req);
        std::stringstream ss;
        ss << res;
        REQUIRE(ss.str()=="Hello in my world gerwazy" );
    }

    SECTION("post /t with params in body") {
        Request req;
        req.method = "POST";
        req.proto = "HTTP/1.1";
        req.queryString = "/t";
        req.remoteAddress = "localhost:" + std::to_string(port);
        req.header["content-type"] = "application/x-www-form-urlencoded";
        req.header["content-length"] = "3";
        req.setContent("lala=b%26d%26%20ty&a=b&jan=nowak");
        Response res = Http::doHttpQuery(req);
        std::stringstream ss;
        ss << res;
        REQUIRE(ss.str()=="Hello in my world b i b&d& ty" );
    }

    srv.stop();

}
