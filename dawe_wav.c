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

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "sysdefs.h"
#include "dawe_wav.h"
#include "dawe_err.h"


struct riff_tag;


static struct riff_tag * find(uint8_t *tag, struct riff_tag * list);
static int read_chunks(dawe_wav_t * w, struct riff_tag *tag, uint32_t clen,
					   int depth,struct riff_tag * taglist);

static int read_type (dawe_wav_t * w,struct riff_tag * tag,uint32_t clen, int depth,
						   struct riff_tag *typelist);

static int riff_print (dawe_wav_t * w,struct riff_tag * tag,uint32_t clen, int depth,
						   struct riff_tag *typelist);
static int riff_fmt (dawe_wav_t * w,struct riff_tag * tag,uint32_t clen, int depth,
						   struct riff_tag *typelist);

static int get_uint32(FILE * infile, uint32_t *val){
	uint8_t buf[4];
	int n;
	n = fread(buf,4,1,infile);
	if (n!=1) {
		return 0;
	}
	*val = buf[0]+(buf[1]<<8)+(buf[2]<<16)+(buf[3]<<24);
	return 1;
}

static int get_uint16(FILE * infile, uint16_t *val){
	uint8_t buf[2];
	int n;
	n = fread(buf,2,1,infile);
	if (n!=1) {
		return 0;
	}
	*val = buf[0]+(buf[1]<<8);
	return 1;
}

struct riff_tag {
	const char *id;
	const char *desc;
	int (*action)(dawe_wav_t * w,struct riff_tag *t,uint32_t clen, int depth,
				   struct riff_tag * list
				   );
	struct riff_tag * list;
};

struct riff_tag riff_types[];
struct riff_tag riff_wave_list[];
struct riff_tag riff_wave[];


struct riff_tag riff_types[]={
	{"WAVE","WAV File - Audio Data",read_chunks,riff_wave},
	{NULL,NULL,NULL,NULL}
};

static struct riff_tag riff_taglist[]={
	{"RIFF","RIFF File",read_type,riff_types},
	{NULL,NULL,NULL,NULL}
};

struct riff_tag riff_wave[]={
	{"LIST","Container",read_type,riff_wave_list},
	{"fmt ","Format",riff_fmt,NULL},
	{NULL,NULL,NULL,NULL}
};

static struct riff_tag riff_wave_list_info[]={
	{"AGES","Rated",riff_print,NULL},
	{"INAM","Title",riff_print,NULL},
	{"IPRD","Product",riff_print,NULL},
	{"IART","Artist",riff_print,NULL},
	{"IGNR","Genre",riff_print,NULL},
	{"ISFT","Software",riff_print,NULL},
	{NULL,NULL,NULL,NULL}
};

struct riff_tag riff_wave_list[]={
	{"INFO","Container",read_chunks,riff_wave_list_info},
	{NULL,NULL,NULL,NULL}
};

static int riff_print (dawe_wav_t * w,struct riff_tag * tag, uint32_t clen,
				int depth,		   struct riff_tag *typelist)
{
	char data[256];
	fread(data,clen,1,w->file);
	printf("FOUND: %s\n",data);
	return 0;
}


static int riff_fmt (dawe_wav_t * w,struct riff_tag * tag,uint32_t clen, int depth,
						   struct riff_tag *typelist)
{
	uint16_t encoding,channels,block_align,bits_per_sample;
	uint32_t sample_rate, bytes_per_second;
	get_uint16(w->file,&encoding);
	printf("Encoding: %04x\n",encoding);
	get_uint16(w->file,&channels);
	printf("Channels: %d\n",channels);
	get_uint32(w->file,&sample_rate);
	printf("Sample Rate: %d\n",sample_rate);
	get_uint32(w->file,&bytes_per_second);
	printf("bytes_per_second: %d\n",bytes_per_second);
	get_uint16(w->file,&block_align);
	printf("block_align: %d\n",block_align);
	get_uint16(w->file,&bits_per_sample);
	printf("bits_per_sample: %d\n",bits_per_sample);
	return 0;
}

int read_type (dawe_wav_t * w,struct riff_tag * tag,uint32_t clen, int depth,
						   struct riff_tag *typelist)
{
	uint8_t type_id[4];
	struct riff_tag *r;

	/* read type id */
	if ( fread(type_id,4,1,w->file) != 1){
		if (errno)
			return errno;
		return DAWE_EPREMATEOF;
	}

	r = find(type_id,typelist);
	if (r){
		printf("Found type: %s %s\n",r->id,r->desc);
		return r->action(w,tag,clen-4,depth+1,r->list);
	}
/* TODO */
	return 0;
}


static struct riff_tag * find(uint8_t *tag, struct riff_tag * list)
{
	while( (list->id != NULL)){
		if (memcmp(list->id,tag,4)==0)
			return list;
		list++;
	}
	return NULL;
}

static int read_chunk_header(dawe_wav_t * w, uint8_t * chunk_id, uint32_t * len)
{
	/* read chunk id */
	if ( fread(chunk_id,4,1,w->file) != 1){
		if (errno){
			return errno;
		}
		return DAWE_EPREMATEOF;
	}

	/* read chunk length */
	if (!get_uint32(w->file,len)){
		if (errno){
			return errno;
		}
		return DAWE_EPREMATEOF;
	}
	return 0;
}

static int read_chunks(dawe_wav_t *w, struct riff_tag *tag, uint32_t clen,
					   int depth,struct riff_tag * taglist)
{
	uint8_t chunk_id[4];
	uint32_t len;
	int rc;

	struct riff_tag * r;

	while (!feof(w->file)) {
		rc = read_chunk_header(w,chunk_id,&len);
		if (rc)
			return rc;

		printf("CHUNK: %c%c%c%c - (LEN: %d, CLEN: %d, DEPTH:%d)\n",
			   chunk_id[0],
				chunk_id[1],
				chunk_id[2],
				chunk_id[3],
				len,clen,depth);

		r = find (chunk_id,taglist);
		if (!r){
			if (depth==0)
				return DAWE_ENOWAV;
			if (fseek(w->file,len,SEEK_CUR)==-1)
				return errno;
		}
		else{
			printf("Found: %s %s\n",r->id,r->desc);
			if (r->action){
				rc = r->action(w,r,len,depth,r->list);
				if (rc)
					return rc;
			}
		}

		if (depth>0){
			clen -= (len+8);
			if (clen<=0)
				return 0;
		}



	}
	return 0;
}

static int start(dawe_wav_t *w){
	uint8_t id[4];
	uint32_t len;
	struct riff_tag * r;

	int rc;
	rc = read_chunk_header(w,id,&len);
	if (rc)
		return rc;
	r = find (id,riff_taglist);
	if (!r){
		return DAWE_ENOWAV;
	}

	/* read type id */
	if ( fread(id,4,1,w->file) != 1){
		if (errno)
			return errno;
		return DAWE_ENOWAV;
	}
	r = find (id,riff_types);
	if (!r){
		return DAWE_ENOWAV;
	}
	return read_chunks(w,r,len-4,1,r->list);
}

/**
 * @brief dawe_wav_close
 * @param w
 */
void dawe_wav_close(dawe_wav_t *w)
{
	if (w->file)
		fclose(w->file);
	free(w);
}

/**
 * @brief dawe_wav_open
 * @param filename
 * @return
 */
dawe_wav_t * dawe_wav_open(const char * filename)
{
	dawe_wav_t * w;
	int rc;

	w = malloc(sizeof(dawe_wav_t));
	if(!w)
		return NULL;
	memset(w,0,sizeof(dawe_wav_t));

	w->file=fopen(filename,"r");
	if (!w->file){
		dawe_wav_close(w);
		return NULL;
	}
/*	rc = read_chunks(w,NULL,0,0,riff_taglist);*/
	rc = start(w);
	if (rc){
		dawe_wav_close(w);
		errno=rc;
		return NULL;
	}
	return w;
}
