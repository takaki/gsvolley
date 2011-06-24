## Process this file with automake to produce Makefile.in

## Created by Anjuta

uidir = $(datadir)/cppgsvolley/ui
ui_DATA = cppgsvolley.ui

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(CPPGSVOLLEY_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = cppgsvolley

cppgsvolley_SOURCES = \
	main.cc

cppgsvolley_LDFLAGS = 

cppgsvolley_LDADD = $(CPPGSVOLLEY_LIBS)

EXTRA_DIST = $(ui_DATA)

# Remove ui directory on uninstall
uninstall-local:
	-rm -r $(uidir)
	-rm -r $(datadir)/cppgsvolley
