#ifndef JSONPP_H
#define JSONPP_H

#include <jansson.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
Stores the JSON value in the JSON document unter the JSON (tilde + URL) pointer.
Does increase the reference of value.
Unlike set, the create function creates intermediate objects or array
in the path and can be used to "touch" the path by setting the value to NULL
and specifying the type of the final path element (via ~a, ~o, ~i, ~s etc).
Note: If the final path elements type is defined, the value is allways casted to a new object.


@param json The JSON document used for the lookup
@param pointer The JSON pointer (tilde + URL encoding) 
@returns Returns the json identified by the pointer.
*/
json_t *jsonpp_create(json_t *json, const char *pointer, json_t *value);

/*
Stores the JSON value in the JSON document unter the JSON (tilde + URL) pointer.
Does not increase the reference of value.
Unlike set, the create function creates intermediate objects or array
in the path and can be used to "touch" the path by setting the value to NULL
and specifying the type of the final path element (via ~a, ~o, ~i, ~s etc).
Note: If the final path elements type is defined, the value is allways casted to a new object.

@param json The JSON document used for the lookup
@param pointer The JSON pointer (tilde + URL encoding) 
@returns Returns the json identified by the pointer.
*/
json_t *jsonpp_create_new(json_t *json, const char *pointer, json_t *value);

/*
Retrieves the value stored unter the JSON (tilde + URL) pointer in the JSON document.

@param json The JSON document used for the lookup
@param pointer The JSON pointer (tilde + URL encoding) 
@returns Returns the json identified by the pointer.
*/
json_t *jsonpp_get(json_t *json, const char *pointer);

/*
Stores the JSON value in the JSON document unter the JSON (tilde + URL) pointer.
Also increases the reference of value.

@param json The JSON document used for the lookup
@param pointer The JSON pointer (tilde + URL encoding)
@param pointer The JSON document to be stored
@returns Returns -1 on error, otherwise 1
*/
int jsonpp_set(json_t *json, const char *pointer, json_t *value);

/*
Stores the JSON value in the JSON document unter the JSON (tilde + URL) pointer.
Does not increase the reference of value.

@param json The JSON document used for the lookup
@param pointer The JSON pointer (tilde + URL encoding)
@param pointer The JSON document to be stored
@returns Returns -1 on error, otherwise 1
*/
int jsonpp_set_new(json_t *json, const char *pointer, json_t *value);

/*
Removes the value stored unter the JSON (tilde + URL) pointer in the JSON document.

@param json The JSON document used for the lookup
@param pointer The JSON pointer (tilde + URL encoding)
@returns Returns -1 on error, otherwise 1
*/
int jsonpp_delete(json_t *json, const char *pointer);

#ifdef __cplusplus
}
#endif

#endif /*JSONPP_H*/