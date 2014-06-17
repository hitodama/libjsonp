#ifndef JSONP_GET_OPTIONS_H
#define JSONP_GET_OPTIONS_H

#include <jansson.h>

#ifdef __cplusplus
extern "C" {
#endif

json_t *jsonp_options(json_t *args, json_t *opts);

#ifdef __cplusplus
}
#endif

#endif /*JSONP_GET_OPTIONS_H*/