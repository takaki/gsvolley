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

GameWin::GameWin(Cairo::RefPtr<Cairo::Context> cr):
cr(cr),
pattern_blue(Cairo::SolidPattern::create_rgb(0,0,1)),
pattern_red (Cairo::SolidPattern::create_rgb(1,0,0)),
pattern_ball (Cairo::SolidPattern::create_rgb(1,1,1)),
pattern_penalty (Cairo::SolidPattern::create_rgb(0,1,0))
{
	cr->set_font_size (16);

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
