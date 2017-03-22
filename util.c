/* See LICENSE file for copyright and license details. */
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

void *
ecalloc(size_t nmemb, size_t size)
{
	void *p;

	if (!(p = calloc(nmemb, size)))
		die("calloc: out of memory");

	return p;
}

void *
emalloc(size_t size)
{
	void *p;

	if (!(p = malloc(size)))
		die("malloc: out of memory");

	return p;
}

void *
erealloc(void *p, size_t size)
{
	if (!(p = realloc(p, size)))
		die("realloc: out of memory");

	return p;
}

char *
estrdup(char *s)
{
	if (!(s = strdup(s)))
		die("strdup: out of memory");

	return s;
}

char *
strconcat(char **s, int c)
{
	if (!s) die("strconcat: given null pointer");

	int len, i;
	char *ret;

	for (i = 0; i < c; i++)
		len += strlen(s[i]) + 1;
	ret = ecalloc(len, sizeof(char));

	strcpy(ret, s[0]);
	strcat(ret, " ");
	for (i = 1; i < c; i++) {
		strcat(ret, s[i]);
		strcat(ret, " ");
	}

	return ret;
}

/* split string s into muiltple strings by a_delim */
char **
strsplit(const char *s, const char a_delim, int *size)
{
	if (!s) die("strsplit: given null pointer");

	char **ret = 0;
	size_t count = 0;
	char *last_delim = 0;
	char delim[2] = { a_delim, 0 }; /* converet a_delim into string for strtok */
	char *a_str = ecalloc(strlen(s)+1, sizeof(char));
	strcpy(a_str, s);

	/* count number of elements that will be extracted. */
	for (char *tmp = a_str; *tmp; tmp++)
		if (a_delim == *tmp) {
			count++;
			last_delim = tmp;
		}

	/* add space for trailing token. */
	count += last_delim < (a_str + strlen(a_str) - 1);

	/* add space for terminating null string so caller
	 * knows where the list of returned strings ends. */
	count++;

	ret = ecalloc(count, sizeof(char*));

	if (ret) {
		size_t idx  = 0;
		char *token = strtok(a_str, delim);

		while (token) {
			assert(idx < count);
			*(ret + idx++) = estrdup(token);
			token = strtok(0, delim);
		}
		assert(idx == count - 1);
		*(ret + idx) = 0;
	}

	free(a_str);
	*size = count - 1;
	return ret;
}

/* remove tailing or leading white space from s */
char *
strtrim(char *s)
{
	char *end;

	/* trim leading space */
	while (isspace((unsigned char)*s)) s++;

	if (*s == 0) /* all spaces? */
		return s;

	/* trim trailing space */
	end = s + strlen(s) - 1;
	while (end > s && isspace((unsigned char)*end)) end--;

	/* write new null terminator */
	*(end+1) = 0;

	return s;
}

void
die(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt)-1] == ':') {
		fputc(' ', stderr);
		perror(NULL);
	} else {
		fputc('\n', stderr);
	}

	exit(1);
}
