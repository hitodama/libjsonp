#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libjsonp.h"
#include "jansson_extension.h"
#include "libjsonp_helper.h"
#include "libjsonp_config.h"
 
/*#define JSON_UNDEFINED 0x09*/
/* suppress clang warning range-compare*/
static const int json_undefined = 0x09; 

typedef json_t *(*jsonp_step)(json_t *, char *, json_type *);
typedef size_t (*jsonp_decoder)(char *, json_type *);
typedef int (*jsonp_setter)(json_t *, const char *, json_t *);

/*
	Different pointer approaches.
*/
static size_t jsonp_decoder_impl(char *str, json_type *type)
{
	size_t t = jsonp_strnirep(str, strlen(str), "~1", 2, "/", 1);
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

static size_t jsonpp_decoder_impl(char *str, json_type *type)
{
	size_t n = strlen(str);
	if(sscanf(str, "%%%02x", type))
		jsonp_strnirep(str, n, str, 3, "", 0);
	if(*type > json_undefined)
		*type = json_undefined;

	return jsonpp_url_decoder(str, n);
}

/*
	Different step approaches.
*/
static json_t *jsonp_simple_step(json_t *json, char *path, json_type *type)
{
	return json_get(json, path);
}

static json_t *jsonp_create_step(json_t *json, char *path, json_type *type)
{
	json_t *r = NULL;

	if(path != NULL)
		r = json_get(json, path);

	if(r != NULL)
	{
		if(json_typeof(r) == JSON_NULL)
			;
		else if(json_typeof(r) == *type)
			return r;
		else if(*type == json_undefined)
			return r;
	}

	if(*type == json_undefined)
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
static json_type jsonp_walk(json_t **ljson, char **lpath,
	json_t *json, char *path, char separator,
	jsonp_decoder decoder, jsonp_step step)
{
	size_t i;
	size_t j;
	json_type type = json_undefined;
	*ljson = NULL;
	*lpath = NULL;

	if(json == NULL || path == NULL || step == NULL || !json_is_object(json))
		return type;

	if(path[0] == separator)
	{
		for(j = 1, i = 1; path[i] != '\0'; ++i)
			if(path[i] == separator)
			{
				path[i] = '\0';
				decoder(path + j, &type);
				if(type != json_undefined && type != JSON_OBJECT && type != JSON_ARRAY)
					break;
				json = step(json, path + j, &type);
				type = json_undefined;
				if(json == NULL)
					break;

				//type = json_undefined;
				j = i + 1;
			}
	}
	else
	{
		i = strlen(path);
		j = 0;
	}

	if((json_is_array(json) || json_is_object(json)) && type != json_undefined)
		return json_undefined;

	if(json != NULL)
		decoder(path + j, &type);

	if(ljson != NULL && lpath != NULL)
	{
		*ljson = json;
		*lpath = path + j;
		if(json_is_array(json) && strncmp(*lpath, "-", 2) == 0)
			*lpath = NULL;
	}

	return type;
}

/*
	Exposed pointer functions / could be marcos or inlines / currently a lot of duplicates
*/
static JSON_INLINE json_t *jsonp_get_(json_t *json, const char *path, jsonp_decoder decoder)
{
	json_t *last; 
	char *lastpath;

	char *path_ = jsonp_strndup(path, JSONP_SIZE_MAX);

	if(json == NULL || !json_is_object(json))
		return NULL;

	if(path_ == NULL || strncmp(path_, "", 1) == 0)
		;
	else
	{
		jsonp_walk(&last, &lastpath, json, path_, JSONP_PATH_SEPERATOR, decoder, jsonp_simple_step);
		json = NULL;
		if(last != NULL)
			json = json_get(last, lastpath);
	}

	free(path_);

	return json;
}

json_t *jsonp_get(json_t *json, const char *path)
{
	return jsonp_get_(json, path, jsonp_decoder_impl);
}

json_t *jsonpp_get(json_t *json, const char *path)
{
	return jsonp_get_(json, path, jsonpp_decoder_impl);
}

static JSON_INLINE json_t *jsonp_set_(json_t *json, const char *path, json_t *value,
	jsonp_decoder decoder, jsonp_step step, jsonp_setter setter)
{
	json_type type;
	json_t *last; 
	char *lastpath;
	json_t *r = NULL;

	char *path_ = jsonp_strndup(path, JSONP_SIZE_MAX);
	
	if(json == NULL || decoder == NULL || step == NULL || setter == NULL || !json_is_object(json))
		r = NULL;
	else if(path_ == NULL || strncmp(path_, "", 1) == 0)
	{
		json_clear(json);
		if(value != NULL)
		{
			json_merge(json, value);
			r = json;
		}
	}
	else
	{
		int rr = -1;

		type = jsonp_walk(&last, &lastpath, json, path_, JSONP_PATH_SEPERATOR, decoder, step);

		//printf("%s %s %i %zu\n", path, lastpath, type, value);

		if(type != json_undefined)
		{
			json_t *t = json_oftype(type);
			if(value != NULL)
			{
				char *s = json_value_copy(value);
				json_value_set(t, s);
				free((void *)s);
			}
			rr = json_set_new(last, lastpath, t);
			value = t;
		}
		else
		{
			if(value != NULL)
			{
				rr = setter(last, lastpath, value);
			}
			else
			{
				value = json_null();
				rr = json_set_new(last, lastpath, value);
			}
		}
		if(rr != -1)
			r = value;
	}

	free(path_);

	return r;
}

int jsonp_set(json_t *json, const char *path, json_t *value)
{
	if(value == NULL)
		return -1;
	return (jsonp_set_(json, path, value, jsonp_decoder_impl, jsonp_simple_step, json_set) == NULL ? -1 : 0);
}

int jsonpp_set(json_t *json, const char *path, json_t *value)
{
	if(value == NULL)
		return -1;
	return (jsonp_set_(json, path, value, jsonpp_decoder_impl, jsonp_simple_step, json_set) == NULL ? -1 : 0);
}

int jsonp_set_new(json_t *json, const char *path, json_t *value)
{
	if(value == NULL)
		return -1;
	return (jsonp_set_(json, path, value, jsonp_decoder_impl, jsonp_simple_step, json_set_new) == NULL ? -1 : 0);
}

int jsonpp_set_new(json_t *json, const char *path, json_t *value)
{
	if(value == NULL)
		return -1;
	return (jsonp_set_(json, path, value, jsonpp_decoder_impl, jsonp_simple_step, json_set_new) == NULL ? -1 : 0);
}

json_t *jsonp_create(json_t *json, const char *path, json_t *value)
{
	return jsonp_set_(json, path, value, jsonp_decoder_impl, jsonp_create_step, json_set);
}

json_t *jsonpp_create(json_t *json, const char *path, json_t *value)
{
	return jsonp_set_(json, path, value, jsonpp_decoder_impl, jsonp_create_step, json_set);
}

json_t *jsonp_create_new(json_t *json, const char *path, json_t *value)
{
	return jsonp_set_(json, path, value, jsonp_decoder_impl, jsonp_create_step, json_set_new);
}

json_t *jsonpp_create_new(json_t *json, const char *path, json_t *value)
{
	return jsonp_set_(json, path, value, jsonpp_decoder_impl, jsonp_create_step, json_set_new);
}

static JSON_INLINE int jsonp_delete_(json_t *json, const char *path, jsonp_decoder decoder)
{
	json_t *last; 
	char *lastpath;
	int r = -1;

	char *path_ = jsonp_strndup(path, JSONP_SIZE_MAX);

	if(json == NULL || !json_is_object(json))
		return r;

	if(path_ == NULL || strncmp(path_, "", 1) == 0)
		return json_clear(json);

	jsonp_walk(&last, &lastpath, json, path_, JSONP_PATH_SEPERATOR, decoder, jsonp_simple_step);
	if(last != NULL)
		r = json_remove(last, lastpath);

	free(path_);

	return r;
}

int jsonp_delete(json_t *json, const char *path)
{
	return jsonp_delete_(json, path, jsonp_decoder_impl);
}

int jsonpp_delete(json_t *json, const char *path)
{
	return jsonp_delete_(json, path, jsonpp_decoder_impl);
}