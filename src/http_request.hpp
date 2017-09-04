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


#ifndef __HTTP_REQUEST__TP__HPP___
#define  __HTTP_REQUEST__TP__HPP___

#include <vector>
#include <string>
#include <map>


namespace tp {
namespace http {


typedef class Request {
public:

	std::vector < std::string > pathSubmatches;

	std::string method;
	std::string proto;
	std::string queryString;

	std::map < std::string, std::string > header;
	std::vector <char> content;
	std::string remoteAddress;

	Request();


	std::map<std::string, std::string> getParams(  );
	std::string getContentType ();
	std::string getPath();
	size_t getContentLength();
	std::string getContentString();
	void setContent(const std::string &s);
} Request;

}
}

#endif
