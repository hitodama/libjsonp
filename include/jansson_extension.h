#ifndef JANSSON_EXTENSION_H
#define JANSSON_EXTENSION_H

#include <jansson.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t json_ref(json_t *json);
json_t *json_oftype(json_type type);

json_t *json_get(json_t *json, const char *key);
int json_set(json_t *json, const char *key, json_t* value);
int json_set_new(json_t *json, const char *key, json_t *value);
int json_remove(json_t *json, const char *key);

int json_replace(json_t *json, json_t *value);

int json_clear(json_t *json);
size_t json_size(const json_t *json);
void json_foreach(json_t *json, void (*cb)(json_t *, const char *, json_t *));
int json_merge(json_t *json, json_t *other);

#ifdef __cplusplus
}
#endif

#endif /*JANSSON_EXTENSION_H*/