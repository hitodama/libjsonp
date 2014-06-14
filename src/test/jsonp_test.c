#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <jansson.h>

#include "libjsonp_helper.h"
#include "libjsonp_config.h"

#include "libjsonpp.h"
#include "libjsonp.h"

#define KS

int main(int argc, char **argv)
{
	int i;
	json_t *json;
	json_error_t error;
	FILE *inFile = stdin;
	FILE *outFile = stdout;

	int pp = -1;
	int in[] = {-1, -1};
	int out[] = {-1, -1};

	int verb = -1;
	int path = -1;
	int value = -1;

	int v = -1;
	json_t *jsonv = NULL;

	for(i = 1; i < argc; ++i)
	{
		if(strncmp(argv[i], "-pp", 4) == 0)
			pp = i;
		else if(strncmp(argv[i], "-i", 2) == 0)
		{
			size_t l = jsonp_strnlen(argv[i], JSONP_PATH_SIZE_MAX);
			in[0] = i;
			if(l == 2)
				in[1] = i + 1;
			else if(l < JSONP_PATH_SIZE_MAX)
				in[1] = i;
			else
			{
				fprintf(stderr, "Could not set IN! Path too long (> %i)?\n", JSONP_PATH_SIZE_MAX);
				return EXIT_FAILURE;
			}
			++i;
		}
		else if(strncmp(argv[i], "-o", 2) == 0)
		{
			size_t l = jsonp_strnlen(argv[i], JSONP_PATH_SIZE_MAX);
			out[0] = i;
			if(l == 2)
				out[1] = i + 1;
			else if(l < JSONP_PATH_SIZE_MAX)
				out[1] = i;
			else
			{
				fprintf(stderr, "Could not set OUT! Path too long (> %i)?\n", JSONP_PATH_SIZE_MAX);
				return EXIT_FAILURE;
			}
			++i;
		}
	}

	if(out[1] >= argc || in[1] >= argc)
	{
		fprintf(stderr, "Missing Path at the end\n");
		return EXIT_FAILURE;
	}

	for(i = 1; i < argc; ++i)
		if(i != pp && i != in[0] && i != in[1] && i != out[0] && i != out[1])
		{
			if(verb == -1)
				verb = i;
			else if(path == -1)
				path = i;
			else
				value = i;

		}

	if(verb == -1)
	{
		fprintf(stderr, "Verb not defined! \n");
		return EXIT_FAILURE;
	}

	if(path == -1)
	{
		fprintf(stderr, "Path not defined! \n");
		return EXIT_FAILURE;
	}

	if(in[1] >= 0)
	{
		inFile = fopen(argv[in[1]] + (in[0] == in[1] ? 2 : 0), "rb");
		if(inFile == NULL)
			fprintf(stderr, "IN file does not exist!\n");
	}
	if(out[1] >= 0)
		outFile = fopen(argv[out[1]] + (out[0] == out[1] ? 2 : 0), "wb");

	if(strncmp(argv[verb], "create", 7) == 0)// && value != -1)
		v = 0;
	else if(strncmp(argv[verb], "get", 4) == 0)
		v = 1;
	else if(strncmp(argv[verb], "set", 4) == 0)// && value != -1)
		v = 2;
	else if(strncmp(argv[verb], "delete", 7) == 0)
		v = 3;
	else
	{
		fprintf(stderr, "Verb not accepted or value missing!\n");
		return EXIT_FAILURE;
	}

	json = json_loadf(inFile, JSON_DECODE_ANY, &error);
	if(!json)
	{
		 fprintf(stderr, "%s\n", error.text);
		 return EXIT_FAILURE;
	}

	if(value != -1)
	{
		jsonv = json_loads(argv[value], JSON_DECODE_ANY, &error);
		if(!jsonv)
		{
			 fprintf(stderr, "%s\n", error.text);
			 return EXIT_FAILURE;
		}
	}

	if(pp >= 0)
	{
		// if(verb >= 0)
		// 	printf("> %i %s\n", verb, argv[verb]);
		// if(path >= 0)
		// 	printf("> %i %s\n", path, argv[path]);
		switch(v)
		{
			case 0: /*create*/
				jsonpp_create(json, argv[path], jsonv);
				break;
			case 1: /*get*/
				json = jsonpp_get(json, argv[path]);
				break;
			case 2: /*set*/
				jsonpp_set(json, argv[path], jsonv);
				break;
			case 3: /*delete*/
				jsonpp_delete(json, argv[path]);
				break;
		}
	}
	else
	{
		// if(verb >= 0)
		// 	printf(">> %i %s\n", verb, argv[verb]);
		// if(path >= 0)
		// 	printf(">> %i %s\n", path, argv[path]);
		switch(v)
		{
			case 0: /*create*/
				jsonp_create(json, argv[path], jsonv);
				break;
			case 1: /*get*/
				json = jsonp_get(json, argv[path]);
				break;
			case 2: /*set*/
				jsonp_set(json, argv[path], jsonv);
				break;
			case 3: /*delete*/
				jsonp_delete(json, argv[path]);
				break;
		}
	}


	if(json_dumpf(json, outFile, JSON_ENCODE_ANY) != 0)
	{
		fprintf(stderr, "Could not write to stdout!\n");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}