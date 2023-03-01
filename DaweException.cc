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

#include <stdio.h>
#include <stdarg.h>

#include "DaweException.h"

DaweException::DaweException(int code,const char *str)
{
	this->code=code;
	this->str=str;
}

const char* DaweException::what() const throw()
{
	return this->buffer;
}

DaweException::DaweException(const char * format, ...)
{
	va_list args;
	va_start (args, format);
	vsnprintf (buffer,256,format, args);
		va_end (args);
}
