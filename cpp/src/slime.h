/*
 * slime.h
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

#ifndef SLIME_H
#define SLIME_H


typedef enum {
	BLUE,
	RED,
} TEAM;

class Slime {
	public:
		explicit Slime(TEAM t);
		void init();
		void move();

		TEAM get_team() const {return team;};
		int get_x() const {return x;};
		void set_vx(int vx_){vx = vx_;}

	private:
		const TEAM team;
		int x;
		int vx;
		int score;
		int limit_left;
		int limit_right;
};


#endif
