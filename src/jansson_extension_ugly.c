#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "jansson_extension_ugly.h"
#include "jansson_extension.h"
#include "libjsonp_helper.h"
#include "libjsonp_config.h"

#define JSON_FORMAT_STRING(x) "%" x

json_t *json_oftype(json_type type)
{
	switch(type)
	{
		case JSON_OBJECT:
			return json_object();
		case JSON_ARRAY:
			return json_array();
		case JSON_STRING:
			return json_string("");
		case JSON_INTEGER:
			return json_integer(0);
		case JSON_REAL:
			return json_real(0.0);
		case JSON_TRUE:
			return json_true();
		case JSON_FALSE:
			return json_false();
		case JSON_NULL:
			return json_null();
		default:
			return NULL;
	}
}

json_type json_typeofs(const char *str)
{
	size_t i = 0;
	int justnumbers = 1;
	int numbers = 0;
	int dots = 0;

	if(str == NULL)
		return JSON_NULL;

	if(*str == '\0')
		return JSON_STRING;

	if(*str == '-' || *str == '+')
		i = 1;

	for(; str[i] != '\0'; ++i)
	{
		if(str[i] == '.')
			++dots;
		else if(str[i] < '0' || str[i] > '9')
			justnumbers = 0;
		else
			++numbers;
	}

	if(justnumbers)
	{
		if(dots == 0 && numbers < jsonp_digits(sizeof(json_int_t)))
			return JSON_INTEGER;
		else if(dots == 1 && numbers) /* < jsonp_digits(sizeof(double)))*/
			return JSON_REAL;
	}
	else
	{
		if (strcmp(str, json_ttos(JSON_FALSE)) == 0)
			return JSON_FALSE;
		if (strcmp(str, json_ttos(JSON_TRUE)) == 0)
			return JSON_TRUE;
		if (strcmp(str, json_ttos(JSON_NULL)) == 0)
			return JSON_NULL;
	}

	if((str[0] == '{' && str[i - 1] == '}') || (str[0] == '[' && str[i - 1] == ']') )
	{
		/* use unpack to verify? */
		json_t *r = json_loads(str, JSON_DECODE_ANY, NULL);
		if(r != NULL)
		{
			json_type t = json_typeof(r);
			json_decref(r);
			return t;
		}
	}

	return JSON_STRING;
}

json_t *json_ofvalue(const char *str)
{
	json_t *r = NULL;
	json_type t = json_typeofs(str);

	if(str == NULL)
		return r;

	switch(t)
	{
		case JSON_STRING:
			r = json_string(str);
			break;
		case JSON_INTEGER:
			{
				json_int_t t = 0;
				if(sscanf(str, JSON_FORMAT_STRING(JSON_INTEGER_FORMAT), &t) > 0)
					r = json_integer(t);
			}
			break;
		case JSON_REAL:
			{
				double t = 0;
				if(sscanf(str, "%lf", &t) > 0)
					r = json_real(t);
			}
			break;
		case JSON_FALSE:
			r = json_false();
			break;
		case JSON_TRUE:
			r = json_true();
			break;
		case JSON_NULL:
			r = json_null();
			break;

		default:
			r = json_loads(str, JSON_DECODE_ANY, NULL);
			break;
	}

	return r;
}

json_t *json_object_zip(json_t *keys, json_t *values)
{
	size_t key;
	json_t *value;
	json_t *r;

	if(!json_is_array(keys))
		return NULL;
	if(values != NULL && !json_is_array(values))
		return NULL;

	r = json_object();
	json_array_foreach(keys, key, value)
	{
		char * tk = json_value_copy(value);
		json_t *v = NULL;
		if(values != NULL)
			v = json_array_get(values, key);

		if(tk != NULL && v != NULL)
			json_set(r, tk, v);
		else if(tk != NULL)
			json_set(r, tk, json_null());

		free((void *)tk);
	}
	return r;
}

json_t *json_object_unzip(json_t *json)
{
	const char *key;
	json_t *value;
	json_t *r;
	json_t *k;
	json_t *v;

	if(!json_is_object(json))
		return NULL;

	r = json_object();
	k = json_array();
	json_set_new(r, "keys", k);
	v = json_array();
	json_set_new(r, "values", v);

	json_object_foreach(json, key, value)
	{
		json_set_new(k, NULL, json_string(key));
		json_set(v, NULL, value);
	}

	return r;
}

json_foreach_iteration json_inverter(const char *key, json_t *val, void *mem)
{
	json_t *r = (json_t *)mem;

	char *v = json_value_copy(val);
	if(v == NULL)
		return json_foreach_break;

	json_set_new(r, v, json_ofvalue(key));

	free((void *)v);

	return json_foreach_continue;
}

json_t *json_invert(json_t *json)
{
	json_t *r = json_object();
	json_foreach(json, json_inverter, (void *)r);
	return r;
}

int json_value_set(json_t *json, const char *value)
{
	int r = -1;

	if(json == NULL || value == NULL)
		return r;

	if(json_is_string(json))
		r = json_string_set(json, value);
	else if(json_is_integer(json))
	{
		json_int_t t = 0;
		if(sscanf(value, JSON_FORMAT_STRING(JSON_INTEGER_FORMAT), &t) > 0)
			r = json_integer_set(json, t);
	}
	else if(json_is_real(json))
	{
		double t = 0;
		if(sscanf(value, "%lf", &t) > 0)
			r = json_real_set(json, t);
	}
	else if(json_is_object(json) || json_is_array(json))
	{
		json_t *v = json_loads(value, 0, NULL);
		json_clear(json);
		json_merge(json, v);
		json_decref(v);
	}

	return r;
}

char *json_value_copy(json_t *json)
{
	char *r = NULL;
	json_type t = json_typeof(json);

	if(json == NULL)
		return r;

	switch(t)
	{
		case JSON_STRING:
			{
				const char *t = json_string_value(json);
				if(t != NULL)
					r = jsonp_strdup(t);
			}
			break;
		case JSON_INTEGER:
			{
				json_int_t t = json_integer_value(json);
				r = malloc(sizeof(char) * (jsonp_digits(sizeof(json_int_t)) + 2));
				if(r != NULL)
					sprintf(r, JSON_FORMAT_STRING(JSON_INTEGER_FORMAT), t);
			}
			break;
		case JSON_REAL:
			{
				double t = json_real_value(json);
				r = malloc(sizeof(char) * (jsonp_digits(sizeof(double)) + 2));
				if(r != NULL)
					sprintf(r, "%lf", t);
			}
			break;
		case JSON_FALSE:
		case JSON_TRUE:
		case JSON_NULL:
			{
				const char *s = json_ttos(t);
				r = malloc(sizeof(char) * (strlen(s) + 1));
				strcpy(r, s);
			}
			break;
		default:
			r = json_dumps(json, 0);
			break;
	}

	return r;
}