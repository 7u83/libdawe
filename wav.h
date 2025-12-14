#ifndef _WAV_H
#define _WAV_H

#include "def.h"


#define MK_ID(a,b,c,d)	((a) | ((b)<<8) | ((c)<<16) | ((d)<<24))

#define WAV_RIFF_ID		MK_ID('R','I','F','F')
#define WAV_TYPE_WAVE		MK_ID('W','A','V','E')
#define WAV_LIST_ID		MK_ID('L','I','S','T')
#define WAV_TYPE_INFO		MK_ID('I','N','F','O')

#define WAV_FMT_MAGIC   "fmt "

#define WAV_DATA_MAGIC  "data"



#endif
