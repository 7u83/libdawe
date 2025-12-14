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
#ifndef REGEX_H
#define REGEX_H

#include <string>
#include <regex.h>

/**
 * @file Class definition for RegEx class
 */


/**
 * @addtogroup TOOLS
 * @{
 */

namespace Dawe {

/**
 * @brief The RegEx class
 *
 *
 */
class RegEx
{
public:
	/**
	 * @brief RegEx constructor.
	 *
	 *
	 * @param r Regular exporession to use.
	 */
	RegEx(const char *r);

	/**
	 * @brief exec
	 * @param str
	 * @return
	 */
	bool exec(const std::string & str);

	/**
	 * @brief getMatch
	 * @param str
	 * @param n
	 * @return
	 */
//	std::string getMatch(const std::string & str, int n);
	std::string captured(int n);

	/**
	 * @brief Destructor
	 */
	~RegEx();

private:
	regex_t regex;
	regmatch_t * matches;
	int num_matches;
	std::string exec_str;
};

/**
 * @}
 */

} // namespace Dawe

#endif // REGEX_H
