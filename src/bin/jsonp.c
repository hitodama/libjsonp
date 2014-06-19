#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <jansson.h>

#include "jansson_extension.h"
#include "libjsonpp.h"
#include "libjsonp.h"

#include "libjsonp_helper.h"
#include "libjsonp_config.h"

#include "jsonp_options.h"

static const char *jsonp_main_options = 
	"{\
		\"arguments\":[\"verb\", \"path\", \"value\"],\
		\"options\":[\"i\", \"o\", \"t\"],\
		\"switches\":[\"pp\", \"h\"],\
		\"help\":{\
			\"verb\": \" One of <create, get, set, delete>\",\
			\"path\": \"The jsonp path\",\
			\"[value]\": \"The value to be set or created\",\
			\"[i]\": \"User input file instead of stdin\",\
			\"[o]\": \"User output file instead of stdout\",\
			\"[t]\": \"Force the type of the value\",\
			\"[pp]\": \"Switch to percent pointers\",\
			\"[h]\": \"Display this help and exit\"\
		}\
	}";

json_t *jsonp_prepare_options(int argc, char **argv)
{
	size_t i;
	json_t *r;
	json_t *args = json_array();

	for(i = 1; i < argc; ++i)
		json_set_new(args, NULL, json_string(argv[i]));

	json_t *opt = json_loads(jsonp_main_options, 0, NULL);

	r = jsonp_options(args, opt, '-');

	json_decref(args);
	json_decref(opt);

	return r;
}

int main(int argc, char **argv)
{
	json_t *json;
	json_error_t error;

	FILE *inFile = stdin;
	FILE *outFile = stdout;

	const char *verb;
	const char *path;

	json_t *args = jsonp_prepare_options(argc, argv);

	json_t *verb_ = jsonpp_get(args, "/arguments/verb");
	json_t *path_ = jsonpp_get(args, "/arguments/path");
	json_t *value = jsonpp_get(args, "/arguments/value");

	json_t *in = jsonpp_get(args, "/options/i");
	json_t *out = jsonpp_get(args, "/options/o");
	json_t *t = jsonpp_get(args, "/options/t");
	json_t *pp = jsonpp_get(args, "/switches/pp");
	json_t *h = jsonpp_get(args, "/switches/h");

	if(h != NULL)
	{
		json_t *opt = json_loads(jsonp_main_options, 0, NULL);
		json_dumpf(jsonpp_get(opt, "/help"), stdout, 
			JSON_PRESERVE_ORDER |JSON_ENCODE_ANY | JSON_INDENT(4));
		json_decref(opt);
		return EXIT_SUCCESS;
	}

	if(verb_ == NULL || !json_is_string(verb_))
	{
		fprintf(stderr, "Verb undefined or wrong!\n");
		return EXIT_FAILURE;
	}
	if(path_ == NULL || !json_is_string(path_))
	{
		fprintf(stderr, "Path undefined or wrong!\n");
		return EXIT_FAILURE;
	}

	if(in != NULL && !json_is_string(in))
	{
		fprintf(stderr, "In defined wrong!\n");
		return EXIT_FAILURE;	
	}
	if(out != NULL && !json_is_string(out))
	{
		fprintf(stderr, "Out defined wrong!\n");
		return EXIT_FAILURE;
	}

	if(in)
	{
		inFile = fopen(json_string_value(in), "rb");
		if(inFile == NULL)
		{
			fprintf(stderr, "Input file could not be opened!\n");
			return EXIT_FAILURE;
		}
	}

	if(out)
	{
		outFile = fopen(json_string_value(out), "wb");
		if(outFile == NULL)
		{
			fprintf(stderr, "Output file could not be opened!\n");
			if(inFile != stdin)
				fclose(inFile);
			return EXIT_FAILURE;
		}
	}

	verb = json_string_value(verb_);
	path = json_string_value(path_);

	json = json_loadf(inFile, JSON_DECODE_ANY, &error);
	if(!json)
	{
		 fprintf(stderr, "Could not read from input!\n");
		 fprintf(stderr, "%s\n", error.text);
		 return EXIT_FAILURE;
	}

	if(t != NULL && json_is_string(t))
	{
		json_t *o = json_oftype(json_type_from_string(json_string_value(t)));
		if(value != NULL)
		{
			char *c = json_value_copy(value);
			json_value_set(o, c);
			free((void *)c);
		}
		value = o;
	}

	if(pp)
	{
		if(strcmp(verb, "create") == 0)
			jsonpp_create(json, path, value);
		else if(strcmp(verb, "get") == 0)
			json = jsonpp_get(json, path);
		else if(strcmp(verb, "set") == 0 && value != NULL)
			jsonpp_set(json, path, value);
		else if(strcmp(verb, "delete") == 0)
			jsonpp_delete(json, path);
		else
		{
			fprintf(stderr, "Verb not accepted or value missing!\n");
			return EXIT_FAILURE;
		}
	}
	else
	{
		if(strcmp(verb, "create") == 0)
			jsonp_create(json, path, value);
		else if(strcmp(verb, "get") == 0)
			json = jsonp_get(json, path);
		else if(strcmp(verb, "set") == 0 && value != NULL)
			jsonp_set(json, path, value);
		else if(strcmp(verb, "delete") == 0)
			jsonp_delete(json, path);
		else
		{
			fprintf(stderr, "Verb not accepted or value missing!\n");
			return EXIT_FAILURE;
		}
	}
	
	if(json != NULL)
	{
		if(json_dumpf(json, outFile, JSON_ENCODE_ANY) < 0)
		{
			fprintf(stderr, "Could not write to output!\n");
			return EXIT_FAILURE;
		}
	}

	if(inFile != stdin)
		fclose(inFile);

	if(outFile != stdout)
		fclose(outFile);

	json_decref(args);

	return EXIT_SUCCESS;
}