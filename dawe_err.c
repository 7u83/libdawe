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

#include <stdlib.h>
#include <string.h>

#include "dawe_err.h"

static struct {
	int code;
	const char * msg;
} msgs[] = {
	{DAWE_ENOWAV, "Not a WAV file"},
	{DAWE_EPREMATEOF,"Premature EOF"},
	{0,NULL}
};

const char * dawe_strerror(int err)
{
	int i;
	if(err>=0)
		return strerror(err);
	for(i=0; msgs[i].code; i++){
		if (msgs[i].code==err)
			return msgs[i].msg;
	}
	return "Unknown error";
}
