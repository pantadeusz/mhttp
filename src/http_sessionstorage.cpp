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

#include "http_sessionstorage.hpp"

#include "http_request.hpp"
#include "http_response.hpp"

#include <iostream>

namespace tp {
namespace http {


std::string StringSessionData::toString() {
	return data;
}
void StringSessionData::fromString( const std::string  &str ) {
	data = str;
}

i_SessionData *Session::getSessionData() {
	return data.get();
}

void Session::setSessionData( i_SessionData *d ) {
	data = std::shared_ptr < i_SessionData >( d );
}
std::string Session::getId() const {
	return _id;
}
void Session::setId( const std::string &id ) {
	_id = id;
}
double Session::getSecondsOfLife() {
	return std::chrono::duration_cast<std::chrono::seconds>(
			   std::chrono::system_clock::now() - createTime ).count();
}
Session::Session() {
	createTime = std::chrono::system_clock::now();
}

}
}
