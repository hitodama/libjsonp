#ifndef JANSSON_EXTENSION_H
#define JANSSON_EXTENSION_H

#include <jansson.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
These extensions generally merge the object and array functions of jansson.
Some extensions like merge also support strings where it makes sense.
*/

/*
The callback to be callen in each foreach loop step.
Differs from janssons macro approach.
Macros would not work on an array+object appraoch.

@see json_foreach
*/
typedef void (*json_foreach_callback)(json_t *, const char *, json_t *);

/*
Returns the reference count of the json. Good for testing and introspection.

@param json JSON to retrieve reference for.
@returns Returns the reference count of the json.
*/
size_t json_ref(json_t *json);

/*
Create a default object of the given json_type with a typical neutral value.

@param type JSON type to create an object for.
@returns Returns an object of the given type.
*/
json_t *json_oftype(json_type type);

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
Replaces the value of json with value.
The type of JSON is not altered (not currently possible in jansson).
Instead the value is casted to the type where possible.
If no cast was possible, the function returns in error.

@warning The type of JSON is not altered (not currently possible in jansson). True, false or null can never be replaced (again due to jansson).

@note We could use a double pointer approach but it would result in other problems.
@param json JSON to be replaced (Any but null, false or true)
@param json JSON object
@returns Returns 0 on success and -1 on error.
*/
int json_replace(json_t *json, json_t *value);

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
Loops over either array entries or object properties.

@see json_foreach_callback
@param json JSON object to be messured
@param cb json_foreach callback
*/
void json_foreach(json_t *json, json_foreach_callback cb);

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