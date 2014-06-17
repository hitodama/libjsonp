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
Create a default object of the given json_type with a typical neutral value.

@param type JSON type to create an object for.
@returns Returns an object of the given type.
*/
json_t *json_oftype(json_type type);

/*
Identifies the json type of a given string.

@param str JSON type to create an object for.
@returns Returns an object of the given type.
*/
json_type json_typeofs(const char *str);

/*
Returns a json from a given string.

@param str JSON in string from.
@returns Returns the JSON contained within str.
*/
json_t *json_ofvalue(const char *str);

/*
Invers an object or array into a new object.

@param json JSON object or array.
@returns Returns an object containing key and value arrays or NULL.
*/
json_t *json_invert(json_t *json);

/*
Create a type from the json type string representation.
@note Could be exposed / inlined.
@param type JSON a string representing the type.
@returns Returns type represented by the string.
*/
json_type json_type_from_string(const char *str);

/*
Create a string representation of the json type. The string must be freed.
@note Could be exposed / inlined.
@param type JSON type be represented as string.
@returns Returns a string representing the type.
*/
const char *json_type_to_string(json_type type);

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
Combines an array of keys and an array of values into a single object.
The value array may be null or smaller. The key array may also be smaller.
The result is as one would expect.

@param keys Keys for the new object
@param values Values for the new object
@returns Returns the combined JSON object or NULL on error.
*/
json_t *json_object_zip(json_t *keys, json_t *values);

/*
Splits an objects into its keys and values and returns them in a new object.
The new object contains two arrays. One is stored under "keys", one under "values".

@param json JSON object to be split
@returns Returns an object container with the keys and values or NULL on error.
*/
json_t *json_object_unzip(json_t *json);

/*
Replaces the value of json with value.
The type of JSON is not altered (not currently possible in jansson).
Instead the value is casted to the type where possible.
If no cast was possible, the function returns without action.

@warning The type of json is not altered (not currently possible in jansson). True, false or null can never be replaced (again due to jansson).
@note We could use a double pointer approach but it would result in other problems.
@param json JSON to be replaced (Any but null, false or true)
@param value The value to be set
@returns Returns 0 on success and -1 on error.
*/
int json_value_set(json_t *json, const char *value);

/*
Returns the value assoziated with the json string, integer or real as string.

@warning The returned value must be freed
@param json JSON to be replaced (Any but null, false or true)
@returns Returns a string representing the value in json. Must be freed.
*/
char *json_value_copy(json_t *json);

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