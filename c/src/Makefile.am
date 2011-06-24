## Process this file with automake to produce Makefile.in

## Created by Anjuta

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(GSVOLLEY_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = gsvolley

gsvolley_SOURCES = \
	main.c    \
	ball.c    \
	ball.h    \
	slime.h    \
	slime.c    \
	gamewin.c    \
	gamewin.h    \
	gameinfo.c    \
	gameinfo.h    \
	defs.h

gsvolley_LDFLAGS = \
	-Wl,--export-dynamic

gsvolley_LDADD = $(GSVOLLEY_LIBS)

EXTRA_DIST = $(ui_DATA)

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(uidir)
	-rm -r $(datadir)/gsvolley
