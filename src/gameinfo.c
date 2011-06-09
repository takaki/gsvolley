/*
 * gameinfo.c
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

#include "gameinfo.h"
#include "slime.h"
#include <math.h>

void
gameinfo_init(GameInfo *gi)
{
	ball_init(gi->ball, BLUE);
	slime_init(gi->slime_blue);
	slime_init(gi->slime_red);
	gi->ball_count = 1;
	gi->wait_count = 0;
	gi->score_blue = SCORE_INIT;
	gi->score_red  = SCORE_INIT;
	gi->state = GS_SERVICE_BY_BLUE;
}

void gameinfo_score_move(GameInfo *gameinfo, TEAM win)
{
	if (gameinfo->ball_count > 0 ) {
		if (gameinfo->wait_count > 0) {
			--gameinfo->wait_count;
		} else  {
			--gameinfo->ball_count;
			if (gameinfo->ball_count == 0) {
				gameinfo->wait_count = 50; // XXX
			} else {
				gameinfo->wait_count = 7; // XXX
			}
			if (win == BLUE) {
				--gameinfo->score_red;
				if (gameinfo->score_red <= 0) {
					gameinfo->state = GS_WON_BY_BLUE;
				}
			} else {
				--gameinfo->score_blue;
				if (gameinfo->score_blue <= 0) {
					gameinfo->state = GS_WON_BY_RED;
				}
			}			
		}
	} else {
		if (gameinfo->wait_count > 0 ){
			--gameinfo->wait_count;
		} else {
			if (win == BLUE) {
				gameinfo->state = GS_SERVICE_BY_BLUE;
			} else {
				gameinfo->state = GS_SERVICE_BY_RED;
			}
		}
	}
};

void 
gameinfo_serve_set(GameInfo *gi, TEAM win)
{
	if (win == BLUE) {
		ball_init(gi->ball, BLUE);
	} else {
		ball_init(gi->ball, RED);
	}
	slime_init(gi->slime_blue);
	slime_init(gi->slime_red);
	gi->penalty_y = g_random_double_range(120, WIN_HEIGHT - 40); // XXX	
	gi->ball_count = 1;
	gi->wait_count = 0;
	gi->state = GS_PLAY;
}

typedef
struct {
	double x;
	double y;
} Vector;

static double
vector_inner_product(Vector a, Vector b)
{
	return (a.x * b.x + a.y * b.y);
};
static double
vector_outer_product(Vector a, Vector b)
{
	return (a.x * b.y - b.x * a.y);
};

static double
vector_norm(Vector a)
{
	return sqrt(a.x*a.x+a.y*a.y);
};

void
gameinfo_collision_ball_slime(GameInfo *gi, TEAM t){
	Ball *b = gi->ball;
	Slime *s;
	if (t == BLUE ){
		s = gi->slime_blue;
	} else {
		s = gi->slime_red;
	}
	Vector v_s = {b->vx, b->vy};
	Vector v_a = {s->x - b->x, WIN_HEIGHT-b->y};
	Vector v_b = {s->x - (b->x + b->vx), WIN_HEIGHT - (b->y + b->vy)};
	double c = vector_inner_product (v_a, v_s) * 
		vector_inner_product (v_b, v_s) ;
	double d = fabs(vector_outer_product (v_s, v_a))/ vector_norm(v_s);
	double r = SLIME_RADIUS + BALL_RADIUS;

	if ( d <= r && ( c<=0 || r > vector_norm (v_a) || r > vector_norm (v_b))){
		double dx_0 = b->x - s->x;
		double dy_0 = b->y - WIN_HEIGHT;
		double dist_0 = sqrt(pow(dx_0, 2) + pow(dy_0, 2));
		double angle = acos (dx_0/dist_0);
		double cos2x = cos(2*angle);
		double sin2x = sin(2*angle);
		double vx = - b->vx * cos2x + b->vy * sin2x;
		double vy =   b->vx * sin2x + b->vy * cos2x;		
		b->vx = vx;
		b->vy = vy;

#if 0
		double pe = GRAVITY * (WIN_HEIGHT - b->y );
		double ke = 0.5 * (pow(b->vx, 2) + pow(b->vy, 2));
		pe = GRAVITY * (WIN_HEIGHT - b->y );
		ke = 0.5 * (pow(b->vx, 2) + pow(b->vy, 2));
		g_print("POST x=%+.2f vx=%+.2f y=%+.2f vy=%+.2f pe=%+.4f ke=%+.4f pe+ke=%+.4f\n",
		        b->x, b->vx, b->y, b->vy, pe, ke, pe+ke);
#endif
		gi->ball_owner = t;
		gi->ball_count++;
	}
}

void
gameinfo_collision_penalty (GameInfo *gi)
{
	Ball *b = gi->ball;
	if (sqrt(pow(b->x-WIN_WIDTH/2,2)+pow(b->y - gi->penalty_y,2)) < 
	    PENALTY_RADIUS + BALL_RADIUS)
	{
		--gi->wait_count;
		if (gi->ball_owner == BLUE ) {
			if (gi->score_blue > 1 && gi->wait_count <= 0) {
				--gi->score_blue;				
				gi->wait_count = 4;
			}				
		} else {
			if (gi->score_red > 1 && gi->wait_count <= 0) {
				--gi->score_red;
				gi->wait_count = 4;
			}
		}
	}
}

