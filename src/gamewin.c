/*
 * gamewin.c
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

#include <math.h>
#include <glib.h>
#include <glib/gprintf.h>

#include "slime.h"
#include "gamewin.h"
#include "defs.h"

GameWin *
gamewin_new(GdkWindow *window)
{
	GameWin *gw = g_new(GameWin, 1);
	gw->window = window;

	gw->cr = gdk_cairo_create (window);
	cairo_set_font_size (gw->cr, 16);

	gw->pattern_blue = cairo_pattern_create_rgb(0,0,1);
	gw->pattern_red = cairo_pattern_create_rgb(1,0,0);
	gw->pattern_ball = cairo_pattern_create_rgb(1,1,1);
	gw->pattern_penalty = cairo_pattern_create_rgb(0,1,0);
	return gw;
}

void
gamewin_free(GameWin *gw)
{
	cairo_destroy(gw->cr);
	cairo_pattern_destroy(gw->pattern_blue);
	cairo_pattern_destroy(gw->pattern_red);
	cairo_pattern_destroy(gw->pattern_ball);
	cairo_pattern_destroy(gw->pattern_penalty);
	g_free(gw);
}

void
gamewin_draw_back(GameWin *gw)
{
	cairo_set_source_rgb (gw->cr, 0,0,0);
	cairo_rectangle (gw->cr, 0, 0, WIN_WIDTH, WIN_HEIGHT);
	cairo_fill(gw->cr);

	cairo_set_source_rgb(gw->cr, 1,1,1);
	cairo_rectangle (gw->cr, WIN_WIDTH/2 - 1,120 , 2, WIN_HEIGHT);
	cairo_fill(gw->cr);
}
void
gamewin_draw_slime(GameWin *gw, Slime *s)
{
	if (s->team == BLUE) {
		cairo_set_source (gw->cr, gw->pattern_blue);
	} else {
		cairo_set_source (gw->cr, gw->pattern_red);
	}	
	cairo_move_to(gw->cr, s->x, WIN_HEIGHT);
	cairo_arc(gw->cr,
	          s->x,
	          WIN_HEIGHT,
	          SLIME_RADIUS,
	          M_PI,0);
	cairo_fill(gw->cr);
}

void
gamewin_draw_ball(GameWin *gw, Ball *b)
{
	cairo_set_source(gw->cr, gw->pattern_ball);
	cairo_arc(gw->cr,
	          b->x,
	          b->y,
	          BALL_RADIUS,
	          0,
	          2 * M_PI);
	cairo_fill(gw->cr);
}
void
gamewin_draw_penalty(GameWin *gw, double y)
{
    cairo_set_line_width(gw->cr, 5.0);  

	cairo_set_source(gw->cr, gw->pattern_penalty);
	cairo_arc(gw->cr, WIN_WIDTH/2,y, PENALTY_RADIUS, // XXX
	          0, 2 * M_PI);
	cairo_stroke_preserve(gw->cr);

	cairo_set_source_rgb (gw->cr, 0,0,0);
	cairo_fill(gw->cr);

	cairo_set_source(gw->cr, gw->pattern_penalty);
	cairo_arc(gw->cr, WIN_WIDTH/2,y, PENALTY_RADIUS - 15 , // XXX
	          0, 2 * M_PI);
    cairo_set_line_width(gw->cr, 5.0);  
	cairo_stroke_preserve(gw->cr);

	cairo_set_source_rgb (gw->cr, 0,0,0);
	cairo_fill(gw->cr);
	

}

void
gamewin_draw_score(GameWin *gw, int blue, int count, int red)
{
	char buf[80]; // XXX 
	g_sprintf(buf, "Blue: %2d Ball: %3d Red: %2d", blue, count,red);
	gamewin_show_text_centering (gw, buf, 20);
}

void 
gamewin_show_text_centering(GameWin *gw, char *text, double y)
{
	cairo_text_extents_t te;
	cairo_text_extents(gw->cr, text, &te);
	cairo_move_to(gw->cr, (WIN_WIDTH - te.width)/2, y);
	cairo_show_text(gw->cr, text);
}
