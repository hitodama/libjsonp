#ifndef JSONP_CONFIG_H
#define JSONP_CONFIG_H

/*
Definition of SIZE_MAX without stdint.h / limits.h
*/
#define JSONP_SIZE_MAX (size_t)-1

/*
The path seperator, so it could be easily altered.
*/
#define JSONP_PATH_SEPERATOR '/'

/*
Maximum path size, for security, catch overflows.
*/
#define JSONP_PATH_SIZE_MAX 4096

#endif /*JSONP_CONFIG_H*/