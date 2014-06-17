#ifndef JSONP_OPTIONS_H
#define JSONP_OPTIONS_H

#include <jansson.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
Parses the options in args by what is defined in opts.
Parses json arguments as native types.
*/
json_t *jsonp_options(json_t *args, json_t *opts);

#ifdef __cplusplus
}
#endif

#endif /*JSONP_OPTIONS_H*/