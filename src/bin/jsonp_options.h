#ifndef JSONP_OPTIONS_H
#define JSONP_OPTIONS_H

#include <jansson.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
Parses the options array in args and extracts what is defined in opts.
Parses json arguments as native types.

@param args JSON representation of argv as array.
@param opts Options in the form of three string arrays (arguments, options, switches).
@returns Returns an object consisting of an arguments array and an options and switches object.
*/
json_t *jsonp_options(json_t *args, json_t *opts, char prefix);

#ifdef __cplusplus
}
#endif

#endif /*JSONP_OPTIONS_H*/