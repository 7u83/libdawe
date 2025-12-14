/*
 * Copyright (C) 2023 7u83 <7u83@mail.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses.
 */
#include "RegEx.h"
#include "../Exception.h"

namespace Dawe {

RegEx::RegEx(const char *r):matches(0){
	if (regcomp(&regex, r, REG_EXTENDED)!=0){
		throw Exception("Can't compile regular expression: %s",r);
	}
	num_matches = regex.re_nsub + 1;
	matches = new regmatch_t[num_matches];
}

bool RegEx::exec(const std::string & str){
	exec_str = str;
	int rc = regexec(&regex, str.c_str(), num_matches, matches,0 );
	if ( rc == REG_NOMATCH) {
		return false;
	}
	return true;
}

/*
std::string RegEx::getMatch(const std::string &str, int n){
	std::string s(str.c_str() + matches[n].rm_so, matches[n].rm_eo - matches[n].rm_so);
	return s;
}
*/

std::string RegEx::captured(int n)
{
	std::string s(exec_str.c_str() + matches[n].rm_so, matches[n].rm_eo - matches[n].rm_so);
	return s;
}

RegEx::~RegEx(){
	if (matches)
		delete[] matches;
	regfree(&regex);
	std::string exec_str;
}



}
