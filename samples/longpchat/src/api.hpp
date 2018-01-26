/**
 *
 *
 * API should be compatible with: http://jsonapi.org/format/
 *
 * */

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


#ifndef __TP_A_WEBAPI_GCD__HPP_____
#define __TP_A_WEBAPI_GCD__HPP_____

#include "msgservice.hpp"

#include <mhttp/http.hpp>

#include <thread>

namespace tp {



class WebApi {
protected:
	services::message_service<std::string> msg_;

public:
	tp::http::Http srv;
	std::thread t;

	virtual ~WebApi();
	void start();
	void stop();
	WebApi( int port );
};


}


#endif
