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

#include "http_memorysessionstorage.hpp"
#include "http_session.hpp"
#include "http.hpp"
#include "http_request.hpp"
#include "http_response.hpp"


#include <iostream>

namespace tp {
namespace http {



Session &HttpWithSession::getSession( tp::http::Request &req ) {
	return sessionStorage.get()->getSessionForRequest( req );
}
tp::http::t_Response HttpWithSession::saveSession( Session &session, tp::http::t_Response &res ) {
	return sessionStorage.get()->storeSessionForRequest( session, res );
}
HttpWithSession::HttpWithSession  ( std::string hostname, int port, int async, i_SessionStorage *storage)
	: Http( hostname, port, async ) {
		if (storage == NULL) {
			storage = new MemorySessionStorage();
		}
	sessionStorage = std::unique_ptr<i_SessionStorage>(storage);
}



void HttpWithSession::sGET( const std::string &mapping, std::function < t_Response ( Request &, Session & ) > f ) {
	Http::GET( mapping, [f, this]( Request & req ) -> t_Response {
		auto &session = getSession( req );
		auto res = f( req, session );
		auto ret = saveSession( session, res );
		return ret;
	} );
}

void HttpWithSession::sPOST( const std::string &mapping, std::function < t_Response ( Request &, Session & ) > f ) {
	Http::POST( mapping, [f, this]( Request & req ) -> t_Response {
		auto &session = getSession( req );
		auto res = f( req, session );
		auto ret = saveSession( session, res );
		return ret;
		} );
}


void HttpWithSession::sDELETE( const std::string &mapping, std::function < t_Response ( Request &, Session & ) > f ) {
	Http::DELETE( mapping, [f, this]( Request & req ) -> t_Response {
		auto &session = getSession( req );
		auto res = f( req, session );
		auto ret = saveSession( session, res );
		return ret;
	} );
}


void HttpWithSession::filter_sGET( const std::string &mapping, std::function < void( Request &, Session & ) > f ) {
	Http::filter_GET( mapping, [f, this]( Request & req ) {
		auto &session = getSession( req );
		if (req.header["cookie"].size() > 0) {			
			if (req.header["cookie"].find("sessionId") == std::string::npos) {
				req.header["cookie"] = req.header["cookie"] + "; sessionId="+session.getId();
			}
		} else {
			req.header["cookie"] = "sessionId="+session.getId();
		}
		f( req, session );
		sessionStorage.get()->storeSession( session );
	} );
}
void HttpWithSession::filter_sPOST( const std::string &mapping, std::function < void( Request &, Session & ) > f ){
	Http::filter_POST( mapping, [f, this]( Request & req ) {
		auto &session = getSession( req );
		if (req.header["cookie"].size() > 0) {
			if (req.header["cookie"].find("sessionId") == std::string::npos) {
				req.header["cookie"] = req.header["cookie"] + "; sessionId="+session.getId();
			}
		} else {
			req.header["cookie"] = "sessionId="+session.getId();
		}
		f( req, session );
		sessionStorage.get()->storeSession( session );
	} );
}
void HttpWithSession::filter_sDELETE( const std::string &mapping, std::function < void( Request &, Session & ) > f ){
	Http::filter_DELETE( mapping, [f, this]( Request & req ) {
		auto &session = getSession( req );
		if (req.header["cookie"].size() > 0) {
			if (req.header["cookie"].find("sessionId") == std::string::npos) {
				req.header["cookie"] = req.header["cookie"] + "; sessionId="+session.getId();
			}
		} else {
			req.header["cookie"] = "sessionId="+session.getId();
		}
		f( req, session );
		sessionStorage.get()->storeSession( session );
	} );
}

}
}
