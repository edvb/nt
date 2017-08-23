# nt - simple note taker
# See LICENSE file for copyright and license details.

include config.mk

EXE = nt
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: options $(EXE)

options:
	@echo $(EXE) build options:
	@echo "CFLAGS  = $(CFLAGS)"
	@echo "LDFLAGS = $(LDFLAGS)"

.o:
	@echo $(LD) $@
	@$(LD) -o $@ $< $(LDFLAGS)

.c.o:
	@echo $(CC) $<
	@$(CC) -c -o $@ $< $(CFLAGS)

${OBJ}: config.h config.mk

config.h:
	@echo creating $@ from config.def.h
	@cp config.def.h $@

$(EXE): $(OBJ)
	@echo $(CC) -o $@
	@$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	@echo -n cleaning ...
	@rm -f $(OBJ) $(EXE)
	@echo \ done

install: all
	@echo -n installing $(EXE) to $(DESTDIR)$(PREFIX)/bin ...
	@mkdir -p $(DESTDIR)$(PREFIX)/bin
	@cp -f $(EXE) $(DESTDIR)$(PREFIX)/bin
	@chmod 755 $(DESTDIR)$(PREFIX)/bin/$(EXE)
	@echo \ done
	@echo -n installing dmenu_$(EXE) to $(DESTDIR)$(PREFIX)/bin ...
	@cp -f dmenu_$(EXE) $(DESTDIR)$(PREFIX)/bin
	@echo \ done
	@echo -n installing manual page to $(DESTDIR)$(MANPREFIX)/man1 ...
	@mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	@sed "s/VERSION/$(VERSION)/g" < $(EXE).1 > $(DESTDIR)$(MANPREFIX)/man1/$(EXE).1
	@chmod 644 $(DESTDIR)$(MANPREFIX)/man1/$(EXE).1
	@echo \ done

uninstall:
	@echo -n removing $(EXE) from $(DESTDIR)$(PREFIX)/bin ...
	@rm -f $(DESTDIR)$(PREFIX)/bin/$(EXE)
	@echo \ done
	@echo -n removing dmenu_$(EXE) file from $(DESTDIR)$(PREFIX)/bin ...
	@rm -f $(DESTDIR)$(PREFIX)/bin/dmenu_$(EXE)
	@echo \ done
	@echo -n removing manual page from $(DESTDIR)$(MANPREFIX)/man1 ...
	@rm -f $(DESTDIR)$(MANPREFIX)/man1/$(EXE).1
	@echo \ done

man:
	@echo -n updating man page $(EXE).1 ...
	@cat README.md | sed "s/# $(EXE)/# $(EXE) 1\n\n##NAME\n\n$(EXE) /" | \
		md2man-roff | sed "s/\\[la\]/\</" | sed "s/\\[ra\]/\>/" > $(EXE).1
	@echo \ done

test: $(EXE)
	@rm -f todo list
	@clitest --prefix tab README.md

.PHONY: all options clean install uninstall man test
