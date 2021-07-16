/*
  LIBDAWE
  Copyright (C) 2021  7u83@mail.ru

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses.
*/
#ifndef _DAWE_ERR_H
#define _DAWE_ERR_H

#define DAWE_ENOWAV		-100	/* no wav file */
#define DAWE_EPREMATEOF	-101	/* premature EOF */

extern const char * dawe_strerror(int err);

#endif
