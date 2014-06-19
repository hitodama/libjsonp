#ifndef JSONP_HELPER_H
#define JSONP_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

/*
Counts the apperance of c in m, with m having the size n.
*/
size_t jsonp_memcnt(void *mem, int c, size_t n);

/*
Shifts all c to the back of m, with m having the size n.
*/
void jsonp_memsh(void *mem, int c, size_t n);

/*
Replaces the string needle with the string replace in the string haystack. Rest are size parameters.

@note Naive impl O(~4n), should be O(n)
@see jsonp_memmemmemi
*/
size_t jsonp_strnirep(char *haystack, size_t hn, const char *needle, size_t nn, const char *replace, size_t rn);


/*
Converts the string s into the size_t value. Returns the sign of s (+1/-1) or 0 on error.
*/
int jsonp_strtozu(size_t *value, const char *s);


/*
Returns the approximate count of digits and sign in a number of size.
*/
size_t jsonp_digits(size_t size);

/*
Returns the string length or n, if the string length is smaller than n.
Since strnlen is not a c99 function, this is a portable MIT implementation.
*/
size_t jsonp_strnlen(const char *s, size_t n);

/*
Returns a duplicate of the string s. If s is larger than n the string is cropped.
Since strndup is not a c99 function, this is a portable MIT implementation.
*/
char *jsonp_strndup(const char *s, size_t n);

/*
Returns a duplicate of the string s.
Since strndup is not a c99 function, this is a portable MIT implementation.
*/
char *jsonp_strdup(const char *s);

/*
Returns 1 if s is an integer (of any size), 0 otherwise.
*/
int jsonp_isint(const char *s);

#ifdef __cplusplus
}
#endif

#endif /*JSONP_HELPER_H*/