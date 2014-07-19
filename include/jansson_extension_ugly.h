#ifndef JANSSON_EXTENSION_UGLY_H
#define JANSSON_EXTENSION_UGLY_H

#include <jansson.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif /*JANSSON_EXTENSION_UGLY_H*/