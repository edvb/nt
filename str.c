/* See LICENSE file for copyright and license details. */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

/* remove tailing or leading white space from s */
char *
strtrim(char *s)
{
	char *end;

	/* trim leading space */
	while (isspace(*s)) s++;

	if (*s == 0) /* all spaces? */
		return s;

	/* trim trailing space */
	end = s + strlen(s) - 1;
	while (end > s && isspace(*end)) end--;

	/* write new null terminator */
	*(end+1) = 0;

	return s;
}

/* return if str is in the list */
int
strinlist(char *str, char **list, int listc)
{
	if (!str || !list) return 0;
	int i;
	for (i = 0; i < listc; i++)
		if (list[i] && strcmp(str, list[i]) == 0)
			return 1;
	return 0;
}
