## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src 

gsvolleydocdir = ${prefix}/doc/gsvolley
gsvolleydoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS
	

EXTRA_DIST = $(gsvolleydoc_DATA)


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(gsvolleydocdir)
