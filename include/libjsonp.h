#ifndef JSONP_H
#define JSONP_H

#include <jansson.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
Follows the jsonp path and if successful adds the value to the json object.
The path can have any depth within the json object, provided 
*/
json_t *jsonp_create(json_t *json, const char *pointer, json_t *value);

json_t *jsonp_create_new(json_t *json, const char *pointer, json_t *value);

json_t *jsonp_get(json_t *json, const char *pointer);

int jsonp_set(json_t *json, const char *pointer, json_t *value);

int jsonp_set_new(json_t *json, const char *pointer, json_t *value);

int jsonp_delete(json_t *json, const char *pointer);

#ifdef __cplusplus
}
#endif

#endif /*JSONP_H*/