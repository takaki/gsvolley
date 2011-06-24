/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.cc
 * Copyright (C) TANIGUCHI Takaki 2011 <takaki@asis.media-as.org>
 * 
 * cppgsvolley is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * cppgsvolley is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtkmm.h>
#include <iostream>



/* For testing propose use the local (not installed) ui file */
/* #define UI_FILE PACKAGE_DATA_DIR"/cppgsvolley/ui/cppgsvolley.ui" */
#define UI_FILE "src/cppgsvolley.ui"
   
int
main (int argc, char *argv[])
{
	Gtk::Main kit(argc, argv);
	
	Gtk::Window window;
	window.set_size_request (100,400);
	window.set_resizable (FALSE);
	Gtk::Main::run(window);
	return EXIT_SUCCESS;
}
