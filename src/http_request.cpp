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


#include "http_request.hpp"
#include "hhelpers.hpp"

#include <algorithm>
#include <regex>


namespace tp {
    namespace http {
    

Request::Request() {
    //	mime = "application/x-www-form-urlencoded";
}

std::string Request::getContentType () {
	return header["content-type"];
}

size_t Request::getContentLength() {
	if (header.count("content-length") > 0)
		return std::stoul(header["content-length"]);
	else
		return 0;
}
void Request::setContent(const std::string &s) {
	content = std::vector<char>(s.begin(),s.end());
	header["content-length"] = std::to_string(s.length());
}
std::string Request::getContentString() { 
	return std::string(content.begin(),content.end());
}

std::map<std::string, std::string> Request::getParams(  ) {
	std::string uristr = "";
	if (getContentType() == "application/x-www-form-urlencoded") {
		uristr = std::string(content.begin(), content.end());
	} else {
		auto i = queryString.find("?");
		if (i != std::string::npos) {
			uristr = queryString.substr(i+1);
		}
	}
	std::vector<std::string> pairs;
	std::map<std::string, std::string> kvr;

	std::regex rex("[&]");
	pairs = std::vector<std::string>(std::sregex_token_iterator (uristr.begin(), uristr.end(), rex, -1), std::sregex_token_iterator());
	for (auto &pair : pairs) {
		auto k = uridecode(pair.substr(0,pair.find("="))); 
		auto v = uridecode(pair.substr(pair.find("=")+1));
		kvr[k]=v;
	}
	return kvr;
}

// // deprecated
// bool Request::isKeepAlive() {
	// if (header.count("connection") > 0) {
		// return header["connection"] == "keep-alive";
	// }
	// return false;
// };

std::string Request::getPath() {
    std::string ret;
    auto idx = queryString.find('?');
    if (idx == std::string::npos) ret = queryString;
    else ret = queryString.substr(0, idx);
    if (ret.length() <= 0) ret = "/";
    return ret;
};


}
}
