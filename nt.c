/* See LICENSE file for copyright and license details. */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arg.h"
#include "util.h"

/* macros */
#define MAX_SUB 2048

/* typedefs */

/* functions */
char *trimwhitespace(char *str);

void nt_new(void);
void nt_edit(void);
void nt_del(void);
void nt_search(void);
void nt_list_all(void);
void nt_list_n(int n);

void run(void);
void setup(void);
void cleanup(void);
void usage(void);

/* varibles */
char *argv0;

FILE *fp;
char *sub;
char **notes;
int notec;

char mode = 0;
int lsnum;

#include "config.h"

/* remove tailing or leading whitespace from str */
char *
trimwhitespace(char *str)
{
	char *end;

	/* trim leading space */
	while(isspace((unsigned char)*str)) str++;

	if(*str == 0)  /* all spaces? */
		return str;

	/* trim trailing space */
	end = str + strlen(str) - 1;
	while(end > str && isspace((unsigned char)*end)) end--;

	/* write new null terminator */
	*(end+1) = 0;

	return str;
}

/* create a new note */
void
nt_new(void)
{
	if (sub == NULL) usage();

	fprintf(fp, "%s\n", sub);
}

/* delete oldest matching note from notes */
void
nt_del(void)
{
	if (sub == NULL) usage();

	int i, found = 0;
	FILE *tmpfp;
	char *tmpfpext = "part";
	char *tmpfpname = ecalloc(strlen(fname)+strlen(tmpfpext)+1, sizeof(char));

	strcpy(tmpfpname, fname);
	strcat(tmpfpname, ".");
	strcat(tmpfpname, tmpfpext);
	tmpfp = fopen(tmpfpname, "w");

	for (i = 0; i < notec; i++)
		if (strcmp(notes[i], sub) != 0 || found)
			fprintf(tmpfp, "%s\n", notes[i]);
		else
			found = 1;
	if (!found)
		die("%s: delete: '%s' not found", argv0, sub);

	fclose(tmpfp);
	remove(fname);
	rename(tmpfpname, fname);
}

void
nt_edit()
{
	if (sub == NULL) usage();
	int i, found = 0;

	for (i = 0; i < notec; i++)
		if (strcmp(notes[i], sub) == 0 && !found) {
			fgets(notes[i], MAX_SUB, stdin);
			trimwhitespace(notes[i]);
			found = 1;
		}
	if (!found)
		die("%s: edit: '%s' not found", argv0, sub);

	fclose(fp);
	fp = fopen(fname, "w");
	for (i = 0; i < notec; i++)
		fprintf(fp, "%s\n", notes[i]);
	rewind(fp);
}

/* search notes for given note */
void
nt_search()
{
	if (sub == NULL) usage();

	int i, found = 0;
	for (i = 0; i < notec; i++)
		if (strstr(notes[i], sub)) {
			printf("%s\n", notes[i]);
			found = 1;
		}
	if (!found)
		die("%s: search: '%s' not found", argv0, sub);
}

/* print out entire file */
void
nt_list_all(void)
{
	int i;
	for (i = 0; i < notec; i++)
		printf("%s\n", notes[i]);
}

/* diplay n most recent subjects in file */
void
nt_list_n(int n)
{
	int i = notec;
	n = n > i ? -1 : i-n-1;
	for (i--; i > n; i--)
		printf("%s\n", notes[i]);
}

/* handle options and create notes */
void
run(void)
{
	switch (mode) {
	case 'd':
		nt_del();
		break;
	case 'e':
		nt_edit();
		break;
	case 'l':
		nt_list_all();
		exit(0);
	case 'n':
		nt_list_n(lsnum);
		exit(0);
	case 's':
		nt_search();
		break;
	default:
		nt_new();
	}
}

/* load fname, set notec, populate notes, allocate sub */
void
setup(void)
{
	int c, i = 0, last = 0;

	/* open file */
	fp = fopen(fname, "ab+");

	/* get number of notec */
	while ((c = fgetc(fp)) != EOF) {
		if (c == '\n' && last != '\n')
			++notec;
		last = c;
	}
	rewind(fp);

	/* copy file fp to notes */
	notes = ecalloc(notec+1, sizeof(char*));
	do
		notes[i] = ecalloc(MAX_SUB, sizeof(char));
	while (fscanf(fp, "%2048[^\n]\n", notes[i++]) != EOF);
	rewind(fp);

	sub = ecalloc(MAX_SUB, sizeof(char));
}

/* close fname, free memory */
void
cleanup(void)
{
	fclose(fp);
	for (int j = 0; j < notec+1; j++)
		free(notes[j]);
	free(notes);
	free(sub);
}

void
usage(void)
{
	die("usage: %s [-lv] [-f FILE] [-e NOTE] [-d NOTE]\n"
		"          [-s SEARCH] [-n NUM | -NUM] [NOTE ...]", argv0);
}

int
main(int argc, char *argv[])
{
	ARGBEGIN {
	case 'd':
		mode = 'd';
		break;
	case 'e':
		mode = 'e';
		break;
	case 'f':
		fname = EARGF(usage());
		break;
	case 'l':
		mode = 'l';
		break;
	case 'n':
		mode = 'n';
		lsnum = atoi(EARGF(usage()));
		break;
	ARGNUM:
		mode = 'n';
		lsnum = ARGNUMF();
		break;
	case 's':
		mode = 's';
		break;
	case 'v': 
		printf("%s v%s\n", argv0, VERSION);
		return 0;
	default:
		usage();
	} ARGEND;

	setup();

	if (argc <= 0 && (!mode || mode == 'd' || mode == 's'))
		fgets(sub, MAX_SUB, stdin);
	else if (argc > 0)
		sub = strconcat(argv, argc);
	trimwhitespace(sub);

	run();

	cleanup();

	return 0;

}
