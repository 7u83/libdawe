/*
 * Copyright (C) 2021  7u83 <7u83@mail.ru>
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
#ifndef DAWEXCEPTION_H
#define DAWEXCEPTION_H

#include <stdarg.h>
#include <iostream>

class DaweException : public std::exception
{
public:
	DaweException(int code,const char *str);
	DaweException(const char * format, ...);

	int code;
	const char *str;
	virtual const char* what() const throw();
	char buffer[256];
};

#define DAWE_EXCEPTION(format,...)\
	throw DaweException(format,##__VA_ARGS__)


#endif // DAWEXCEPTION_H

