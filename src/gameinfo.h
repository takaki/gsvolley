/*
 * gameinfo.h
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

#ifndef GAMEINFO_H
#define GAMEINFO_H

#include <gtk/gtk.h>
#include "defs.h"
#include "slime.h"
#include "ball.h"

typedef enum {
	GS_INIT,
	GS_PLAY,	
	GS_GOT_BY_BLUE,
	GS_GOT_BY_RED,
	GS_SERVICE_BY_BLUE,
	GS_SERVICE_BY_RED,
	GS_WON_BY_BLUE,
	GS_WON_BY_RED,
} GameState;

typedef struct {
	Slime *slime_blue;
	Slime *slime_red;
	Ball *ball;
	GtkWidget *window;
	
	GameState state;

	int score_blue;
	int score_red;
	int ball_count;
	TEAM ball_owner;
	double penalty_y;
	int wait_count;
} GameInfo;

void gameinfo_init(GameInfo *gi);
void gameinfo_score_move(GameInfo *gi, TEAM win);
void gameinfo_serve_set(GameInfo *gi, TEAM win);
void gameinfo_collision_ball_slime(GameInfo *gi, TEAM t);
void gameinfo_collision_penalty(GameInfo *gi);

#endif