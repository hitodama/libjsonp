#ifndef JSONP_HELPER_H
#define JSONP_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

size_t jsonp_memcnt(void *mem, int c, size_t n);
void jsonp_memsh(void *mem, int c, size_t n);
/*Naive impl O(m*n), fix at some point*/
/*void memmemmemi(void *mem, size_t memn, const void *nee, size_t neen, const void *rep, size_t repn);*/
/*Naive impl O(~4n), should be O(n)*/
size_t jsonp_strnirep(char *haystack, size_t hn, const char *needle, size_t nn, const char *replace, size_t rn);
size_t jsonp_strirep(char *haystack, const char *needle, const char *replace);
/*int jsonp_strtozu(size_t *value, const char *s, char **e, int base);*/
int jsonp_strtozu(size_t *value, const char *s);
size_t jsonp_zudigits(size_t number);
size_t jsonp_strnlen(const char *s, size_t n);
char *jsonp_strndup(const char *s, size_t n);
int jsonp_isnint(const char *s, size_t n);
int jsonp_isint(const char *s);

#ifdef __cplusplus
}
#endif

#endif /*JSONP_HELPER_H*/