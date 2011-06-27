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
#include <iostream>
#include <memory>
#include <gdk/gdkkeysyms.h>

#include "defs.h"
#include "gamewin.h"
#include "gameinfo.h"


class GSVolley: public Gtk::Window 
{
	public:
		GSVolley();
	protected:
		bool cb_delete_event(GdkEventAny*);
		bool cb_expose_event(const Cairo::RefPtr< Cairo::Context >&);
		bool cb_key_press(GdkEventKey *);
		bool event_loop();
	private:
		GameInfo gameinfo;

};


GSVolley::GSVolley()
{
	set_size_request (WIN_WIDTH, WIN_HEIGHT);
	set_resizable (false);
	set_app_paintable (true);

	signal_draw().connect(sigc::mem_fun(*this, &GSVolley::cb_expose_event ));
	signal_key_press_event().connect(sigc::mem_fun(*this, &GSVolley::cb_key_press),false);
	signal_delete_event().connect(sigc::mem_fun(*this, &GSVolley::cb_delete_event));

	Glib::signal_timeout ().connect(sigc::mem_fun(*this, &GSVolley::event_loop),10);
	gameinfo.init();

}


bool
GSVolley::cb_expose_event(const Cairo::RefPtr< Cairo::Context >& cr)
{
	GameWin gw(cr);
	gw.draw_back();
	gw.draw_slime(gameinfo.get_slime_blue());
	gw.draw_slime(gameinfo.get_slime_red());
	gw.draw_penalty(gameinfo.get_penalty_y());
	gw.draw_ball(gameinfo.get_ball());
	gw.draw_score(gameinfo.get_score_blue(), gameinfo.get_ball_count(),
	                   gameinfo.get_score_red());

	switch(gameinfo.get_state()) {
		case GS_INIT: 
			break;
		case GS_PLAY: 
			break;
		case GS_GOT_BY_BLUE:
			gw.show_text_centering ("DROPPED BY RED", 40);		
			break;	
		case GS_GOT_BY_RED:
			gw.show_text_centering ("DROPPED BY BLUE", 40);		
			break;	
		case GS_SERVICE_BY_BLUE:
			break;
		case GS_SERVICE_BY_RED:
			break;
		case GS_WON_BY_BLUE:
			gw.show_text_centering ("PRESS R TO PLAY AGAIN", 80);		
			gw.show_text_centering ("WON BY BLUE", 60);		
			break;
		case GS_WON_BY_RED:
			gw.show_text_centering ("PRESS R TO PLAY AGAIN", 80);		
			gw.show_text_centering ("WON BY RED", 60);		
			break;
		default:
			std::cerr << "ERROR!! Unknown Game State" << std::endl;
			break;
	}
	return false;
}

bool
GSVolley::cb_key_press (GdkEventKey *ev)
{
	switch(ev->keyval) {
		case GDK_KEY_a:
			gameinfo.get_slime_blue().set_vx(-1);
			break;
		case GDK_KEY_s:
			gameinfo.get_slime_blue().set_vx(1);
			break;
		case GDK_KEY_Left:
			gameinfo.get_slime_red().set_vx(-1);
			break;
		case GDK_KEY_Right:
			gameinfo.get_slime_red().set_vx(1);
			break;
		case GDK_KEY_r:
			if (gameinfo.get_state() == GS_WON_BY_BLUE ||
			    gameinfo.get_state() == GS_WON_BY_RED) {
					gameinfo.set_state(GS_INIT);
				}
			break;
		default:
			break;
	}

	return false;
}

bool
GSVolley::event_loop()
{

	switch(gameinfo.get_state()) {
		case GS_INIT: 
			gameinfo.init();
			break;
		case GS_PLAY: 
			gameinfo.get_slime_blue().move();
			gameinfo.get_slime_red().move();
			gameinfo.collision_ball_slime (BLUE);
			gameinfo.collision_ball_slime (RED);
			gameinfo.collision_penalty();
			gameinfo.get_ball().move();
			
			if (gameinfo.get_ball().get_y() >= WIN_HEIGHT - BALL_RADIUS) {
				if ( gameinfo.get_ball().get_x() <= WIN_WIDTH / 2 ) {
					gameinfo.set_state(GS_GOT_BY_RED);
				} else {
					gameinfo.set_state(GS_GOT_BY_BLUE);
				}
			}
			break;
		case GS_GOT_BY_BLUE:
			gameinfo.score_move(BLUE);
			break;	
		case GS_GOT_BY_RED:
			gameinfo.score_move(RED);
			break;	
		case GS_SERVICE_BY_BLUE:
			gameinfo.serve_set(BLUE);
			break;
		case GS_SERVICE_BY_RED:
			gameinfo.serve_set(RED);
			break;
		case GS_WON_BY_BLUE:
			break;
		case GS_WON_BY_RED:
			break;
		default:
			std::cerr << "ERROR!! Unknown Game State" << std::endl;
			break;
	}
	queue_draw ();
	
	return true;
}

bool
GSVolley::cb_delete_event (GdkEventAny*)
{
	// timeout signal disconnect???
	return false;
}
	
int
main (int argc, char *argv[])
{
	Gtk::Main kit(argc, argv);
	
	GSVolley window;
	Gtk::Main::run(window);
	return EXIT_SUCCESS;
}
