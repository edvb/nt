/* See LICENSE file for copyright and license details. */
#include <libgen.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "arg.h"
#include "str.h"
#include "util.h"

/* macros */
#define MAX_SUB 2048

/* typedefs */
typedef struct Note {
	char *str;
	struct Note *next;
	struct Note *prev;
} Note;

/* functions */
void linkadd(char *str);
int confirm(const char *msg, ...);
char *get_cat(char *str);

void nt_del(void);
void nt_del_all(void);
void nt_edit(void);
void nt_list_all(void);
void nt_list_n(void);
void nt_new(void);

void setup(void);
void cleanup(void);
void usage(void);

/* variables */
char *argv0;

char *sub;
Note *head = NULL;
Note *tail = NULL;

void (*mode)(void) = nt_new;
int neednt = 1;
int lsnum;

#include "config.h"

/* add str to Note linked list */
void
linkadd(char *str)
{
	if (!head->str && !head->next && !head->prev) {
		head = ecalloc(1, sizeof(Note));
		head->str  = estrdup(str);
		head->next = NULL;
		head->prev = NULL;
		tail = head;
		return;
	}

	/* go through notes until we reach the last one */
	Note *cur = head;
	while (cur->next)
		cur = cur->next;

	/* allocate note */
	cur->next = ecalloc(1, sizeof(Note));
	cur->next->prev = cur;
	cur = cur->next;
	cur->str = estrdup(str);
	cur->next = NULL;
	tail = cur;
}

/* prompt user if no -y option to confirm action */
int
confirm(const char *msg, ...)
{
	char input = 'n';
	va_list ap;

	if (yes) return 1;

	va_start(ap, msg);
	printf("%s: ", argv0);
	vprintf(msg, ap);
	printf("? [y/N] ");
	va_end(ap);
	scanf("%c", &input);

	return (input == 'y' || input == 'Y') ? 1 : 0;
}

/* return just category from str */
char *
get_cat(char *str)
{
	char *cat = estrdup(str);
	char delim[2] = { catdelim, 0 };

	if (strchr(str, catdelim))
		strtok(cat, delim);
	else
		cat = NULL;

	return cat;
}

/* delete oldest matching note from notes */
void
nt_del(void)
{
	if (!sub) usage();
	Note *cur = head;

	for (; cur; cur = cur->next) {
		if (!strcmp(cur->str, sub)) {
			if (confirm("delete note '%s'", cur->str)) {
				if (!cur->prev) { /* beginning */
					head = cur->next;
				} else if (!cur->next) { /* end */
					cur->prev->next = NULL;
				} else { /* middle */
					cur->prev->next = cur->next;
					cur->next->prev = cur->prev;
				}
				free(cur);
			}
			return;
		}
	}

	die("%s: delete: '%s' not found", argv0, sub);
}

/* delete all notes from fname */
void
nt_del_all(void)
{
	Note *cur = head;
	if (confirm("delete all notes in '%s'", fname)) {
		for (; cur; cur = cur->next)
			cur->str = NULL;
		remove(fname);
	}
}

/* edit given note sub from stdin */
void
nt_edit(void)
{
	if (!sub) usage();
	Note *cur = head;

	for (; cur; cur = cur->next)
		if (!strcmp(cur->str, sub)) {
			fgets(cur->str, MAX_SUB, stdin);
			strtrim(cur->str);
			return;
		}

	die("%s: edit: '%s' not found", argv0, sub);
}

/* print out entire file */
void
nt_list_all(void)
{
	Note *cur = head;
	for (; cur; cur = cur->next)
		if (cur->str)
			printf("%s\n", cur->str);
}

/* display n most recent subjects in file */
void
nt_list_n(void)
{
	int i;
	Note *cur = tail;
	for (i = 0; i < lsnum && cur; cur = cur->prev, i++)
		if (cur->str)
			printf("%s\n", cur->str);
}

/* list all categories or all notes with a given category */
void
nt_cat(void)
{
	Note *cur = head;
	char **cat = ecalloc(1, sizeof(char*));
	int catc = 0, i;

	for (; cur; cur = cur->next, catc++) {
		cat = erealloc(cat, (catc+2) * sizeof(char*));
		if (!(cat[catc] = get_cat(cur->str)))
			continue;
		if (!strcmp(sub, "") && !strinlist(cat[catc], cat, catc))
			printf("%s\n", cat[catc]);
		else if (!strcmp(sub, cat[catc]))
			printf("%s\n", cur->str);
	}

	for (i = 0; i < catc; i++)
		free(cat[i]);
	free(cat);
}

/* create a new note */
void
nt_new(void)
{
	if (!sub) usage();
	linkadd(sub);
}

/* populate notes list, allocate sub */
void
setup(void)
{
	char buf[MAX_SUB];
	FILE *fp;

	head = ecalloc(1, sizeof(Note));
	tail = head;

	/* load file if it exists */
	if (access(fname, F_OK) != -1) {
		if (!(fp = fopen(fname, "r")))
			die("%s: %s:", argv0, fname);
		while (fscanf(fp, "%2048[^\n]\n", buf) != EOF)
			linkadd(buf);
		fclose(fp);
	}

	sub = ecalloc(MAX_SUB, sizeof(char));

}

/* write list to file, free memory */
void
cleanup(void)
{
	FILE *fp;
	Note *cur = head;

	/* write note list to file */
	if (!(fp = fopen(fname, "w")))
		die("%s: %s:", argv0, fname);
	for (; cur; cur = cur->next)
		if (cur->str)
			fprintf(fp, "%s\n", cur->str);
	fclose(fp);

	for (cur = head; cur; cur = cur->next)
		free(cur->str);
	free(head);
	free(sub);
}

void
usage(void)
{
	die("usage: %s [-Dilyv] [-f FILE] [-e NOTE] [-d NOTE]\n"
	    "          [-c [CATEGORY]] [-n NUM | -NUM] [NOTE ...]", argv0);
}

int
main(int argc, char *argv[])
{
	ARGBEGIN {
	case 'd':
		mode = nt_del;
		break;
	case 'D':
		mode = nt_del_all;
		neednt = 0;
		break;
	case 'e':
		mode = nt_edit;
		break;
	case 'f':
		fname = EARGF(usage());
		break;
	case 'i':
		yes = 0;
		break;
	case 'l':
		mode = nt_list_all;
		neednt = 0;
		break;
	case 'n':
		mode = nt_list_n;
		neednt = 0;
		lsnum = atoi(EARGF(usage()));
		break;
	ARGNUM:
		mode = nt_list_n;
		neednt = 0;
		lsnum = ARGNUMF();
		break;
	case 'c':
		mode = nt_cat;
		neednt = 0;
		break;
	case 'y':
		yes = 1;
		break;
	case 'v':
		printf("%s v%s (c) 2017 Ed van Bruggen\n", argv0, VERSION);
		return 0;
	default:
		usage();
	} ARGEND;

	setup();

	if (argc <= 0 && neednt)
		fgets(sub, MAX_SUB, stdin);
	else if (argc > 0) {
		while (*argv) {
			strcat(sub, *argv);
			if (*(++argv))
				strcat(sub, " ");
		}
	}
	strtrim(sub); /* TODO replace by stopping newline from stdin */

	mode();

	cleanup();

	return 0;
}
