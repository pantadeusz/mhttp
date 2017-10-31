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


#include "hhelpers.hpp"


#include <sys/stat.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iostream>
#include <map>
#include <algorithm>


namespace tp {
namespace http {


std::string uridecode( const std::string &uristr ) {
	std::string retstr;
	retstr.reserve( uristr.length() * 2 );
	for ( unsigned i = 0; i < uristr.length(); i++ ) {
		if ( uristr[i] == '%' && i < ( uristr.length() - 2 ) ) {
			char num[3];
			num[0] = ( char )uristr[i + 1];
			num[1] = ( char )uristr[i + 2];
			num[2] = 0;
			char c = strtoul( num, NULL, 16 );
			retstr = retstr + c;
			i += 2;
		} else if ( uristr[i] == '+' ) {
			retstr = retstr + ' ';
		} else {
			retstr = retstr + uristr[i];
		}
	}
	return retstr;
}


std::vector<char> base64tobin( const std::string &s ) {
	std::vector<char> ret;
	ret.reserve( s.length() );
	unsigned int val = 0;
	int pads = 0;
	int bitsCount = 0;
	for ( int i = s.find( ',' ) + 1; i < ( int )s.size(); i ++ ) {
		int v = 0;
		int c = s[i];
		if ( ( c >= 'A' ) && ( c <= 'Z' ) )
			v = c - 'A';
		else if ( ( c >= 'a' ) && ( c <= 'z' ) )
			v = c - 'a' + 26;
		else if ( ( c >= '0' ) && ( c <= '9' ) )
			v = c - '0' + 52;
		else if ( c == '+' )
			v = 62;
		else if ( c == '/' )
			v = 63;
		else if ( c == '=' )
			pads++;
		val = ( val << 6 ) + v;
		bitsCount += 6;

		if (bitsCount >= 24) {
			ret.push_back( (val >> 16) & 0x0ff );
			if (pads < 2) ret.push_back( (val >> 8) & 0x0ff );
			if (pads < 1) ret.push_back( val & 0x0ff );
			val = val >> 24;
			bitsCount -=24;
		}
	}
	return ret;
}

std::string filenameProtect( const std::string &s ) {
	std::string projname = s;
	if ( projname[0] == '/' ) projname = projname.substr( 1 );
	for ( std::size_t found = projname.find( "../" ); found != std::string::npos;
			found = projname.find( "../", found ) ) {
		projname[found] = '_';
		projname[found + 1] = '_';
		projname[found + 2] = '_';
	}
	return projname;
}


unsigned long int getFileModTime( const std::string &fname ) {
	struct stat sb;
	stat( fname.c_str(), &sb );
	return sb.st_mtime;
}



// after http://stackoverflow.com/questions/236129/split-a-string-in-c
std::vector<std::string> &split( const std::string &s, const char delim, std::vector<std::string> &elems ) {
	std::stringstream ss( s );
	std::string item;
	while ( std::getline( ss, item, delim ) ) {
		elems.push_back( item );
	}
	return elems;
}
// after http://stackoverflow.com/questions/236129/split-a-string-in-c
std::vector<std::string> split( const std::string &s, char delim ) {
	std::vector<std::string> elems;
	split( s, delim, elems );
	return elems;
}

// after http://stackoverflow.com/
std::string trim( const std::string& str,
				  const std::string& whitespace ) {
	const auto strBegin = str.find_first_not_of( whitespace );
	if ( strBegin == std::string::npos )
		return ""; // no content
	const auto strEnd = str.find_last_not_of( whitespace );
	const auto strRange = strEnd - strBegin + 1;
	return str.substr( strBegin, strRange );
}


const std::string &getMimeType( const std::string &fileName ) {
	static std::map <std::string, std::string > mimeTypesMap;
	if ( mimeTypesMap.size() == 0 ) {
		mimeTypesMap["."] = "application/octet-stream";
		mimeTypesMap["txt"] = "text/plain";
		mimeTypesMap["cpp"] = "text/plain";
		mimeTypesMap["css"] = "text/css";
		mimeTypesMap["json"] = "application/json";
		mimeTypesMap["html"] = "text/html";
		mimeTypesMap["htm"] = "text/html";
		mimeTypesMap["blend"] = "application/blender";
		mimeTypesMap["3dm"] = "x-world/x-3dmf";
		mimeTypesMap["3dmf"] = "x-world/x-3dmf";
		mimeTypesMap["a"] = "application/octet-stream";
		mimeTypesMap["aab"] = "application/x-authorware-bin";
		mimeTypesMap["aam"] = "application/x-authorware-map";
		mimeTypesMap["aas"] = "application/x-authorware-seg";
		mimeTypesMap["abc"] = "text/vnd.abc";
		mimeTypesMap["acgi"] = "text/html";
		mimeTypesMap["afl"] = "video/animaflex";
		mimeTypesMap["ai"] = "application/postscript";
		mimeTypesMap["aif"] = "audio/aiff";
		mimeTypesMap["aif"] = "audio/x-aiff";
		mimeTypesMap["aifc"] = "audio/aiff";
		mimeTypesMap["aifc"] = "audio/x-aiff";
		mimeTypesMap["aiff"] = "audio/aiff";
		mimeTypesMap["aiff"] = "audio/x-aiff";
		mimeTypesMap["aim"] = "application/x-aim";
		mimeTypesMap["aip"] = "text/x-audiosoft-intra";
		mimeTypesMap["ani"] = "application/x-navi-animation";
		mimeTypesMap["aos"] = "application/x-nokia-9000-communicator-add-on-software";
		mimeTypesMap["aps"] = "application/mime";
		mimeTypesMap["arc"] = "application/octet-stream";
		mimeTypesMap["arj"] = "application/arj";
		mimeTypesMap["arj"] = "application/octet-stream";
		mimeTypesMap["art"] = "image/x-jg";
		mimeTypesMap["asf"] = "video/x-ms-asf";
		mimeTypesMap["asm"] = "text/x-asm";
		mimeTypesMap["asp"] = "text/asp";
		mimeTypesMap["asx"] = "application/x-mplayer2";
		mimeTypesMap["asx"] = "video/x-ms-asf";
		mimeTypesMap["asx"] = "video/x-ms-asf-plugin";
		mimeTypesMap["au"] = "audio/basic";
		mimeTypesMap["au"] = "audio/x-au";
		mimeTypesMap["avi"] = "application/x-troff-msvideo";
		mimeTypesMap["avi"] = "video/avi";
		mimeTypesMap["avi"] = "video/msvideo";
		mimeTypesMap["avi"] = "video/x-msvideo";
		mimeTypesMap["avs"] = "video/avs-video";
		mimeTypesMap["bcpio"] = "application/x-bcpio";
		mimeTypesMap["bin"] = "application/mac-binary";
		mimeTypesMap["bin"] = "application/macbinary";
		mimeTypesMap["bin"] = "application/octet-stream";
		mimeTypesMap["bin"] = "application/x-binary";
		mimeTypesMap["bin"] = "application/x-macbinary";
		mimeTypesMap["bm"] = "image/bmp";
		mimeTypesMap["bmp"] = "image/bmp";
		mimeTypesMap["bmp"] = "image/x-windows-bmp";
		mimeTypesMap["boo"] = "application/book";
		mimeTypesMap["book"] = "application/book";
		mimeTypesMap["boz"] = "application/x-bzip2";
		mimeTypesMap["bsh"] = "application/x-bsh";
		mimeTypesMap["bz"] = "application/x-bzip";
		mimeTypesMap["bz2"] = "application/x-bzip2";
		mimeTypesMap["c"] = "text/plain";
		mimeTypesMap["c"] = "text/x-c";
		mimeTypesMap["c++"] = "text/plain";
		mimeTypesMap["cat"] = "application/vnd.ms-pki.seccat";
		mimeTypesMap["cc"] = "text/plain";
		mimeTypesMap["cc"] = "text/x-c";
		mimeTypesMap["ccad"] = "application/clariscad";
		mimeTypesMap["cco"] = "application/x-cocoa";
		mimeTypesMap["cdf"] = "application/cdf";
		mimeTypesMap["cdf"] = "application/x-cdf";
		mimeTypesMap["cdf"] = "application/x-netcdf";
		mimeTypesMap["cer"] = "application/pkix-cert";
		mimeTypesMap["cer"] = "application/x-x509-ca-cert";
		mimeTypesMap["cha"] = "application/x-chat";
		mimeTypesMap["chat"] = "application/x-chat";
		mimeTypesMap["class"] = "application/java";
		mimeTypesMap["class"] = "application/java-byte-code";
		mimeTypesMap["class"] = "application/x-java-class";
		mimeTypesMap["com"] = "application/octet-stream";
		mimeTypesMap["com"] = "text/plain";
		mimeTypesMap["conf"] = "text/plain";
		mimeTypesMap["cpio"] = "application/x-cpio";
		mimeTypesMap["cpp"] = "text/x-c";
		mimeTypesMap["cpt"] = "application/mac-compactpro";
		mimeTypesMap["cpt"] = "application/x-compactpro";
		mimeTypesMap["cpt"] = "application/x-cpt";
		mimeTypesMap["crl"] = "application/pkcs-crl";
		mimeTypesMap["crl"] = "application/pkix-crl";
		mimeTypesMap["crt"] = "application/pkix-cert";
		mimeTypesMap["crt"] = "application/x-x509-ca-cert";
		mimeTypesMap["crt"] = "application/x-x509-user-cert";
		mimeTypesMap["csh"] = "application/x-csh";
		mimeTypesMap["csh"] = "text/x-script.csh";
		mimeTypesMap["css"] = "application/x-pointplus";
		mimeTypesMap["css"] = "text/css";
		mimeTypesMap["cxx"] = "text/plain";
		mimeTypesMap["dcr"] = "application/x-director";
		mimeTypesMap["deepv"] = "application/x-deepv";
		mimeTypesMap["def"] = "text/plain";
		mimeTypesMap["der"] = "application/x-x509-ca-cert";
		mimeTypesMap["dif"] = "video/x-dv";
		mimeTypesMap["dir"] = "application/x-director";
		mimeTypesMap["dl"] = "video/dl";
		mimeTypesMap["dl"] = "video/x-dl";
		mimeTypesMap["doc"] = "application/msword";
		mimeTypesMap["dot"] = "application/msword";
		mimeTypesMap["dp"] = "application/commonground";
		mimeTypesMap["drw"] = "application/drafting";
		mimeTypesMap["dump"] = "application/octet-stream";
		mimeTypesMap["dv"] = "video/x-dv";
		mimeTypesMap["dvi"] = "application/x-dvi";
		mimeTypesMap["dwf"] = "drawing/x-dwf (old)";
		mimeTypesMap["dwf"] = "model/vnd.dwf";
		mimeTypesMap["dwg"] = "application/acad";
		mimeTypesMap["dwg"] = "image/vnd.dwg";
		mimeTypesMap["dwg"] = "image/x-dwg";
		mimeTypesMap["dxf"] = "application/dxf";
		mimeTypesMap["dxf"] = "image/vnd.dwg";
		mimeTypesMap["dxf"] = "image/x-dwg";
		mimeTypesMap["dxr"] = "application/x-director";
		mimeTypesMap["el"] = "text/x-script.elisp";
		mimeTypesMap["elc"] = "application/x-bytecode.elisp (compiled elisp)";
		mimeTypesMap["elc"] = "application/x-elc";
		mimeTypesMap["env"] = "application/x-envoy";
		mimeTypesMap["eps"] = "application/postscript";
		mimeTypesMap["es"] = "application/x-esrehber";
		mimeTypesMap["etx"] = "text/x-setext";
		mimeTypesMap["evy"] = "application/envoy";
		mimeTypesMap["evy"] = "application/x-envoy";
		mimeTypesMap["exe"] = "application/octet-stream";
		mimeTypesMap["f"] = "text/plain";
		mimeTypesMap["f"] = "text/x-fortran";
		mimeTypesMap["f77"] = "text/x-fortran";
		mimeTypesMap["f90"] = "text/plain";
		mimeTypesMap["f90"] = "text/x-fortran";
		mimeTypesMap["fdf"] = "application/vnd.fdf";
		mimeTypesMap["fif"] = "application/fractals";
		mimeTypesMap["fif"] = "image/fif";
		mimeTypesMap["fli"] = "video/fli";
		mimeTypesMap["fli"] = "video/x-fli";
		mimeTypesMap["flo"] = "image/florian";
		mimeTypesMap["flx"] = "text/vnd.fmi.flexstor";
		mimeTypesMap["fmf"] = "video/x-atomic3d-feature";
		mimeTypesMap["for"] = "text/plain";
		mimeTypesMap["for"] = "text/x-fortran";
		mimeTypesMap["fpx"] = "image/vnd.fpx";
		mimeTypesMap["fpx"] = "image/vnd.net-fpx";
		mimeTypesMap["frl"] = "application/freeloader";
		mimeTypesMap["funk"] = "audio/make";
		mimeTypesMap["g"] = "text/plain";
		mimeTypesMap["g3"] = "image/g3fax";
		mimeTypesMap["gif"] = "image/gif";
		mimeTypesMap["gl"] = "video/gl";
		mimeTypesMap["gl"] = "video/x-gl";
		mimeTypesMap["gsd"] = "audio/x-gsm";
		mimeTypesMap["gsm"] = "audio/x-gsm";
		mimeTypesMap["gsp"] = "application/x-gsp";
		mimeTypesMap["gss"] = "application/x-gss";
		mimeTypesMap["gtar"] = "application/x-gtar";
		mimeTypesMap["gz"] = "application/x-compressed";
		mimeTypesMap["gz"] = "application/x-gzip";
		mimeTypesMap["gzip"] = "application/x-gzip";
		mimeTypesMap["gzip"] = "multipart/x-gzip";
		mimeTypesMap["h"] = "text/plain";
		mimeTypesMap["h"] = "text/x-h";
		mimeTypesMap["hdf"] = "application/x-hdf";
		mimeTypesMap["help"] = "application/x-helpfile";
		mimeTypesMap["hgl"] = "application/vnd.hp-hpgl";
		mimeTypesMap["hh"] = "text/plain";
		mimeTypesMap["hh"] = "text/x-h";
		mimeTypesMap["hlb"] = "text/x-script";
		mimeTypesMap["hlp"] = "application/hlp";
		mimeTypesMap["hlp"] = "application/x-helpfile";
		mimeTypesMap["hlp"] = "application/x-winhelp";
		mimeTypesMap["hpg"] = "application/vnd.hp-hpgl";
		mimeTypesMap["hpgl"] = "application/vnd.hp-hpgl";
		mimeTypesMap["hqx"] = "application/binhex";
		mimeTypesMap["hqx"] = "application/binhex4";
		mimeTypesMap["hqx"] = "application/mac-binhex";
		mimeTypesMap["hqx"] = "application/mac-binhex40";
		mimeTypesMap["hqx"] = "application/x-binhex40";
		mimeTypesMap["hqx"] = "application/x-mac-binhex40";
		mimeTypesMap["hta"] = "application/hta";
		mimeTypesMap["htc"] = "text/x-component";
		mimeTypesMap["htm"] = "text/html";
		mimeTypesMap["html"] = "text/html";
		mimeTypesMap["htmls"] = "text/html";
		mimeTypesMap["htt"] = "text/webviewhtml";
		mimeTypesMap["htx"] = "text/html";
		mimeTypesMap["ice"] = "x-conference/x-cooltalk";
		mimeTypesMap["ico"] = "image/x-icon";
		mimeTypesMap["idc"] = "text/plain";
		mimeTypesMap["ief"] = "image/ief";
		mimeTypesMap["iefs"] = "image/ief";
		mimeTypesMap["iges"] = "application/iges";
		mimeTypesMap["iges"] = "model/iges";
		mimeTypesMap["igs"] = "application/iges";
		mimeTypesMap["igs"] = "model/iges";
		mimeTypesMap["ima"] = "application/x-ima";
		mimeTypesMap["imap"] = "application/x-httpd-imap";
		mimeTypesMap["inf"] = "application/inf";
		mimeTypesMap["ins"] = "application/x-internett-signup";
		mimeTypesMap["ip"] = "application/x-ip2";
		mimeTypesMap["isu"] = "video/x-isvideo";
		mimeTypesMap["it"] = "audio/it";
		mimeTypesMap["iv"] = "application/x-inventor";
		mimeTypesMap["ivr"] = "i-world/i-vrml";
		mimeTypesMap["ivy"] = "application/x-livescreen";
		mimeTypesMap["jam"] = "audio/x-jam";
		mimeTypesMap["jav"] = "text/plain";
		mimeTypesMap["jav"] = "text/x-java-source";
		mimeTypesMap["java"] = "text/plain";
		mimeTypesMap["java"] = "text/x-java-source";
		mimeTypesMap["jcm"] = "application/x-java-commerce";
		mimeTypesMap["jfif"] = "image/jpeg";
		mimeTypesMap["jfif"] = "image/pjpeg";
		mimeTypesMap["jfif-tbnl"] = "image/jpeg";
		mimeTypesMap["jpe"] = "image/jpeg";
		mimeTypesMap["jpe"] = "image/pjpeg";
		mimeTypesMap["jpeg"] = "image/jpeg";
		mimeTypesMap["jpeg"] = "image/pjpeg";
		mimeTypesMap["jpg"] = "image/jpeg";
		mimeTypesMap["jpg"] = "image/pjpeg";
		mimeTypesMap["jps"] = "image/x-jps";
		mimeTypesMap["js"] = "application/x-javascript";
		mimeTypesMap["js"] = "application/javascript";
		mimeTypesMap["js"] = "application/ecmascript";
		mimeTypesMap["js"] = "text/javascript";
		mimeTypesMap["js"] = "text/ecmascript";
		mimeTypesMap["jut"] = "image/jutvision";
		mimeTypesMap["kar"] = "audio/midi";
		mimeTypesMap["kar"] = "music/x-karaoke";
		mimeTypesMap["ksh"] = "application/x-ksh";
		mimeTypesMap["ksh"] = "text/x-script.ksh";
		mimeTypesMap["la"] = "audio/nspaudio";
		mimeTypesMap["la"] = "audio/x-nspaudio";
		mimeTypesMap["lam"] = "audio/x-liveaudio";
		mimeTypesMap["latex"] = "application/x-latex";
		mimeTypesMap["lha"] = "application/lha";
		mimeTypesMap["lha"] = "application/octet-stream";
		mimeTypesMap["lha"] = "application/x-lha";
		mimeTypesMap["lhx"] = "application/octet-stream";
		mimeTypesMap["list"] = "text/plain";
		mimeTypesMap["lma"] = "audio/nspaudio";
		mimeTypesMap["lma"] = "audio/x-nspaudio";
		mimeTypesMap["log"] = "text/plain";
		mimeTypesMap["lsp"] = "application/x-lisp";
		mimeTypesMap["lsp"] = "text/x-script.lisp";
		mimeTypesMap["lst"] = "text/plain";
		mimeTypesMap["lsx"] = "text/x-la-asf";
		mimeTypesMap["ltx"] = "application/x-latex";
		mimeTypesMap["lzh"] = "application/octet-stream";
		mimeTypesMap["lzh"] = "application/x-lzh";
		mimeTypesMap["lzx"] = "application/lzx";
		mimeTypesMap["lzx"] = "application/octet-stream";
		mimeTypesMap["lzx"] = "application/x-lzx";
		mimeTypesMap["m"] = "text/plain";
		mimeTypesMap["m"] = "text/x-m";
		mimeTypesMap["m1v"] = "video/mpeg";
		mimeTypesMap["m2a"] = "audio/mpeg";
		mimeTypesMap["m2v"] = "video/mpeg";
		mimeTypesMap["m3u"] = "audio/x-mpequrl";
		mimeTypesMap["man"] = "application/x-troff-man";
		mimeTypesMap["map"] = "application/x-navimap";
		mimeTypesMap["mar"] = "text/plain";
		mimeTypesMap["mbd"] = "application/mbedlet";
		mimeTypesMap["mc$"] = "application/x-magic-cap-package-1.0";
		mimeTypesMap["mcd"] = "application/mcad";
		mimeTypesMap["mcd"] = "application/x-mathcad";
		mimeTypesMap["mcf"] = "image/vasa";
		mimeTypesMap["mcf"] = "text/mcf";
		mimeTypesMap["mcp"] = "application/netmc";
		mimeTypesMap["me"] = "application/x-troff-me";
		mimeTypesMap["mht"] = "message/rfc822";
		mimeTypesMap["mhtml"] = "message/rfc822";
		mimeTypesMap["mid"] = "application/x-midi";
		mimeTypesMap["mid"] = "audio/midi";
		mimeTypesMap["mid"] = "audio/x-mid";
		mimeTypesMap["mid"] = "audio/x-midi";
		mimeTypesMap["mid"] = "music/crescendo";
		mimeTypesMap["mid"] = "x-music/x-midi";
		mimeTypesMap["midi"] = "application/x-midi";
		mimeTypesMap["midi"] = "audio/midi";
		mimeTypesMap["midi"] = "audio/x-mid";
		mimeTypesMap["midi"] = "audio/x-midi";
		mimeTypesMap["midi"] = "music/crescendo";
		mimeTypesMap["midi"] = "x-music/x-midi";
		mimeTypesMap["mif"] = "application/x-frame";
		mimeTypesMap["mif"] = "application/x-mif";
		mimeTypesMap["mime"] = "message/rfc822";
		mimeTypesMap["mime"] = "www/mime";
		mimeTypesMap["mjf"] = "audio/x-vnd.audioexplosion.mjuicemediafile";
		mimeTypesMap["mjpg"] = "video/x-motion-jpeg";
		mimeTypesMap["mm"] = "application/base64";
		mimeTypesMap["mm"] = "application/x-meme";
		mimeTypesMap["mme"] = "application/base64";
		mimeTypesMap["mod"] = "audio/mod";
		mimeTypesMap["mod"] = "audio/x-mod";
		mimeTypesMap["moov"] = "video/quicktime";
		mimeTypesMap["mov"] = "video/quicktime";
		mimeTypesMap["movie"] = "video/x-sgi-movie";
		mimeTypesMap["mp2"] = "audio/mpeg";
		mimeTypesMap["mp2"] = "audio/x-mpeg";
		mimeTypesMap["mp2"] = "video/mpeg";
		mimeTypesMap["mp2"] = "video/x-mpeg";
		mimeTypesMap["mp2"] = "video/x-mpeq2a";
		mimeTypesMap["mp3"] = "audio/mpeg3";
		mimeTypesMap["mp3"] = "audio/x-mpeg-3";
		mimeTypesMap["mp3"] = "video/mpeg";
		mimeTypesMap["mp3"] = "video/x-mpeg";
		mimeTypesMap["mpa"] = "audio/mpeg";
		mimeTypesMap["mpa"] = "video/mpeg";
		mimeTypesMap["mpc"] = "application/x-project";
		mimeTypesMap["mpe"] = "video/mpeg";
		mimeTypesMap["mpeg"] = "video/mpeg";
		mimeTypesMap["mpg"] = "audio/mpeg";
		mimeTypesMap["mpg"] = "video/mpeg";
		mimeTypesMap["mpga"] = "audio/mpeg";
		mimeTypesMap["mpp"] = "application/vnd.ms-project";
		mimeTypesMap["mpt"] = "application/x-project";
		mimeTypesMap["mpv"] = "application/x-project";
		mimeTypesMap["mpx"] = "application/x-project";
		mimeTypesMap["mrc"] = "application/marc";
		mimeTypesMap["ms"] = "application/x-troff-ms";
		mimeTypesMap["mv"] = "video/x-sgi-movie";
		mimeTypesMap["my"] = "audio/make";
		mimeTypesMap["mzz"] = "application/x-vnd.audioexplosion.mzz";
		mimeTypesMap["nap"] = "image/naplps";
		mimeTypesMap["naplps"] = "image/naplps";
		mimeTypesMap["nc"] = "application/x-netcdf";
		mimeTypesMap["ncm"] = "application/vnd.nokia.configuration-message";
		mimeTypesMap["nif"] = "image/x-niff";
		mimeTypesMap["niff"] = "image/x-niff";
		mimeTypesMap["nix"] = "application/x-mix-transfer";
		mimeTypesMap["nsc"] = "application/x-conference";
		mimeTypesMap["nvd"] = "application/x-navidoc";
		mimeTypesMap["o"] = "application/octet-stream";
		mimeTypesMap["oda"] = "application/oda";
		mimeTypesMap["omc"] = "application/x-omc";
		mimeTypesMap["omcd"] = "application/x-omcdatamaker";
		mimeTypesMap["omcr"] = "application/x-omcregerator";
		mimeTypesMap["p"] = "text/x-pascal";
		mimeTypesMap["p10"] = "application/pkcs10";
		mimeTypesMap["p10"] = "application/x-pkcs10";
		mimeTypesMap["p12"] = "application/pkcs-12";
		mimeTypesMap["p12"] = "application/x-pkcs12";
		mimeTypesMap["p7a"] = "application/x-pkcs7-signature";
		mimeTypesMap["p7c"] = "application/pkcs7-mime";
		mimeTypesMap["p7c"] = "application/x-pkcs7-mime";
		mimeTypesMap["p7m"] = "application/pkcs7-mime";
		mimeTypesMap["p7m"] = "application/x-pkcs7-mime";
		mimeTypesMap["p7r"] = "application/x-pkcs7-certreqresp";
		mimeTypesMap["p7s"] = "application/pkcs7-signature";
		mimeTypesMap["part"] = "application/pro_eng";
		mimeTypesMap["pas"] = "text/pascal";
		mimeTypesMap["pbm"] = "image/x-portable-bitmap";
		mimeTypesMap["pcl"] = "application/vnd.hp-pcl";
		mimeTypesMap["pcl"] = "application/x-pcl";
		mimeTypesMap["pct"] = "image/x-pict";
		mimeTypesMap["pcx"] = "image/x-pcx";
		mimeTypesMap["pdb"] = "chemical/x-pdb";
		mimeTypesMap["pdf"] = "application/pdf";
		mimeTypesMap["pfunk"] = "audio/make";
		mimeTypesMap["pfunk"] = "audio/make.my.funk";
		mimeTypesMap["pgm"] = "image/x-portable-graymap";
		mimeTypesMap["pgm"] = "image/x-portable-greymap";
		mimeTypesMap["pic"] = "image/pict";
		mimeTypesMap["pict"] = "image/pict";
		mimeTypesMap["pkg"] = "application/x-newton-compatible-pkg";
		mimeTypesMap["pko"] = "application/vnd.ms-pki.pko";
		mimeTypesMap["pl"] = "text/plain";
		mimeTypesMap["pl"] = "text/x-script.perl";
		mimeTypesMap["plx"] = "application/x-pixclscript";
		mimeTypesMap["pm"] = "image/x-xpixmap";
		mimeTypesMap["pm"] = "text/x-script.perl-module";
		mimeTypesMap["pm4"] = "application/x-pagemaker";
		mimeTypesMap["pm5"] = "application/x-pagemaker";
		mimeTypesMap["png"] = "image/png";
		mimeTypesMap["pnm"] = "application/x-portable-anymap";
		mimeTypesMap["pnm"] = "image/x-portable-anymap";
		mimeTypesMap["pot"] = "application/mspowerpoint";
		mimeTypesMap["pot"] = "application/vnd.ms-powerpoint";
		mimeTypesMap["pov"] = "model/x-pov";
		mimeTypesMap["ppa"] = "application/vnd.ms-powerpoint";
		mimeTypesMap["ppm"] = "image/x-portable-pixmap";
		mimeTypesMap["pps"] = "application/mspowerpoint";
		mimeTypesMap["pps"] = "application/vnd.ms-powerpoint";
		mimeTypesMap["ppt"] = "application/mspowerpoint";
		mimeTypesMap["ppt"] = "application/powerpoint";
		mimeTypesMap["ppt"] = "application/vnd.ms-powerpoint";
		mimeTypesMap["ppt"] = "application/x-mspowerpoint";
		mimeTypesMap["ppz"] = "application/mspowerpoint";
		mimeTypesMap["pre"] = "application/x-freelance";
		mimeTypesMap["prt"] = "application/pro_eng";
		mimeTypesMap["ps"] = "application/postscript";
		mimeTypesMap["psd"] = "application/octet-stream";
		mimeTypesMap["pvu"] = "paleovu/x-pv";
		mimeTypesMap["pwz"] = "application/vnd.ms-powerpoint";
		mimeTypesMap["py"] = "text/x-script.phyton";
		mimeTypesMap["pyc"] = "application/x-bytecode.python";
		mimeTypesMap["qcp"] = "audio/vnd.qcelp";
		mimeTypesMap["qd3"] = "x-world/x-3dmf";
		mimeTypesMap["qd3d"] = "x-world/x-3dmf";
		mimeTypesMap["qif"] = "image/x-quicktime";
		mimeTypesMap["qt"] = "video/quicktime";
		mimeTypesMap["qtc"] = "video/x-qtc";
		mimeTypesMap["qti"] = "image/x-quicktime";
		mimeTypesMap["qtif"] = "image/x-quicktime";
		mimeTypesMap["ra"] = "audio/x-pn-realaudio";
		mimeTypesMap["ra"] = "audio/x-pn-realaudio-plugin";
		mimeTypesMap["ra"] = "audio/x-realaudio";
		mimeTypesMap["ram"] = "audio/x-pn-realaudio";
		mimeTypesMap["ras"] = "application/x-cmu-raster";
		mimeTypesMap["ras"] = "image/cmu-raster";
		mimeTypesMap["ras"] = "image/x-cmu-raster";
		mimeTypesMap["rast"] = "image/cmu-raster";
		mimeTypesMap["rexx"] = "text/x-script.rexx";
		mimeTypesMap["rf"] = "image/vnd.rn-realflash";
		mimeTypesMap["rgb"] = "image/x-rgb";
		mimeTypesMap["rm"] = "application/vnd.rn-realmedia";
		mimeTypesMap["rm"] = "audio/x-pn-realaudio";
		mimeTypesMap["rmi"] = "audio/mid";
		mimeTypesMap["rmm"] = "audio/x-pn-realaudio";
		mimeTypesMap["rmp"] = "audio/x-pn-realaudio";
		mimeTypesMap["rmp"] = "audio/x-pn-realaudio-plugin";
		mimeTypesMap["rng"] = "application/ringing-tones";
		mimeTypesMap["rng"] = "application/vnd.nokia.ringing-tone";
		mimeTypesMap["rnx"] = "application/vnd.rn-realplayer";
		mimeTypesMap["roff"] = "application/x-troff";
		mimeTypesMap["rp"] = "image/vnd.rn-realpix";
		mimeTypesMap["rpm"] = "audio/x-pn-realaudio-plugin";
		mimeTypesMap["rt"] = "text/richtext";
		mimeTypesMap["rt"] = "text/vnd.rn-realtext";
		mimeTypesMap["rtf"] = "application/rtf";
		mimeTypesMap["rtf"] = "application/x-rtf";
		mimeTypesMap["rtf"] = "text/richtext";
		mimeTypesMap["rtx"] = "application/rtf";
		mimeTypesMap["rtx"] = "text/richtext";
		mimeTypesMap["rv"] = "video/vnd.rn-realvideo";
		mimeTypesMap["s"] = "text/x-asm";
		mimeTypesMap["s3m"] = "audio/s3m";
		mimeTypesMap["saveme"] = "application/octet-stream";
		mimeTypesMap["sbk"] = "application/x-tbook";
		mimeTypesMap["scm"] = "application/x-lotusscreencam";
		mimeTypesMap["scm"] = "text/x-script.guile";
		mimeTypesMap["scm"] = "text/x-script.scheme";
		mimeTypesMap["scm"] = "video/x-scm";
		mimeTypesMap["sdml"] = "text/plain";
		mimeTypesMap["sdp"] = "application/sdp";
		mimeTypesMap["sdp"] = "application/x-sdp";
		mimeTypesMap["sdr"] = "application/sounder";
		mimeTypesMap["sea"] = "application/sea";
		mimeTypesMap["sea"] = "application/x-sea";
		mimeTypesMap["set"] = "application/set";
		mimeTypesMap["sgm"] = "text/sgml";
		mimeTypesMap["sgm"] = "text/x-sgml";
		mimeTypesMap["sgml"] = "text/sgml";
		mimeTypesMap["sgml"] = "text/x-sgml";
		mimeTypesMap["sh"] = "application/x-bsh";
		mimeTypesMap["sh"] = "application/x-sh";
		mimeTypesMap["sh"] = "application/x-shar";
		mimeTypesMap["sh"] = "text/x-script.sh";
		mimeTypesMap["shar"] = "application/x-bsh";
		mimeTypesMap["shar"] = "application/x-shar";
		mimeTypesMap["shtml"] = "text/html";
		mimeTypesMap["shtml"] = "text/x-server-parsed-html";
		mimeTypesMap["sid"] = "audio/x-psid";
		mimeTypesMap["sit"] = "application/x-sit";
		mimeTypesMap["sit"] = "application/x-stuffit";
		mimeTypesMap["skd"] = "application/x-koan";
		mimeTypesMap["skm"] = "application/x-koan";
		mimeTypesMap["skp"] = "application/x-koan";
		mimeTypesMap["skt"] = "application/x-koan";
		mimeTypesMap["sl"] = "application/x-seelogo";
		mimeTypesMap["smi"] = "application/smil";
		mimeTypesMap["smil"] = "application/smil";
		mimeTypesMap["snd"] = "audio/basic";
		mimeTypesMap["snd"] = "audio/x-adpcm";
		mimeTypesMap["sol"] = "application/solids";
		mimeTypesMap["spc"] = "application/x-pkcs7-certificates";
		mimeTypesMap["spc"] = "text/x-speech";
		mimeTypesMap["spl"] = "application/futuresplash";
		mimeTypesMap["spr"] = "application/x-sprite";
		mimeTypesMap["sprite"] = "application/x-sprite";
		mimeTypesMap["src"] = "application/x-wais-source";
		mimeTypesMap["ssi"] = "text/x-server-parsed-html";
		mimeTypesMap["ssm"] = "application/streamingmedia";
		mimeTypesMap["sst"] = "application/vnd.ms-pki.certstore";
		mimeTypesMap["step"] = "application/step";
		mimeTypesMap["stl"] = "application/sla";
		mimeTypesMap["stl"] = "application/vnd.ms-pki.stl";
		mimeTypesMap["stl"] = "application/x-navistyle";
		mimeTypesMap["stp"] = "application/step";
		mimeTypesMap["sv4cpio"] = "application/x-sv4cpio";
		mimeTypesMap["sv4crc"] = "application/x-sv4crc";
		mimeTypesMap["svf"] = "image/vnd.dwg";
		mimeTypesMap["svf"] = "image/x-dwg";
		mimeTypesMap["svr"] = "application/x-world";
		mimeTypesMap["svr"] = "x-world/x-svr";
		mimeTypesMap["swf"] = "application/x-shockwave-flash";
		mimeTypesMap["t"] = "application/x-troff";
		mimeTypesMap["talk"] = "text/x-speech";
		mimeTypesMap["tar"] = "application/x-tar";
		mimeTypesMap["tbk"] = "application/toolbook";
		mimeTypesMap["tbk"] = "application/x-tbook";
		mimeTypesMap["tcl"] = "application/x-tcl";
		mimeTypesMap["tcl"] = "text/x-script.tcl";
		mimeTypesMap["tcsh"] = "text/x-script.tcsh";
		mimeTypesMap["tex"] = "application/x-tex";
		mimeTypesMap["texi"] = "application/x-texinfo";
		mimeTypesMap["texinfo"] = "application/x-texinfo";
		mimeTypesMap["text"] = "application/plain";
		mimeTypesMap["text"] = "text/plain";
		mimeTypesMap["tgz"] = "application/gnutar";
		mimeTypesMap["tgz"] = "application/x-compressed";
		mimeTypesMap["tif"] = "image/tiff";
		mimeTypesMap["tif"] = "image/x-tiff";
		mimeTypesMap["tiff"] = "image/tiff";
		mimeTypesMap["tiff"] = "image/x-tiff";
		mimeTypesMap["tr"] = "application/x-troff";
		mimeTypesMap["tsi"] = "audio/tsp-audio";
		mimeTypesMap["tsp"] = "application/dsptype";
		mimeTypesMap["tsp"] = "audio/tsplayer";
		mimeTypesMap["tsv"] = "text/tab-separated-values";
		mimeTypesMap["turbot"] = "image/florian";
		mimeTypesMap["txt"] = "text/plain";
		mimeTypesMap["uil"] = "text/x-uil";
		mimeTypesMap["uni"] = "text/uri-list";
		mimeTypesMap["unis"] = "text/uri-list";
		mimeTypesMap["unv"] = "application/i-deas";
		mimeTypesMap["uri"] = "text/uri-list";
		mimeTypesMap["uris"] = "text/uri-list";
		mimeTypesMap["ustar"] = "application/x-ustar";
		mimeTypesMap["ustar"] = "multipart/x-ustar";
		mimeTypesMap["uu"] = "application/octet-stream";
		mimeTypesMap["uu"] = "text/x-uuencode";
		mimeTypesMap["uue"] = "text/x-uuencode";
		mimeTypesMap["vcd"] = "application/x-cdlink";
		mimeTypesMap["vcs"] = "text/x-vcalendar";
		mimeTypesMap["vda"] = "application/vda";
		mimeTypesMap["vdo"] = "video/vdo";
		mimeTypesMap["vew"] = "application/groupwise";
		mimeTypesMap["viv"] = "video/vivo";
		mimeTypesMap["viv"] = "video/vnd.vivo";
		mimeTypesMap["vivo"] = "video/vivo";
		mimeTypesMap["vivo"] = "video/vnd.vivo";
		mimeTypesMap["vmd"] = "application/vocaltec-media-desc";
		mimeTypesMap["vmf"] = "application/vocaltec-media-file";
		mimeTypesMap["voc"] = "audio/voc";
		mimeTypesMap["voc"] = "audio/x-voc";
		mimeTypesMap["vos"] = "video/vosaic";
		mimeTypesMap["vox"] = "audio/voxware";
		mimeTypesMap["vqe"] = "audio/x-twinvq-plugin";
		mimeTypesMap["vqf"] = "audio/x-twinvq";
		mimeTypesMap["vql"] = "audio/x-twinvq-plugin";
		mimeTypesMap["vrml"] = "application/x-vrml";
		mimeTypesMap["vrml"] = "model/vrml";
		mimeTypesMap["vrml"] = "x-world/x-vrml";
		mimeTypesMap["vrt"] = "x-world/x-vrt";
		mimeTypesMap["vsd"] = "application/x-visio";
		mimeTypesMap["vst"] = "application/x-visio";
		mimeTypesMap["vsw"] = "application/x-visio";
		mimeTypesMap["w60"] = "application/wordperfect6.0";
		mimeTypesMap["w61"] = "application/wordperfect6.1";
		mimeTypesMap["w6w"] = "application/msword";
		mimeTypesMap["wav"] = "audio/wav";
		mimeTypesMap["wav"] = "audio/x-wav";
		mimeTypesMap["wb1"] = "application/x-qpro";
		mimeTypesMap["wbmp"] = "image/vnd.wap.wbmp";
		mimeTypesMap["web"] = "application/vnd.xara";
		mimeTypesMap["wiz"] = "application/msword";
		mimeTypesMap["wk1"] = "application/x-123";
		mimeTypesMap["wmf"] = "windows/metafile";
		mimeTypesMap["wml"] = "text/vnd.wap.wml";
		mimeTypesMap["wmlc"] = "application/vnd.wap.wmlc";
		mimeTypesMap["wmls"] = "text/vnd.wap.wmlscript";
		mimeTypesMap["wmlsc"] = "application/vnd.wap.wmlscriptc";
		mimeTypesMap["word"] = "application/msword";
		mimeTypesMap["wp"] = "application/wordperfect";
		mimeTypesMap["wp5"] = "application/wordperfect";
		mimeTypesMap["wp5"] = "application/wordperfect6.0";
		mimeTypesMap["wp6"] = "application/wordperfect";
		mimeTypesMap["wpd"] = "application/wordperfect";
		mimeTypesMap["wpd"] = "application/x-wpwin";
		mimeTypesMap["wq1"] = "application/x-lotus";
		mimeTypesMap["wri"] = "application/mswrite";
		mimeTypesMap["wri"] = "application/x-wri";
		mimeTypesMap["wrl"] = "application/x-world";
		mimeTypesMap["wrl"] = "model/vrml";
		mimeTypesMap["wrl"] = "x-world/x-vrml";
		mimeTypesMap["wrz"] = "model/vrml";
		mimeTypesMap["wrz"] = "x-world/x-vrml";
		mimeTypesMap["wsc"] = "text/scriplet";
		mimeTypesMap["wsrc"] = "application/x-wais-source";
		mimeTypesMap["wtk"] = "application/x-wintalk";
		mimeTypesMap["xbm"] = "image/x-xbitmap";
		mimeTypesMap["xbm"] = "image/x-xbm";
		mimeTypesMap["xbm"] = "image/xbm";
		mimeTypesMap["xdr"] = "video/x-amt-demorun";
		mimeTypesMap["xgz"] = "xgl/drawing";
		mimeTypesMap["xif"] = "image/vnd.xiff";
		mimeTypesMap["xl"] = "application/excel";
		mimeTypesMap["xla"] = "application/excel";
		mimeTypesMap["xla"] = "application/x-excel";
		mimeTypesMap["xla"] = "application/x-msexcel";
		mimeTypesMap["xlb"] = "application/excel";
		mimeTypesMap["xlb"] = "application/vnd.ms-excel";
		mimeTypesMap["xlb"] = "application/x-excel";
		mimeTypesMap["xlc"] = "application/excel";
		mimeTypesMap["xlc"] = "application/vnd.ms-excel";
		mimeTypesMap["xlc"] = "application/x-excel";
		mimeTypesMap["xld"] = "application/excel";
		mimeTypesMap["xld"] = "application/x-excel";
		mimeTypesMap["xlk"] = "application/excel";
		mimeTypesMap["xlk"] = "application/x-excel";
		mimeTypesMap["xll"] = "application/excel";
		mimeTypesMap["xll"] = "application/vnd.ms-excel";
		mimeTypesMap["xll"] = "application/x-excel";
		mimeTypesMap["xlm"] = "application/excel";
		mimeTypesMap["xlm"] = "application/vnd.ms-excel";
		mimeTypesMap["xlm"] = "application/x-excel";
		mimeTypesMap["xls"] = "application/excel";
		mimeTypesMap["xls"] = "application/vnd.ms-excel";
		mimeTypesMap["xls"] = "application/x-excel";
		mimeTypesMap["xls"] = "application/x-msexcel";
		mimeTypesMap["xlt"] = "application/excel";
		mimeTypesMap["xlt"] = "application/x-excel";
		mimeTypesMap["xlv"] = "application/excel";
		mimeTypesMap["xlv"] = "application/x-excel";
		mimeTypesMap["xlw"] = "application/excel";
		mimeTypesMap["xlw"] = "application/vnd.ms-excel";
		mimeTypesMap["xlw"] = "application/x-excel";
		mimeTypesMap["xlw"] = "application/x-msexcel";
		mimeTypesMap["xm"] = "audio/xm";
		mimeTypesMap["xml"] = "application/xml";
		mimeTypesMap["xml"] = "text/xml";
		mimeTypesMap["xmz"] = "xgl/movie";
		mimeTypesMap["xpix"] = "application/x-vnd.ls-xpix";
		mimeTypesMap["xpm"] = "image/x-xpixmap";
		mimeTypesMap["xpm"] = "image/xpm";
		mimeTypesMap["x-png"] = "image/png";
		mimeTypesMap["xsr"] = "video/x-amt-showrun";
		mimeTypesMap["xwd"] = "image/x-xwd";
		mimeTypesMap["xwd"] = "image/x-xwindowdump";
		mimeTypesMap["xyz"] = "chemical/x-pdb";
		mimeTypesMap["z"] = "application/x-compress";
		mimeTypesMap["z"] = "application/x-compressed";
		mimeTypesMap["zip"] = "application/x-compressed";
		mimeTypesMap["zip"] = "application/x-zip-compressed";
		mimeTypesMap["zip"] = "application/zip";
		mimeTypesMap["zip"] = "multipart/x-zip";
		mimeTypesMap["zoo"] = "application/octet-stream";
		mimeTypesMap["zsh"] = "text/x-script.zsh";
	}
	std::string fnameLower = fileName;
	std::transform( fnameLower.begin(), fnameLower.end(), fnameLower.begin(), ::tolower );
	std::string fext;
	int fextI = fnameLower.length();
	for ( int i = fnameLower.length() - 1; i > 0 && fnameLower[i] != '.' && fnameLower[i] != '/'; i-- )
		fextI = i;
	fext = fnameLower.substr( fextI );

	if ( mimeTypesMap.count( fext ) > 0 ) return mimeTypesMap[fext];
	return mimeTypesMap["."];
}


}
}
