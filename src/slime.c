/*
 * slime.c
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

#include "slime.h"
#include "defs.h"
#include <glib.h>
#include <math.h>

Slime* 
slime_new(TEAM t)
{
	Slime *s = g_new(Slime,1);
	s->team = t;
	
		
	if (t == BLUE) {
		s->limit_left = SLIME_RADIUS;
		s->limit_right = WIN_WIDTH / 2 - SLIME_RADIUS;
	} else {
		s->limit_left = WIN_WIDTH / 2 + SLIME_RADIUS;
		s->limit_right = WIN_WIDTH - SLIME_RADIUS;

	}
	slime_init(s);
	return s;	
}

void
slime_free(Slime *s)
{
	g_free(s);
}

void
slime_init(Slime *s)
{
	if ( s->team == BLUE) {
		s->x = 40;
		s->vx = 0;
	} else {
		s->x = 600;
		s->vx = 0;
	}
}

void
slime_move(Slime *s)
{
	s->x += s->vx;
	if (s->x < s->limit_left ) {
		s->x = s->limit_left;
		s->vx = 0;
	}
	if (s->limit_right < s->x ) {
		s->x = s->limit_right;
		s->vx = 0;		
	}
	return ;
}


