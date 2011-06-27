// slime.cc
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
#include "slime.h"

Slime::Slime(TEAM t) : team(t)
{
	if (t == BLUE) {
		limit_left = SLIME_RADIUS;
		limit_right = WIN_WIDTH / 2 - SLIME_RADIUS;
	} else {
		limit_left = WIN_WIDTH / 2 + SLIME_RADIUS;
		limit_right = WIN_WIDTH - SLIME_RADIUS;

	}
	init();
}

void
Slime::init()
{
	if ( team == BLUE) {
		x = 40;
		vx = 0;
	} else {
		x = 600;
		vx = 0;
	}
}

void
Slime::move()
{
	x += vx;
	if (x < limit_left ) {
		x = limit_left;
		vx = 0;
	}
	if (limit_right < x ) {
		x = limit_right;
		vx = 0;		
	}
	return ;
}


