#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "jansson_extension.h"
#include "libjsonp_helper.h"
#include "libjsonp_config.h"

const static char *json_type_strings[] = {"object", "array", "string", "integer", "real", "true", "false", "null"};

size_t json_ref(json_t *json)
{
	return json->refcount;
}

json_type json_stot(const char *str)
{
	if(str == NULL)
		return 0;
	size_t i;
	for(i = 0; i < sizeof(json_type_strings); ++i)
		if(strcmp(str, json_type_strings[i]) == 0)
			return (json_type)i;
	return 0;
}

const char *json_ttos(json_type type)
{
	if((int)type >= sizeof(json_type_strings))
		return NULL;
	return json_type_strings[type];
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
	else if(json_is_string(json))
		return json_string_set(json, "");

	return -1;
}

json_foreach_iteration json_foreach(json_t *json, json_foreach_callback cb, void *mem)
{
	size_t index;
	const char *key;
	json_t* value;
	json_foreach_iteration r = json_foreach_continue;

	if(json == NULL || cb == NULL)
		return r;

	if(json_is_object(json))
	{
		json_object_foreach(json, key, value)
		{
			r = cb(key, value, mem);
			if(r == json_foreach_break)
				break;
		}
	}
	else if(json_is_array(json))
	{
		key = malloc(sizeof(char) * (jsonp_digits(sizeof(JSONP_SIZE_MAX)) + 2)); /*pre define some const?*/
		json_array_foreach(json, index, value)
		{
			sprintf((char *)key, "%zu", index);
			r = cb(key, value, mem);
			if(r == json_foreach_break)
				break;
		}
		free((void *)key);
	}

	return r;
}

size_t json_size(const json_t *json)
{
	if(json == NULL)
		return 0;

	if(json_is_object(json))
		return json_object_size(json);
	else if(json_is_array(json))
		return json_array_size(json);
	else if(json_is_string(json))
	{
		const char *c = json_string_value(json);
		if(c != NULL)
			return strlen(c);
	}

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

int json_merge(json_t *json, json_t *other) /*extend cases?*/
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