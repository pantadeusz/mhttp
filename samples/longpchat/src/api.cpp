/*

    Copyright (C) 2017 Tadeusz Puźniakowski

    This file is part of gcdapi.

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

#include "api.hpp"
#include <mhttp/http.hpp>
#include <json.hpp>

#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
using namespace tp::http;

using json = nlohmann::json;

namespace tp {

WebApi::WebApi( int port ) : srv( "0.0.0.0", port, true )    {
	srv.filter_GET( "/", [&]( Request & req )->void {
		req.queryString = "/index.html";
	} );
	srv.POST( "/api/message", [&]( Request & req )->Response {
		auto msg = req.getContentString();
		msg_.send_message( msg );
		return ResponseFactory::response( msg );
	} );
	srv.GET( "/api/message", [&]( Request & req )->Response {
		auto p = req.getParams( );
		json ret;
		if ( p.count( "id" ) ) {
			auto msgs = msg_.wait_message( p["id"] );
			ret["messages"] = msgs;
			return ResponseFactory::response( ret.dump() );
		} else {
			return ResponseFactory::response( "you should provide your id" );
		}

	} );

	srv.GET( "/api/token", [&]( Request & req )->Response {
		static int tokenv = 0;
		tokenv ++;
		return ResponseFactory::response( std::to_string( tokenv ) );

	} );
	srv.GET( "/(.*)", getStaticFileHandler( "www/", true ) );
}
void WebApi::start() {
	srv.start();
}
void WebApi::stop() {
	srv.stop();
}
WebApi::~WebApi() {

}

}

////////////////////////////////////////////////////////////////////////////////////

tp::WebApi *api_ptr;

void exit_handler( int ) {
	api_ptr->stop();
}

int main() {
	tp::WebApi api( 5555 );
	api_ptr = &api;
	struct sigaction sa;
	sa.sa_handler = exit_handler;
	sigemptyset( &sa.sa_mask );
	sa.sa_flags = 0;
	sigaction( SIGINT, &sa, NULL );
	api.start();
	return 0;
}
