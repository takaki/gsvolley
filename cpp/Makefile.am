## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src 

cppgsvolleydocdir = ${prefix}/doc/cppgsvolley
cppgsvolleydoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(cppgsvolleydoc_DATA)


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(cppgsvolleydocdir)
