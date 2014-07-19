#ifndef JANSSON_EXTENSION_H
#define JANSSON_EXTENSION_H

#include <jansson.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum json_foreach_iteration { json_foreach_continue, json_foreach_break } json_foreach_iteration;

/*
These extensions generally merge the object and array functions of jansson.
Some extensions like merge also support strings where it makes sense.
*/

/*
The callback to be callen in each foreach loop step.
The first argument is the object, the second is the current key, the third is the current value.
Differs from janssons macro approach.
Macros would not work on an array+object appraoch.

@see json_foreach
*/
typedef json_foreach_iteration (*json_foreach_callback)(const char *, json_t *, void *);

/*
Returns the reference count of the json. Good for testing and introspection.

@param json JSON to retrieve reference for.
@returns Returns the reference count of the json.
*/
size_t json_ref(json_t *json);

/*
Create a type from the json type string representation.
@note Could be exposed / inlined.
@param type JSON a string representing the type.
@returns Returns type represented by the string.
*/
json_type json_stot(const char *str);

/*
Create a string representation of the json type. The string must be freed.
@note Could be exposed / inlined.
@param type JSON type be represented as string.
@returns Returns a string representing the type.
*/
const char *json_ttos(json_type type);

/*
Get the json object or array value assoziated with the property (or index) key.

@param json JSON object or array to look the key up.
@returns Returns the JSON value of key or NULL on error.
*/
json_t *json_get(json_t *json, const char *key);

/*
Set the json object or array property key (or index) to value.
Increases the reference count.

@param json JSON object or array to set the key.
@param key Key to set.
@returns Returns 0 on success and -1 on error.
*/
int json_set(json_t *json, const char *key, json_t* value);

/*
Set the json object or array property key (or index) to value.
Increases the reference count of value.

@param json JSON object or array to set the key.
@param key Key to set.
@param value Value to be stored
@returns Returns 0 on success and -1 on error.
*/
int json_set_new(json_t *json, const char *key, json_t *value);

/*
Remove the property key (or index) the json object or array.

@param json JSON object or array to remove the key in
@param key Key to store the value under
@param value Value to be stored
@returns Returns 0 on success and -1 on error.
*/
int json_remove(json_t *json, const char *key);

/*
Clears the json array, object or string.

@param json JSON object to be replaced
@param value Value to replace json with
@returns Returns 0 on success and -1 on error.
*/
int json_clear(json_t *json);

/*
Returns the size of the json array, object or string.

@param json JSON object to be cleared
@returns The size of the json array, object or string. Zero on Error.
*/
size_t json_size(const json_t *json);

/*
Loops over either array entries or object properties and calls cb for each.
Mem is passed through each time to communicate or store results or options.

@see json_foreach_callback
@param json JSON object to be messured
@param cb json_foreach callback
*/
json_foreach_iteration json_foreach(json_t *json, json_foreach_callback cb, void *mem);

/*
Merges either JSON arrays, objects or strings from other into json.

@param json JSON object to merge
@param other JSON object to merge
@returns Returns 0 on success and -1 on error.
*/
int json_merge(json_t *json, json_t *other);

#ifdef __cplusplus
}
#endif

#endif /*JANSSON_EXTENSION_H*/