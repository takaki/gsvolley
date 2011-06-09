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

#include <gtk/gtk.h>
#include "ball.h"
#include "slime.h"
#include "gameinfo.h"

typedef struct {
	GdkWindow *window;
	cairo_t *cr;

	cairo_pattern_t *pattern_blue;
	cairo_pattern_t *pattern_red;
	cairo_pattern_t *pattern_ball;
	cairo_pattern_t *pattern_penalty;
} GameWin;

GameWin *gamewin_new(GdkWindow *window);
void gamewin_free(GameWin *gw);
void gamewin_draw_back(GameWin *gw);
void gamewin_draw_slime(GameWin *gw, Slime *s);
void gamewin_draw_ball(GameWin *gw, Ball *b);
void gamewin_draw_penalty(GameWin *gw, double y);
void gamewin_draw_score(GameWin *gw, int blue, int count, int red);
void gamewin_show_text_centering(GameWin *gw, char *text, double y);

#endif /* GAMEWIN_H */
