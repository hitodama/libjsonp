#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "jansson_extension.h"
#include "libjsonp_helper.h"
#include "libjsonp_config.h"

#define JSON_FORMAT_STRING(x) "%" x

size_t json_ref(json_t *json)
{
	return json->refcount;
}

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

JSON_INLINE int json_replace(json_t *json, json_t *value)
{
	json_type type;
	json_type vtype;
	json_type vtype2;
	size_t l = 0;
	int r = -1;

	if(json == NULL || value == NULL)
		return -1;

	type = json_typeof(json);
	vtype = json_typeof(value);

	char *s = json_dumps(value, JSON_ENCODE_ANY);
	if(s != NULL)
	{
		l = strlen(s);
		if(l >= 2 && s[0] == '\"' && s[l - 1] == '\"')
		{
			s[0] = ' ';
			s[l - 1] = ' ';
		}
	}
	json_t *v = json_loads(s, JSON_DECODE_ANY, NULL);

	vtype2 = json_typeof(v);

	switch(type)
	{
		case JSON_OBJECT:
		case JSON_ARRAY:
		{
			if(type == vtype)
			{
				r = json_clear(json);
				if(r >= 0)
					r = json_merge(json, value);
			}
			break;
		}
		case JSON_STRING:
		{
			r = json_string_set(json, s);
			break;
		}
		case JSON_INTEGER:
		{
			if(vtype2 == JSON_INTEGER)
			{
				json_int_t t = 0;
				r = sscanf(s, JSON_FORMAT_STRING(JSON_INTEGER_FORMAT), &t);
				if(r > 0)
					r = json_integer_set(json, t);
			}
			if(vtype2 == JSON_REAL)
			{
				double t;
				r = sscanf(s, "%lf", &t);
				if(r > 0)
					r = json_integer_set(json, (json_int_t)t);
			}
			break;
		}
		case JSON_REAL:
		{
			if(vtype2 == JSON_INTEGER)
			{
				json_int_t t = 0;
				r = sscanf(s, JSON_FORMAT_STRING(JSON_INTEGER_FORMAT), &t);
				if(r > 0)
					r = json_real_set(json, (double)t);
			}
			if(vtype2 == JSON_REAL)
			{
				double t;
				r = sscanf(s, "%lf", &t);
				if(r > 0)
					r = json_real_set(json, t);
			}
			break;
		}
		case JSON_TRUE: /*sadly not possible*/
		case JSON_FALSE:
		case JSON_NULL:
			break;
		default:
			break;

	}

	json_decref(v);
	free(s);

	return r;
}

int json_set_new(json_t *json, const char *key, json_t *value)
{
	if(json == NULL || value == NULL)
		return -1;

	if(json_is_object(json) && key != NULL)
		return json_object_set_new(json, key, value);
	else if(json_is_array(json))
	{
		size_t i = 0;
		if(key == NULL)
			return json_array_append_new(json, value);
		if(!jsonp_isint(key))
			return -1;
		if(jsonp_strtozu(&i, key) < 0)
			return json_array_insert_new(json, i, value);
		else
			return json_array_set_new(json, i, value);
	}

	return -1;
}

int json_set(json_t *json, const char *key, json_t* value)
{
	if(json == NULL || value == NULL)
		return -1;

	if(json_is_object(json) && key != NULL)
		return json_object_set(json, key, value);
	else if(json_is_array(json))
	{
		size_t i = 0;
		if(key == NULL)
			return json_array_append(json, value);
		if(!jsonp_isint(key))
			return -1;
		if(jsonp_strtozu(&i, key) < 0)
			return json_array_insert(json, i, value);
		else
			return json_array_set(json, i, value);
	}

	return -1;
}

json_t *json_get(json_t *json, const char *key)
{
	if(json == NULL || key == NULL)
		return NULL;

	if(json_is_object(json))
		return json_object_get(json, key);
	else if(json_is_array(json) && jsonp_isint(key))
	{
		size_t i = 0;
		if(jsonp_strtozu(&i, key) < 0)
			/* size_t s = json_array_size(json); return json_array_get(json, s - 1);*/
			return NULL;
		return json_array_get(json, i);
	}

	return NULL;
}

int json_clear(json_t *json)
{
	if(json == NULL)
		return -1;

	if(json_is_object(json))
		return json_object_clear(json);
	else if(json_is_array(json))
		return json_array_clear(json);

	return -1;
}

void json_foreach(json_t *json, void (*cb)(json_t *, const char *, json_t *))
{
	size_t index;
	char *key;
	json_t* value;

	if(json == NULL || cb == NULL)
		return;

	if(json_is_object(json))
	{
		json_object_foreach(json, key, value)
		{
			cb(json, key, value);
		}
	}
	else if(json_is_array(json))
	{
		key = malloc(sizeof(char) * (jsonp_zudigits(JSONP_SIZE_MAX) + 1)); /*pre define some const?*/
		json_array_foreach(json, index, value)
		{
			sprintf(key, "%zu", index);
			cb(json, key, value);
		}
		free(key);
	}
}

size_t json_size(const json_t *json)
{
	if(json == NULL)
		return 0;

	if(json_is_object(json))
		return json_object_size(json);
	else if(json_is_array(json))
		return json_array_size(json);

	return 0;
}

int json_remove(json_t *json, const char *key)
{
	if(json == NULL || key == NULL)
		return -1;

	if(json_is_object(json))
		return json_object_del(json, key);
	else if(json_is_array(json) && jsonp_isint(key))
	{
		size_t i = 0;
		if(jsonp_strtozu(&i, key) < 0)
			return json_array_remove(json, i);
	}

	return -1;
}

int json_merge(json_t *json, json_t *other)
{
	if(json == NULL || other == NULL)
		return -1;

	if(json_is_object(json) && json_is_object(other))
		return json_object_update(json, other);
	else if(json_is_array(json) && json_is_array(other))
		return json_array_extend(json, other);
	else if(json_is_string(json) && json_is_string(other))
	{
		int r = -1;
		const char *a = json_string_value(json);
		const char *b = json_string_value(other);
		if(a != NULL &&  b != NULL)
		{
			size_t l = strlen(a) + strlen(b);
			char *s = (char *)malloc(sizeof(char) * (l + 1));
			strcpy(s, a);
			strcat(s, b);
			r = json_string_set(json, s);
			free(s);
		}
		return r;
	}

	return -1;
}