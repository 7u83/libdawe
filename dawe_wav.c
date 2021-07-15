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
#include <error.h>
#include <string.h>

#include "sysdefs.h"
#include "dawe_wav.h"


struct riff_tag;


static struct riff_tag * find(uint8_t *tag, struct riff_tag * list);
static int read_chunks(FILE *infile, struct riff_tag *tag, uint32_t clen,
					   int depth,struct riff_tag * taglist);

void read_type (FILE * infile,struct riff_tag * tag,int clen, int depth,
						   struct riff_tag *typelist);

void riff_print (FILE * infile,struct riff_tag * tag,int clen, int depth,
						   struct riff_tag *typelist);
void riff_fmt (FILE * infile,struct riff_tag * tag,int clen, int depth,
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
	void (*action)(FILE * infile,struct riff_tag *t,int clen, int depth,
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

void riff_print (FILE * infile,struct riff_tag * tag,int clen, int depth,
						   struct riff_tag *typelist)
{
	char data[256];
	fread(data,clen,1,infile);
	printf("FOUND: %s\n",data);
}


void riff_fmt (FILE * infile,struct riff_tag * tag,int clen, int depth,
						   struct riff_tag *typelist)
{
	uint16_t encoding,channels,block_align,bits_per_sample;
	uint32_t sample_rate, bytes_per_second;
	get_uint16(infile,&encoding);
	printf("Encoding: %04x\n",encoding);
	get_uint16(infile,&channels);
	printf("Channels: %d\n",channels);
	get_uint32(infile,&sample_rate);
	printf("Sample Rate: %d\n",sample_rate);
	get_uint32(infile,&bytes_per_second);
	printf("bytes_per_second: %d\n",bytes_per_second);
	get_uint16(infile,&block_align);
	printf("block_align: %d\n",block_align);
	get_uint16(infile,&bits_per_sample);
	printf("bits_per_sample: %d\n",bits_per_sample);

}

void read_type (FILE * infile,struct riff_tag * tag,int clen, int depth,
						   struct riff_tag *typelist)
{
	uint8_t type_id[4];
	struct riff_tag *r;

	/* read type id */
	if ( fread(type_id,4,1,infile) != 1)
		return;

	r = find(type_id,typelist);
	if (r){
		printf("Found type: %s %s\n",r->id,r->desc);
		/*xget_chunk(infile,clen-4,depth+1,riff_wave);*/
		r->action(infile,tag,clen-4,depth+1,r->list);
	}

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


static int read_chunks(FILE *infile, struct riff_tag *tag, uint32_t clen,
					   int depth,struct riff_tag * taglist)
{
	uint8_t chunk_id[4];
	uint32_t len;

	struct riff_tag * r;

	while (!feof(infile)) {
		
		/* read chunk id */
		if ( fread(chunk_id,4,1,infile) != 1)
			return 1;

		/* read chunk length */
		if (!get_uint32(infile,&len)){
			return 1;
		}

		printf("CHUNK: %c%c%c%c - (LEN: %d, CLEN: %d, DEPTH:%d)\n",
			   chunk_id[0],
				chunk_id[1],
				chunk_id[2],
				chunk_id[3],
				len,clen,depth);

		r = find (chunk_id,taglist);
		if (r){
			printf("Found: %s %s\n",r->id,r->desc);
			if (r->action){
				r->action(infile,r,len,depth,r->list);
			}
		}
		else{
			fseek(infile,len,SEEK_CUR);
		}


		if (depth>0){
			clen -= (len+8);
			if (clen<=0)
				return 0;
		}
	}

	return 0;
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

	w = malloc(sizeof(dawe_wav_t));
	if(!w)
		return NULL;
	memset(w,0,sizeof(dawe_wav_t));

	w->file=fopen(filename,"r");
	if (!w->file){
		dawe_wav_close(w);
		return NULL;
	}
	read_chunks(w->file,NULL,0,0,riff_taglist);
	return w;
}
