#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libjsonp.h"
#include "jansson_extension.h"
#include "libjsonp_helper.h"
#include "libjsonp_config.h"
 
#define JSON_UNDEFINED 0x09

typedef json_t *(*jsonp_step)(json_t *, char *, size_t, json_type *);
typedef size_t (*jsonp_decoder)(char *, size_t, json_type *);

/*
	Different pointer approaches.
*/
static size_t jsonp_decode(char *str, size_t n, json_type *type)
{
	size_t t = jsonp_strnirep(str, n, "~1", 2, "/", 1);
	return jsonp_strnirep(str, t, "~0", 2, "~", 1);
}

static size_t jsonpp_url_decoder(char *str, size_t n)
{
	size_t i;
	unsigned int t;

	if(n == 0)
		return 0;

	for(i = 0; str[i] != '\0' && i < n; ++i)
		if(str[i] == '%')
		{
			if(i > n - 2)
				return n;

			sscanf(str + i + 1, "%02x", &t);

			str[i++] = (char)t;
			str[i++] = '\0';
			str[i] = '\0';
		}

	jsonp_memsh(str, 0, n);
	return n - jsonp_memcnt(str, 0, n);
}

static size_t jsonpp_decode(char *str, size_t n, json_type *type)
{
	if(sscanf(str, "%%%02x", type))
		jsonp_strnirep(str, n, str, 3, "", 0);

	return jsonpp_url_decoder(str, n);
}

/*
	Different step approaches.
*/
static json_t *jsonp_simple_step(json_t *json, char *path, size_t n, json_type *type)
{
	return json_get(json, path);
}

static json_t *jsonp_create_step(json_t *json, char *path, size_t n, json_type *type)
{
	json_t *r = NULL;

	r = json_get(json, path);

	if(r != NULL)
	{
		if(json_typeof(r) == JSON_NULL)
			;
		else if(json_typeof(r) == *type)
			return r;
		else if(*type == JSON_UNDEFINED)
			return r;
	}

	if(*type == JSON_UNDEFINED)
		*type = JSON_OBJECT;

	r = json_oftype(*type);

	if(json_is_array(json) && strncmp(path, "-", 2) == 0)
		path = NULL;

	if(json_set_new(json, path, r) < 0)
		return NULL;

	return r;
}

/*
	Walk the json pointer.
*/
static int jsonp_walk(json_t **ljson, char **lpath, size_t *llen,
	json_t *json, char *path, size_t n, char separator,
	jsonp_step step, jsonp_decoder decoder)
{
	size_t i;
	size_t j;
	size_t t;
	json_type type = JSON_UNDEFINED;

	if(json == NULL || path == NULL || step == NULL || !json_is_object(json))
		return -1;

	if(path[0] == separator)
	{
		for(j = 1, i = 1; path[i] != '\0' && i < n; ++i)
			if(path[i] == separator)
			{
				path[i] = '\0';

				t = decoder(path + j, (j >= i) ? 0 : i - j, &type);
				if(type != JSON_UNDEFINED && type != JSON_OBJECT && type != JSON_ARRAY)
					break;
				json = step(json, path + j, t, &type);
				if(json == NULL)
					break;

				type = JSON_UNDEFINED;
				j = i + 1;
			}
	}
	else
	{
		i = jsonp_strnlen(path, n);
		j = 0;
	}

	if(json != NULL)
	{
		t = decoder(path + j, (j >= i) ? 0 : i - j, &type);
		if(step(json, path + j, t, &type) == NULL)
			return -1;
		//return -1;
	}

	if(ljson != NULL && lpath != NULL && llen != NULL)
	{
		*ljson = json;
		*lpath = path + j;
		*llen = t;
	}

	return 0;
}

/*
	Exposed pointer functions / could be marcos or inlines / currently a lot of duplicates
*/
static JSON_INLINE json_t *jsonp_get_(json_t *json, const char *path, jsonp_decoder decoder)
{
	size_t i = 0;
	json_t *last; 
	char *lastpath;
	size_t lastpathlength;

	char *path_ = jsonp_strndup(path, JSONP_PATH_SIZE_MAX);

	if(json == NULL || !json_is_object(json))
		return NULL;

	if(path_ == NULL || strncmp(path_, "", 1) == 0)
		;
	else
	{
		i = jsonp_walk(&last, &lastpath, &lastpathlength, json, path_, jsonp_strnlen(path_, JSONP_PATH_SIZE_MAX), JSONP_PATH_SEPERATOR, jsonp_simple_step, decoder);
		if(i >= 0)
			json = json_get(last, lastpath);
	}

	free(path_);

	return json;
}

json_t *jsonp_get(json_t *json, const char *path)
{
	return jsonp_get_(json, path, jsonp_decode);
}

json_t *jsonpp_get(json_t *json, const char *path)
{
	return jsonp_get_(json, path, jsonpp_decode);
}

static JSON_INLINE int jsonp_set_(json_t *json, const char *path, json_t *value, jsonp_decoder decoder)
{
	json_t *last; 
	char *lastpath;
	size_t lastpathlength;
	int r;

	char *path_ = jsonp_strndup(path, JSONP_PATH_SIZE_MAX);
	
	if(json == NULL || value == NULL || !json_is_object(json))
		r = -1;
	else if(path_ == NULL || strncmp(path_, "", 1) == 0)
	{
		json_clear(json);
		r = json_merge(json, value);
	}
	else
	{
		r = jsonp_walk(&last, &lastpath, &lastpathlength, json, path_, jsonp_strnlen(path_, JSONP_PATH_SIZE_MAX), JSONP_PATH_SEPERATOR, jsonp_simple_step, decoder);
		if(r >= 0)
			r = json_set(last, lastpath, value);
	}

	free(path_);

	return r;
}

int jsonp_set(json_t *json, const char *path, json_t *value)
{
	return jsonp_set_(json, path, value, jsonp_decode);
}

int jsonpp_set(json_t *json, const char *path, json_t *value)
{
	return jsonp_set_(json, path, value, jsonpp_decode);
}

static JSON_INLINE int jsonp_set_new_(json_t *json, const char *path, json_t *value, jsonp_decoder decoder)
{
	json_t *last; 
	char *lastpath;
	size_t lastpathlength;
	int r;

	char *path_ = jsonp_strndup(path, JSONP_PATH_SIZE_MAX);

	if(json == NULL || value == NULL || !json_is_object(json))
		r = -1;
	else if(path_ == NULL || strncmp(path_, "", 1) == 0)
	{
		json_clear(json);
		r = json_merge(json, value);
	}
	else
	{
		r = jsonp_walk(&last, &lastpath, &lastpathlength, json, path_, jsonp_strnlen(path_, JSONP_PATH_SIZE_MAX), JSONP_PATH_SEPERATOR, jsonp_simple_step, decoder);
		if(r >= 0)
			r = json_set_new(last, lastpath, value);
	}

	free(path_);

	return r;
}

int jsonp_set_new(json_t *json, const char *path, json_t *value)
{
	return jsonp_set_new_(json, path, value, jsonp_decode);
}

int jsonpp_set_new(json_t *json, const char *path, json_t *value)
{
	return jsonp_set_new_(json, path, value, jsonpp_decode);
}

static JSON_INLINE int jsonp_create_(json_t *json, const char *path, json_t *value, jsonp_decoder decoder)
{
	json_t *last;
	char *lastpath;
	size_t lastpathlength;
	int r;

	char *path_ = jsonp_strndup(path, JSONP_PATH_SIZE_MAX);

	if(json == NULL || !json_is_object(json))
		r = -1;
	else if(path_ == NULL || strncmp(path_, "", 1) == 0)
	{
		r = json_clear(json);
		if(value != NULL)
			r = json_merge(json, value);
	}
	else
	{
		r = jsonp_walk(&last, &lastpath, &lastpathlength, json, path_, jsonp_strnlen(path_, JSONP_PATH_SIZE_MAX), JSONP_PATH_SEPERATOR, jsonp_create_step, decoder);
		if(r >= 0)
			r = json_set(last, lastpath, value);
	}

	free(path_);

	return r;
}

int jsonp_create(json_t *json, const char *path, json_t *value)
{
	return jsonp_create_(json, path, value, jsonp_decode);
}

int jsonpp_create(json_t *json, const char *path, json_t *value)
{
	return jsonp_create_(json, path, value, jsonpp_decode);
}

static JSON_INLINE int jsonp_create_new_(json_t *json, const char *path, json_t *value, jsonp_decoder decoder)
{
	json_t *last;
	char *lastpath;
	size_t lastpathlength;
	int r;

	char *path_ = jsonp_strndup(path, JSONP_PATH_SIZE_MAX);

	if(json == NULL || !json_is_object(json))
		r = -1;
	else if(path_ == NULL || strncmp(path_, "", 1) == 0)
	{
		r = json_clear(json);
		if(value != NULL)
			r = json_merge(json, value);
	}
	else
	{
		r = jsonp_walk(&last, &lastpath, &lastpathlength, json, path_, jsonp_strnlen(path_, JSONP_PATH_SIZE_MAX), JSONP_PATH_SEPERATOR, jsonp_create_step, decoder);
		if(r >= 0)
			r = json_set_new(last, lastpath, value);
	}

	free(path_);

	return r;
}

int jsonp_create_new(json_t *json, const char *path, json_t *value)
{
	return jsonp_create_new_(json, path, value, jsonp_decode);
}

int jsonpp_create_new(json_t *json, const char *path, json_t *value)
{
	return jsonp_create_new_(json, path, value, jsonpp_decode);
}

static JSON_INLINE int jsonp_delete_(json_t *json, const char *path, jsonp_decoder decoder)
{
	json_t *last; 
	char *lastpath;
	size_t lastpathlength;
	int r;

	char *path_ = jsonp_strndup(path, JSONP_PATH_SIZE_MAX);

	if(json == NULL || !json_is_object(json))
		return -1;

	if(path_ == NULL || strncmp(path_, "", 1) == 0)
		return json_clear(json);

	r = jsonp_walk(&last, &lastpath, &lastpathlength, json, path_, jsonp_strnlen(path_, JSONP_PATH_SIZE_MAX), JSONP_PATH_SEPERATOR, jsonp_simple_step, decoder);
	if(r >= 0)
		r = json_remove(last, lastpath);

	free(path_);

	return r;
}

int jsonp_delete(json_t *json, const char *path)
{
	return jsonp_delete_(json, path, jsonp_decode);
}

int jsonpp_delete(json_t *json, const char *path)
{
	return jsonp_delete_(json, path, jsonpp_decode);
}