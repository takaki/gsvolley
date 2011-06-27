/*
 * gamewin.h
 *
 * Copyright (C) 2011 - TANIGUCHI Takaki
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GAMEWIN_H
#define GAMEWIN_H
#include <cairomm/cairomm.h>
#include "ball.h"

class GameWin {
	public:
		GameWin(Cairo::RefPtr<Cairo::Context> cr);
		~GameWin(){};
		void draw_back();
		void draw_ball(const Ball &b);
		void draw_slime(const Slime &s);
		void draw_penalty(const double y);
		void draw_score(const int blue, const int count, const int red);
		void show_text_centering(const std::string &text, const double y);
	private:
		Cairo::RefPtr<Cairo::Context> cr;
		Cairo::RefPtr<Cairo::SolidPattern> pattern_blue;
		Cairo::RefPtr<Cairo::SolidPattern> pattern_red;
		Cairo::RefPtr<Cairo::SolidPattern> pattern_ball;
		Cairo::RefPtr<Cairo::SolidPattern> pattern_penalty;
};

#endif
