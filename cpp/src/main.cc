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

// #include <cairomm/cairomm.h>
#include <gtkmm.h>

#include <memory>
#include <gdk/gdkkeysyms.h>

#include "defs.h"
#include "gamewin.h"
#include "gameinfo.h"


int
main (int argc, char *argv[])
{
	Gtk::Main kit(argc, argv);
	
	GameWin window;
	Gtk::Main::run(window);
	return EXIT_SUCCESS;
}
