/* See LICENSE file for copyright and license details. */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "arg.h"
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
int confirm(const char *msg, ...);

void nt_add(char *str);
void nt_edit(void);
void nt_del(void);
void nt_search(void);
void nt_list_all(void);
void nt_list_n(int n);

void run(void);
void setup(void);
void cleanup(void);
void usage(void);

/* variables */
char *argv0;

char *sub;
Note *head = NULL;
Note *tail = NULL;

char mode = 0;
int lsnum;

#include "config.h"

int
confirm(const char *msg, ...)
{
	if (yes) return 1;

	char input = 'n';
	va_list ap;

	va_start(ap, msg);
	printf("%s: ", argv0);
	vprintf(msg, ap);
	printf("? [y/N] ");
	va_end(ap);
	scanf("%c", &input);

	return (input == 'y' || input == 'Y') ? 1 : 0;
}

/* create a new note */
void
nt_add(char *str)
{
	if (!str) usage();

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

/* delete oldest matching note from notes */
void
nt_del(void)
{
	if (!sub) usage();
	Note *cur = head;

	for (; cur; cur = cur->next) {
		if (strcmp(cur->str, sub) == 0) {
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

void
nt_edit()
{
	if (!sub) usage();
	Note *cur = head;

	for (; cur; cur = cur->next)
		if (strcmp(cur->str, sub) == 0) {
			fgets(cur->str, MAX_SUB, stdin);
			strtrim(cur->str);
			return;
		}

	die("%s: edit: '%s' not found", argv0, sub);
}

/* search notes for given note */
void
nt_search()
{
	if (!sub) usage();
	int found = 0;
	Note *cur = head;

	for (; cur; cur = cur->next)
		if (strstr(cur->str, sub)) {
			printf("%s\n", cur->str);
			found = 1;
		}
	if (!found)
		die("%s: search: '%s' not found", argv0, sub);
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
nt_list_n(int n)
{
	int i;
	Note *cur = tail;
	for (i = 0; i < n && cur; cur = cur->prev, i++)
		if (cur->str)
			printf("%s\n", cur->str);
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
		break;
	case 'n':
		nt_list_n(lsnum);
		break;
	case 's':
		nt_search();
		break;
	default:
		nt_add(sub);
	}
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
		fp = fopen(fname, "r");
		while (fscanf(fp, "%2048[^\n]\n", buf) != EOF)
			nt_add(buf);
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
	fp = fopen(fname, "w");
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
	die("usage: %s [-lvy] [-f FILE] [-e NOTE] [-d NOTE]\n"
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
	case 'y':
		yes = 1;
		break;
	default:
		usage();
	} ARGEND;

	setup();

	if (argc <= 0 && (!mode || mode == 'd' || mode == 's'))
		fgets(sub, MAX_SUB, stdin);
	else if (argc > 0)
		sub = strconcat(argv, argc);
	strtrim(sub);

	run();

	cleanup();

	return 0;

}
