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

#ifndef ___HHELPERS_HPP___
#define ___HHELPERS_HPP___

#include <vector>
#include <string>
#include <functional>

namespace tp {
  namespace http {
  std::vector<char> base64tobin(const std::string &s);
  std::string filenameProtect(const std::string &s);


  // after http://stackoverflow.com/questions/236129/split-a-string-in-c
  std::vector<std::string> &split( const std::string &s, const char delim, std::vector<std::string> &elems );

  // after http://stackoverflow.com/questions/236129/split-a-string-in-c
  std::vector<std::string> split( const std::string &s, char delim );

  // after http://stackoverflow.com/
  std::string trim( const std::string& str, const std::string& whitespace = "\n \t\r" );

  // returns file modification time
  unsigned long int getFileModTime(const std::string &fname);

  std::string uridecode( const std::string &uristr );  

	const std::string &getMimeType(const std::string &fileName);
  



}
}

#endif
