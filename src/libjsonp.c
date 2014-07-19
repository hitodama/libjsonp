#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "libjsonp.h"
#include "jansson_extension.h"
//#include "jansson_extension_ugly.h"
#include "libjsonp_helper.h"
#include "libjsonp_config.h"
 
/* #define JSON_UNDEFINED 0x09 */
/* suppress clang warning range-compare */

static const int json_type_last = 0x07;
static const int json_undefined = 0x08;
static const int json_error = 0x09;

typedef json_t *(*jsonp_step_t)(json_t *, char *, json_type);
typedef json_type (*jsonp_decoder_t)(char *);
typedef int (*jsonp_setter_t)(json_t *, const char *, json_t *);

static size_t jsonpp_tilde_decoder(char *str, size_t n)
{
	n = jsonp_strnirep(str, n, "~1", 2, "/", 1);
	return jsonp_strnirep(str, n, "~0", 2, "~", 1);
}

static json_type jsonp_decoder(char *str)
{
	size_t n = strlen(str);
	jsonpp_tilde_decoder(str, n);
	return json_undefined;

	/*
	json_type type = json_undefined;
	size_t n = strlen(str);
	size_t i, l;
	char c[3];

	c[0] = '~';
	c[2] = '\0';
	for(i = 0; i <= json_type_last; ++i)
	{
		l = n;
		c[1] = json_type_to_string(i)[0];
		n = jsonp_strnirep(str, n, c, 2, "", 0);
		if(l != n)
			type = i;
	}

	jsonpp_tilde_decoder(str, n);

	return type;
	*/
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
			if(i > n - 2 || str[i + 1] == '\0' || str[i + 2] == '\0')
				return json_error;

			sscanf(str + i + 1, "%02x", &t);

			str[i] = '\0';
			if(isprint(t))
				str[i] = (char)t;
			i++;
			str[i++] = '\0';
			str[i] = '\0';
		}

	jsonp_memsh(str, 0, n);
	return n - jsonp_memcnt(str, 0, n);
}

static json_type jsonpp_decoder(char *str)
{
	json_type type = json_undefined;
	size_t n = strlen(str);
	char *s;
	char c[4];

	for(s = strstr(str, "%"); s != NULL; s = strstr(s, "%"))
	{
		if(s[1] == '0' && s[2] >= '0' && s[2] <= '7')
		{
			if(sscanf(s, "%%%02x", &type) > 0)
			{
				sprintf(c, "%%%02x", type);
				n = jsonp_strnirep(str, n, c, 3, "", 0);
				s += 2;
			}
		}
		s += 1;
	}

	if(type > json_type_last && type != json_undefined)
		type = json_error;
	else
	{
		n = jsonpp_tilde_decoder(str, n);
		jsonpp_url_decoder(str, n);
	}

	return type;
}

static json_t *jsonp_simple_step(json_t *json, char *path, json_type type)
{
	return json_get(json, path);
}

static json_t *jsonp_create_step(json_t *json, char *path, json_type type)
{
	json_t *r = NULL;

	if(path != NULL)
		r = json_get(json, path);

	if(type == json_undefined)
	{
		if(r != NULL && (json_is_array(r) || json_is_object(r)))
			return r;
		if(r == NULL || json_is_null(r)) /*override or not*/
			r = json_object();
		else
		 	r = NULL;
	}
	else
	{
		if(r != NULL && type == json_typeof(r))
			return r;
		r = json_oftype(type);
	}
	
	if(r != NULL && json_set_new(json, path, r) < 0)
		return NULL;

	return r;
}

static int jsonp_walk(json_t **outjson, char **outpath, json_t *json,
	char *path, char separator, jsonp_decoder_t decoder, jsonp_step_t step)
{
	json_type type = json_undefined;
	size_t i;
	size_t j;

	if(json == NULL || path == NULL || step == NULL || !json_is_object(json))
		return type;

	if(path[0] == separator)
	{
		for(j = 1, i = 1; path[i] != '\0'; ++i)
			if(path[i] == separator)
			{
				path[i] = '\0';

				type = decoder(path + j);
				if(type == json_error)
					return json_error;

				if(json_is_array(json) && strncmp(path + j, "-", 2) == 0)
					json = step(json, NULL, type);
				else
					json = step(json, path + j, type);

				if(json == NULL)
					return json_error; //break;

				j = i + 1;
			}
	}
	else
	{
		i = strlen(path);
		j = 0;
	}

	if(json != NULL)
		type = decoder(path + j);

	if(outjson != NULL && outpath != NULL)
	{
		*outjson = json;
		*outpath = path + j;
		if(json_is_array(json) && strncmp(*outpath, "-", 2) == 0)
			*outpath = NULL;
	}

	return type;
}

static JSON_INLINE json_t *jsonp_get_(json_t *json, const char *path, jsonp_decoder_t decoder)
{
	json_t *last; 
	char *lastpath;

	char *path_ = jsonp_strdup(path);

	if(json == NULL || !json_is_object(json))
		return NULL;

	if(path_ == NULL || strncmp(path_, "", 1) == 0)
		;
	else
	{
		int r = jsonp_walk(&last, &lastpath, json, path_, JSONP_PATH_SEPERATOR, decoder, jsonp_simple_step);
		
		json = NULL;
		if(r != json_error)
		{
			if(last != NULL)
				json = json_get(last, lastpath);
		}
	}

	free(path_);

	return json;
}

json_t *jsonp_get(json_t *json, const char *path)
{
	return jsonp_get_(json, path, jsonp_decoder);
}

json_t *jsonpp_get(json_t *json, const char *path)
{
	return jsonp_get_(json, path, jsonpp_decoder);
}

static JSON_INLINE json_t *jsonp_create_(json_t *json, const char *path, json_t *value,
	jsonp_decoder_t decoder, jsonp_step_t step, jsonp_setter_t setter)
{
	json_t *last = NULL; 
	char *lastpath = NULL;
	json_t *r = NULL;

	char *path_ = jsonp_strdup(path);
	
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
		json_type type = json_undefined;
		int rr = -1;

		type = jsonp_walk(&last, &lastpath, json, path_, JSONP_PATH_SEPERATOR, decoder, step);

		if(type != json_error)
		{
			if(value != NULL)
			{
				if(type != json_undefined)
				{
					json_t *o = json_get(last, lastpath);
					if(o == NULL || type != json_typeof(o))
						o = json_oftype(type);
				
					char *c = json_value_copy(value);
					json_value_set(o, c);
					free((void *)c);
					value = o;

					rr = json_set_new(last, lastpath, value);
				}
				else
					rr = setter(last, lastpath, value);
			}
			else
			{
				if(type > json_type_last)
					value = json_null();
				else
					value = json_oftype(type);
				rr = json_set_new(last, lastpath, value);
			}
			if(rr >= 0)
				r = value;
		}
		else
			r = NULL;
	}

	free(path_);

	return r;
}

json_t *jsonp_create(json_t *json, const char *path, json_t *value)
{
	return jsonp_create_(json, path, value, jsonp_decoder, jsonp_create_step, json_set);
}

json_t *jsonpp_create(json_t *json, const char *path, json_t *value)
{
	return jsonp_create_(json, path, value, jsonpp_decoder, jsonp_create_step, json_set);
}

json_t *jsonp_create_new(json_t *json, const char *path, json_t *value)
{
	return jsonp_create_(json, path, value, jsonp_decoder, jsonp_create_step, json_set_new);
}

json_t *jsonpp_create_new(json_t *json, const char *path, json_t *value)
{
	return jsonp_create_(json, path, value, jsonpp_decoder, jsonp_create_step, json_set_new);
}

/*
static JSON_INLINE int jsonp_set_(json_t *json, const char *path, json_t *value,
	jsonp_decoder_t decoder, jsonp_step_t step, jsonp_setter_t setter)
{
	json_t *last = NULL; 
	char *lastpath = NULL;
	int r = -1;

	char *path_ = jsonp_strdup(path);
	
	if(json == NULL || value == NULL || decoder == NULL || step == NULL || setter == NULL || !json_is_object(json))
		r = -1;
	else if(path_ == NULL || strncmp(path_, "", 1) == 0)
	{
		json_clear(json);
		r = json_merge(json, value);
	}
	else
	{
		jsonp_walk(&last, &lastpath, json, path_, JSONP_PATH_SEPERATOR, decoder, step);
		
		if(last != NULL)
			r = setter(last, lastpath, value);
	}

	free(path_);

	return r;
}
*/

int jsonp_set(json_t *json, const char *path, json_t *value)
{
	return jsonp_create_(json, path, value, jsonp_decoder, jsonp_simple_step, json_set) == NULL ? -1 : 0 ;
}

int jsonpp_set(json_t *json, const char *path, json_t *value)
{
	return jsonp_create_(json, path, value, jsonpp_decoder, jsonp_simple_step, json_set) == NULL ? -1 : 0;
}

int jsonp_set_new(json_t *json, const char *path, json_t *value)
{
	return jsonp_create_(json, path, value, jsonp_decoder, jsonp_simple_step, json_set_new) == NULL ? -1 : 0;
}

int jsonpp_set_new(json_t *json, const char *path, json_t *value)
{
	return jsonp_create_(json, path, value, jsonpp_decoder, jsonp_simple_step, json_set_new) == NULL ? -1 : 0;
}

static JSON_INLINE int jsonp_delete_(json_t *json, const char *path, jsonp_decoder_t decoder)
{
	json_t *last; 
	char *lastpath;
	int r = -1;

	char *path_ = jsonp_strdup(path);

	if(json == NULL || !json_is_object(json))
		return r;

	if(path_ == NULL || strncmp(path_, "", 1) == 0)
		return json_clear(json);

	r = jsonp_walk(&last, &lastpath, json, path_, JSONP_PATH_SEPERATOR, decoder, jsonp_simple_step);
	
	if(r != json_error)
	{
		if(last != NULL)
			r = json_remove(last, lastpath);
	}
	else
		r = -1;

	free(path_);

	return r;
}

int jsonp_delete(json_t *json, const char *path)
{
	return jsonp_delete_(json, path, jsonp_decoder);
}

int jsonpp_delete(json_t *json, const char *path)
{
	return jsonp_delete_(json, path, jsonpp_decoder);
}