# nt - simple note taker

## SYNOPSIS

`nt` [**-Dilvy**] [**-f** *FILE*] [**-e** *NOTE*] [**-d** *NOTE*] [**-s** *SEARCH*] [**-t** [*TAG*]] [**-n** *NUM* | **-NUM**] [*NOTE* ...]

## DESCRIPTION

Simple note taker for the command line. To take a note simply run the command
with the new note as arguments or from stdin. The note will be saved to a plain
text file with each line being a note. The file is saved to the file either
specified with **-f** or in the default one from `config.h`. Notes can have tags
by prefixing the note with a tag followed by a colon. For example a note
`chores: take out trash` will have the tag `chores`.

## OPTIONS

**-d** *NOTE*
	Delete *NOTE* from notes

**-D**
	Delete all notes from *FILE*

**-e** *NOTE*
	Edit *NOTE* with new text given through stdin

**-f** *FILE*
	Load *FILE* instead of default one

**-i**
	Force prompt to confirm action

**-l**
	List notes

**-n** *NUM*, **-NUM**
	List last *NUM* notes

**-s** *SEARCH*
	Search for pattern *SEARCH* in notes

**-t** [*TAG*]
	Search for *TAG* in notes, list all tags if no *TAG* is given

**-v**
	Print version info and exit

**-y**
	Auto reply yes to confirmation prompts

## USAGE

Add simple note and list it:

	$ nt clean dishes
	$ nt -l
	clean dishes

Create note in specified file:

	$ nt -f list take out trash
	$ nt -f list -l
	take out trash

Add more notes by running command again:

	$ nt wash car
	$ nt -l
	clean dishes
	wash car
	$ nt buy pie
	$ nt -l
	clean dishes
	wash car
	buy pie

List just the *NUM* most recent notes:

	$ nt -1
	buy pie
	$ nt -n 1
	buy pie
	$ nt -2
	buy pie
	wash car
	$ nt -n 2
	buy pie
	wash car

Search notes with a specified term:

	$ nt -s pie
	buy pie
	$ nt make pie
	$ nt -s pie
	buy pie
	make pie

Remove given note:

	$ nt -yd buy pie
	$ nt -l
	clean dishes
	wash car
	make pie

Edit note with new text given from stdin:

	$ echo eat pie | nt -e make pie
	$ nt -l
	clean dishes
	wash car
	eat pie

Remove all notes:

	$ nt -yD
	$ nt -l
	$ nt -f list -yD
	$ nt -f list -l

## AUTHOR

Ed van Bruggen <edvb54@gmail.com>

## SEE ALSO

View source code at: <https://gitlab.com/edvb/nt>

## LICENSE

zlib License
