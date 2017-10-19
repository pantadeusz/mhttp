
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

#ifndef __PUZNIAKOWSKI_SESSION_HTTP__
#define __PUZNIAKOWSKI_SESSION_HTTP__

#include "http_sessionstorage.hpp"
#include "http.hpp"
#include "http_request.hpp"
#include "http_response.hpp"

#include <chrono>
#include <ctime>
#include <unordered_map>
#include <random>


namespace tp {
namespace http {

class HttpWithSession : public Http {
private:
public:
	std::unique_ptr < i_SessionStorage > sessionStorage;

	Session &getSession( tp::http::Request &req );
	tp::http::t_Response saveSession( Session &session, tp::http::t_Response &res );

	void sGET( const std::string &mapping, std::function < t_Response ( Request &, Session & ) > f );
	void sPOST( const std::string &mapping, std::function < t_Response ( Request &, Session & ) > f );
	void sDELETE( const std::string &mapping, std::function < t_Response ( Request &, Session & ) > f );
    
    void filter_sGET( const std::string &mapping, std::function < void( Request &, Session & ) > f );
    void filter_sPOST( const std::string &mapping, std::function < void( Request &, Session & ) > f );
    void filter_sDELETE( const std::string &mapping, std::function < void( Request &, Session & ) > f );

	HttpWithSession  ( std::string hostname = "localhost", int port = 8080, int async = false, i_SessionStorage *storage = NULL );
};

}
}


#endif
