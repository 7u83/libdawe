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


#include "dawe.h"

#include <iostream>
#include <stdio.h>
#include <string.h>

#include <jack/jack.h>
#include <jack/control.h>

#include "JackSess.h"
#include "Exception.h"

namespace Dawe {

JackSess::JackSess(const char*name) : Session(name)
{
	return;

	const char *client_name = "LibDawe";
	const char *server_name = 0;

	jack_options_t options = JackNullOption; // JackNoStartServer;
	jack_status_t status;

	client = jack_client_open (client_name, options, &status, server_name);
	if (client==0) {
		throw Exception("jac_client_open() failed: 0x%2.0x", status);
	}
	this->sampling_rate = jack_get_sample_rate (client);
	this->format=FLOAT;

//	jack_port_t *input_port;

	input_port = jack_port_register (client, "input",
					 JACK_DEFAULT_AUDIO_TYPE,
					 JackPortIsInput, 0);

	int rc = jack_activate (client);
	if (rc){
		throw Exception("JACK: cannot activate client %d",rc);
	}
}

class Param{
private:
	union jackctl_parameter_value  pval;
	jackctl_parameter_t * pparam;
	char buf[64];
public:


	Param(jackctl_parameter_t *p){
		pparam=p;
		jackctl_param_type_t  t = jackctl_parameter_get_type (p);
		pval = jackctl_parameter_get_value(p);
		name = jackctl_parameter_get_name(p);

		switch (t)
		{
		case JackParamInt:
			type="INT";
			sprintf(buf,"%d",pval.i);
			val=buf;
			break;
		case JackParamUInt:
			type="UINT";
			sprintf(buf,"%u",pval.ui);
			val=buf;
			break;
		case JackParamChar:
			type="CHAR";
			sprintf(buf,"%c",pval.c);
			val=buf;
			break;
		case JackParamString:
			type="STRING";
			val=pval.str;
			break;
		case JackParamBool:
			type="BOOL";
			sprintf(buf,"%s",pval.b ? "True":"False");
			val=buf;
			break;
		default:
			type="Unknown";
			val = "";
		}

	};
	const char *type;
	const char * val;
	const char * name;

	void print(){
		std::cout << name << "(" << type << ")" << ":" << val << std::endl;
	}

	void printv(){
		print();
		std::cout << "\t"
			  << jackctl_parameter_get_short_description(pparam)
			  << std::endl;
		std::cout << "\t"
			  << jackctl_parameter_get_long_description(pparam)
			  << std::endl;
	}

};



/*

static void print_parameter(jackctl_parameter_t *p)
{
	std::cout << "===============================" << std::endl;
	std::cout << "Name: " << jackctl_parameter_get_name(p) << std::endl;
	const char *sdesc = jackctl_parameter_get_short_description(p);
	const char *ldesc = jackctl_parameter_get_long_description(p);
	if (sdesc){
		std::cout << "DESC: " << sdesc << std::endl;
		if (ldesc){
			if (strcmp(sdesc,ldesc))
				std::cout << "COMMENT: " << ldesc << std::endl;
		}
	}

	jackctl_param_type_t  t = jackctl_parameter_get_type (p);

	std::cout << "TYPE: " << param_type_and_val(t) << std::endl;

}

*/

void JackSess::run_server()
{
	server = jackctl_server_create2(0,0,0);
	if (server == 0 )
		throw Exception("jackctl_server_create failed");

	const JSList * paramlist = jackctl_server_get_parameters(server);

	std::cout << "=== Server Params ===" << std::endl;

	while (paramlist){
		jackctl_parameter_t * p = (jackctl_parameter_t*)paramlist->data;
		Param param = Param(p);
		param.printv();

		/*
		std::cout << "Param:" << jackctl_parameter_get_name(d) << std::endl;
		std::cout << "  SDesc:" << jackctl_parameter_get_short_description(d) << std::endl;
		std::cout << "  LDesc:" << jackctl_parameter_get_long_description(d) << std::endl;
		//std::cout << "  LDesc:" << jackctl_parameter_get_value(d) << std::endl;
*/
		paramlist = paramlist->next;
	};

	const JSList * drvlist = jackctl_server_get_drivers_list(server);
	while (drvlist){
		jackctl_driver_t * d = (jackctl_driver_t*)drvlist->data;
		std::cout << "Driver:" << jackctl_driver_get_name(d) << std::endl;
		const JSList * paramlist = jackctl_driver_get_parameters(d);
		while (paramlist){
			jackctl_parameter_t * p = (jackctl_parameter_t*)paramlist->data;
			Param param = Param(p);
			param.printv();
			paramlist = paramlist->next;
		};

		drvlist = drvlist->next;
	};
	//jack_get_server_list();

}

JackSess::~JackSess()
{
	if (this->server )
		jackctl_server_destroy(this->server );
}

} //namespace
