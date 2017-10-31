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

#include <hhelpers.hpp>
#include <catch.hpp>
#include <fakeit.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>


using namespace fakeit;
using namespace tp::http;

TEST_CASE( "base64 decoding", "[base64]" ) {
    std::string testowe = "data:text/gcode;base64,dGVzdG93ZQ==";
    std::string testowe_nohead = "dGVzdG93ZQ==";

    std::string testowexx = "data:text/gcode;base64,dGVzdG93ZXh4";
    std::string testowexx_nohead = "dGVzdG93ZXh4";

    std::string testowex = "data:text/gcode;base64,dGVzdG93ZXg=";
    std::string testowex_nohead = "dGVzdG93ZXg=";
    
    SECTION("decoded testowexx") {
        auto testoweFromBase64 = base64tobin(testowexx);        
        auto testoweStr = std::string(testoweFromBase64.begin(), testoweFromBase64.end());
        REQUIRE(testoweStr=="testowexx");
    }
    SECTION("decoded testowex") {
        auto testoweFromBase64 = base64tobin(testowex);        
        auto testoweStr = std::string(testoweFromBase64.begin(), testoweFromBase64.end());
        REQUIRE(testoweStr=="testowex");
    }
    SECTION("decoded testowe") {
        auto testoweFromBase64 = base64tobin(testowe);        
        auto testoweStr = std::string(testoweFromBase64.begin(), testoweFromBase64.end());
        REQUIRE(testoweStr=="testowe");
    }

}
