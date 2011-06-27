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

class GameInfo {
	public:
		GameInfo();
		void init();
		void score_move(TEAM win);
		void serve_set(TEAM win);
		void collision_ball_slime(TEAM t);
		void collision_penalty();

		Slime &get_slime_blue(){return slime_blue;}
		Slime &get_slime_red(){return slime_red;}
		GameState get_state(){return state;}
		void set_state(const GameState s){state = s;}
		double get_penalty_y(){return penalty_y;}
		Ball &get_ball(){return ball;}
		int get_score_blue(){return score_blue;}
		int get_score_red(){return score_red;}
		int get_ball_count(){return ball_count;}
		
	private:	
		Slime slime_blue;
		Slime slime_red;
		Ball ball;
		

		GameState state;

		int score_blue;
		int score_red;
		int ball_count;
		TEAM ball_owner;
		double penalty_y;
		int wait_count;
};

#endif