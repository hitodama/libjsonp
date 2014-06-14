#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libjsonp_helper.h"
#include "libjsonp_config.h"

size_t jsonp_memcnt(void *m, int c, size_t n)
{
	size_t i;
	unsigned char *mem = m;
	size_t r = 0;

	for(i = 0; i < n; ++i)
		if(mem[i] == c)
			++r;

	return r;
}

void jsonp_memsh(void *m, int c, size_t n)
{
	size_t s;
	size_t i;
	char t;
	unsigned char *mem = m;

	for(i = 0, s = 0; i + s < n; ++i)
	{
		if(mem[i] == c)
			while(i + s < n && mem[i + s] == c)
				++s;
		t = mem[i + s];
		mem[i + s] = c;
		mem[i] = t;
	}
}

void jsonp_memmemmemi(void *m, size_t memn, const void *n, size_t neen, const void *r, size_t repn)
{
	size_t i;
	size_t j;
	unsigned char *mem = m;
	const unsigned char *nee = n;
	const unsigned char *rep = r;
	
	if(repn > neen)
		return;

	for(i = 0; i < memn; ++i)
	{
		for(j = 0; i + j < memn && j < neen && mem[i + j] == nee[j]; ++j);
		if(j == neen)
			for(j= 0; j < neen; ++j)
			{
				if(j < repn)
					mem[i + j] = rep[j];
				else
					mem[i + j] = 0x00;
			}
	}
}

size_t jsonp_strnirep(char *haystack, size_t hn, const char *needle, size_t nn, const char *replace, size_t rn)
{
	jsonp_memmemmemi(haystack, hn, needle, nn, replace, rn);
	jsonp_memsh(haystack, 0, hn);
	return hn - jsonp_memcnt(haystack, 0, hn);
}

int jsonp_strtozu(size_t *value, const char *s) /*, char **e, int base)*/
{
	int sign = 1;
	if(value == NULL)
		return 0;
	if(strncmp(s, "-", 1) == 0)
	{
		s++;
		sign = -1;
	}
	/**value = (size_t)strtoull(s, e, base); not c89*/
	sscanf(s, "%zu", value);
	return sign;
}

size_t jsonp_zudigits(size_t number)
{
	size_t c;
	if(number == 0)
		return 1;
	for(c = 0; number != 0; number /= 10, ++c);
	return c;
}

size_t jsonp_strnlen(const char *s, size_t n)
{
	if(s == NULL)
		return 0;
	size_t i;
	for(i = 0; i < n && *s != '\0'; ++i, ++s);
	return i;
}

char *jsonp_strndup(const char *s, size_t n)
{
	if(s == NULL)
		return NULL;
	size_t i = jsonp_strnlen(s, n);
	char *r = malloc(sizeof(char) * (i + 1));
	//memset(r, 0, i);
	memcpy(r, s, i);
	r[i] = '\0';
	return r;
}

int jsonp_isnint(const char *s, size_t n)
{
	size_t i = 0;

	if(*s != '\0' && (*s == '-' || *s == '+'))
		++s;

	for(i = 0; *s != 0 && i < n; ++i, ++s)
		//if(!isdigit(*s))
		if(*s < '0' || *s > '9')
			return 0;
	return 1;
}

int jsonp_isint(const char *s)
{
	return jsonp_isnint(s, JSONP_SIZE_MAX);
}