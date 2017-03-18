# nt - simple note taker

## SYNOPSIS

`nt` [-lv] [-f *FILE*] [-e *NOTE*] [-d *NOTE*] [-s *SEARCH*] [-n *NUM* | -*NUM*] [*NOTE* ...]

## DESCRIPTION

Simple note taker for the command line.

## OPTIONS

-v
	Print version info and exit

-f *FILE*
	Load *FILE* instead of default one

-l
	List notes

-n *NUM*
	List last *NUM* notes

-s *SEARCH*
	Search for *SEARCH* in notes

-d *NOTE*
	Delete *NOTE* from notes

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

	$ nt -d buy pie
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

## AUTHOR

Ed van Bruggen <edvb54@gmail.com>

## SEE ALSO

View source code at: <https://gitlab.com/edvb/nt>

## LICENSE

zlib License
