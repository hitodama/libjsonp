#ifndef JSONPP_H
#define JSONPP_H

#include <jansson.h>

#ifdef __cplusplus
extern "C" {
#endif

json_t *jsonpp_create(json_t *json, const char *pointer, json_t *value);
json_t *jsonpp_create_new(json_t *json, const char *pointer, json_t *value);
json_t *jsonpp_get(json_t *json, const char *pointer);
int jsonpp_set(json_t *json, const char *pointer, json_t *value);
int jsonpp_set_new(json_t *json, const char *pointer, json_t *value);
int jsonpp_delete(json_t *json, const char *pointer);

#ifdef __cplusplus
}
#endif

#endif /*JSONPP_H*/