/*
 * ball.c
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

#include "ball.h"
#include "defs.h"
#include "slime.h"
#include <math.h>
#include <glib.h>




Ball*
ball_new(void)
{
	Ball *b = g_new(Ball, 1);
	ball_init(b, BLUE);
	return b;
}
void ball_free(Ball *b)
{
	g_free(b);
}

void
ball_init(Ball *b, TEAM t)
{
	if (t == BLUE) {
		b->x = BALL_SERVE_OFFSET;
	} else {
		b->x = WIN_WIDTH - BALL_SERVE_OFFSET;
	}
		
	b->y = BALL_Y_INIT;
	b->vx = 0;
	b->vy = 0;
	return ;
}

void
ball_move(Ball *b)
{
	b->vy += GRAVITY;	
	b->x += b->vx;
	b->y += b->vy;

	if ( b->x < 0 + BALL_RADIUS ){
		b->x = (BALL_RADIUS - b->x) + BALL_RADIUS;
		b->vx = - b->vx;

	};
	if ( b->x > WIN_WIDTH - BALL_RADIUS) {
		b->x = (WIN_WIDTH - BALL_RADIUS) - (b->x - (WIN_WIDTH - BALL_RADIUS)); 	
		b->vx = - b->vx;
	}
}
