// gamewin.cc
//
// Copyright (C) 2011 - TANIGUCHI Takaki
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "defs.h"
#include "gamewin.h"
#include <sstream>
#include <iomanip>
#include <iostream>

GameWin::GameWin():
pattern_blue(Cairo::SolidPattern::create_rgb(0,0,1)),
pattern_red (Cairo::SolidPattern::create_rgb(1,0,0)),
pattern_ball (Cairo::SolidPattern::create_rgb(1,1,1)),
pattern_penalty (Cairo::SolidPattern::create_rgb(0,1,0))
{
	set_size_request (WIN_WIDTH, WIN_HEIGHT);
	set_resizable (false);
	set_app_paintable (true);

	signal_draw().connect(sigc::mem_fun(*this, &GameWin::cb_expose_event ));
	signal_key_press_event().connect(sigc::mem_fun(*this, &GameWin::cb_key_press),false);
	signal_delete_event().connect(sigc::mem_fun(*this, &GameWin::cb_delete_event));

	Glib::signal_timeout ().connect(sigc::mem_fun(*this, &GameWin::event_loop),10);
	gameinfo.init();

}


bool
GameWin::cb_expose_event(const Cairo::RefPtr< Cairo::Context >& cr_)
{
	cr = cr_;
	cr->set_font_size (16);
	
	draw_back();
	draw_slime(gameinfo.get_slime_blue());
	draw_slime(gameinfo.get_slime_red());
	draw_penalty(gameinfo.get_penalty_y());
	draw_ball(gameinfo.get_ball());
	draw_score(gameinfo.get_score_blue(), gameinfo.get_ball_count(),
	                   gameinfo.get_score_red());

	switch(gameinfo.get_state()) {
		case GS_INIT: 
			break;
		case GS_PLAY: 
			break;
		case GS_GOT_BY_BLUE:
			show_text_centering ("DROPPED BY RED", 40);		
			break;	
		case GS_GOT_BY_RED:
			show_text_centering ("DROPPED BY BLUE", 40);		
			break;	
		case GS_SERVICE_BY_BLUE:
			break;
		case GS_SERVICE_BY_RED:
			break;
		case GS_WON_BY_BLUE:
			show_text_centering ("PRESS R TO PLAY AGAIN", 80);		
			show_text_centering ("WON BY BLUE", 60);		
			break;
		case GS_WON_BY_RED:
			show_text_centering ("PRESS R TO PLAY AGAIN", 80);		
			show_text_centering ("WON BY RED", 60);		
			break;
		default:
			std::cerr << "ERROR!! Unknown Game State" << std::endl;
			break;
	}
	return false;
}

bool
GameWin::cb_key_press (GdkEventKey *ev)
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
GameWin::event_loop()
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
GameWin::cb_delete_event (GdkEventAny*)
{
	// timeout signal disconnect???
	return false;
}
	
void
GameWin::draw_back()
{
	cr->set_source_rgb(0,0,0);
	cr->rectangle (0,0,WIN_WIDTH, WIN_HEIGHT);
	cr->fill();
	cr->set_source_rgb(1,1,1);
	cr->rectangle (WIN_WIDTH/2 - 1,120 , 2, WIN_HEIGHT);
	cr->fill();
}

void
GameWin::draw_slime(const Slime &s)
{
	if (s.get_team() == BLUE) {
		cr->set_source (pattern_blue);
	} else {
		cr->set_source (pattern_red);
	}	
	cr->move_to(s.get_x(), WIN_HEIGHT);
	cr->arc(
	          s.get_x(),
	          WIN_HEIGHT,
	          SLIME_RADIUS,
	          M_PI,0);
	cr->fill();
}

void
GameWin::draw_ball(const Ball &b)
{
	cr->set_source(pattern_ball);
	cr->arc(
	          b.get_x(),
	          b.get_y(),
	          BALL_RADIUS,
	          0,
	          2 * M_PI);
	cr->fill();
}
void
GameWin::draw_penalty(const double y)
{
	cr->set_line_width(5.0);  

	cr->set_source(pattern_penalty);
	cr->arc(WIN_WIDTH/2,y, PENALTY_RADIUS, // XXX
	          0, 2 * M_PI);
	cr->stroke_preserve();

	cr->set_source_rgb (0,0,0);
	cr->fill();
	
	cr->set_source(pattern_penalty);
	cr->arc(WIN_WIDTH/2,y, PENALTY_RADIUS - 15 , // XXX
	          0, 2 * M_PI);
    cr->set_line_width( 5.0);  
	cr->stroke_preserve();

	cr->set_source_rgb ( 0,0,0);
	cr->fill();

}

void
GameWin::draw_score(const int blue, const int count, const int red)
{
	std::stringstream buf;
	buf << "Blue: " << std::setw(2) << blue 
		<< " Ball: " << std::setw(3) << count
		<< " Red: " << std::setw(2) << red;
	show_text_centering (buf.str(), 20);
}

void 
GameWin::show_text_centering(const std::string &text, const double y)
{
	Cairo::TextExtents te;
	cr->get_text_extents (text, te);
	cr->move_to((WIN_WIDTH - te.width)/2, y);
	cr->show_text(text);
}
