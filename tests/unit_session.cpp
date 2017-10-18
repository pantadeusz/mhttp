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

#include <http_session.hpp>
#include <catch.hpp>
#include <fakeit.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>


using namespace fakeit;
using namespace tp::http;

TEST_CASE( "http server with session support", "[mhttp][http][session]" ) {
    errlog = [](const std::string & e){};
    stdlog = [](const std::string & e){};
    static int port = 10990;
    port++;
    HttpWithSession srv( "localhost", port, true );
    
    srv.GET( "/", []( Request &req, Session session )->t_Response {
        return ResponseFactory::response(session.getId());
    } );

    std::thread t([&](){srv.start();});
    t.detach();

    SECTION("setting up session cookie") {
        Request req;
        req.method = "GET";
        req.proto = "HTTP/1.1";
        req.queryString = "/";
        req.remoteAddress = "localhost:" + std::to_string(port);
        t_Response res1 = Http::doHttpQuery(req);
        std::stringstream ss;
        ss << res1;
        std::string sessionID = ss.str();
        for (auto h : res1.getHeader()) {
            std::cout << "h: " << h.first << " = " << h.second << std::endl;
        }
        REQUIRE(("sessionId="+sessionID)==res1.getHeader()["set-cookie"]);
    }

    srv.stop();

}
