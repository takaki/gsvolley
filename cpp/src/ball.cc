// ball.cc
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

#include "ball.h"
#include "defs.h"

void
Ball::init (TEAM t)
{
	if (t == BLUE) {
		x = BALL_SERVE_OFFSET;
	} else {
		x = WIN_WIDTH - BALL_SERVE_OFFSET;
	}
		
	y = BALL_Y_INIT;
	vx = 0;
	vy = 0;
}

void
Ball::move()
{
	vy += GRAVITY;	
	x += vx;
	y += vy;

	if ( x < 0 + BALL_RADIUS ){
		x = (BALL_RADIUS - x) + BALL_RADIUS;
		vx = - vx;

	};
	if ( x > WIN_WIDTH - BALL_RADIUS) {
		x = (WIN_WIDTH - BALL_RADIUS) - (x - (WIN_WIDTH - BALL_RADIUS)); 	
		vx = - vx;
	}
}
