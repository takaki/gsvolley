// gameinfo.cc
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
#include "gameinfo.h"
#include "ball.h"
#include <glibmm/random.h>
#include <cmath>

GameInfo::GameInfo() :
slime_blue(BLUE),
slime_red(RED)
{
	init();
}

void
GameInfo::init()
{
	ball.init(BLUE);
	slime_blue.init();
	slime_red.init();
	ball_owner = BLUE;
	ball_count = 1;
	wait_count = 0;
	score_blue = SCORE_INIT;
	score_red  = SCORE_INIT;
	state = GS_SERVICE_BY_BLUE;
}

void 
GameInfo::score_move(TEAM win)
{
	if (ball_count > 0 ) {
		if (wait_count > 0) {
			--wait_count;
		} else  {
			--ball_count;
			if (ball_count == 0) {
				wait_count = 50; // XXX
			} else {
				wait_count = 7; // XXX
			}
			if (win == BLUE) {
				--score_red;
				if (score_red <= 0) {
					state = GS_WON_BY_BLUE;
				}
			} else {
				--score_blue;
				if (score_blue <= 0) {
					state = GS_WON_BY_RED;
				}
			}			
		}
	} else {
		if (wait_count > 0 ){
			--wait_count;
		} else {
			if (win == BLUE) {
				state = GS_SERVICE_BY_BLUE;
			} else {
				state = GS_SERVICE_BY_RED;
			}
		}
	}
};

void 
GameInfo::serve_set(TEAM win)
{
	if (win == BLUE) {
		ball.init(BLUE);
	} else {
		ball.init(RED);
	}
	slime_blue.init();
	slime_red.init();
	Glib::Rand r;
	penalty_y = r.get_double_range(120, WIN_HEIGHT - 40); // XXX	
	ball_count = 1;
	wait_count = 0;
	state = GS_PLAY;
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
GameInfo::collision_ball_slime(TEAM t){
	Ball &b = ball;
	Slime *s;
	if (t == BLUE ){
		s = &slime_blue;
	} else {
		s = &slime_red;
	}
	Vector v_s = {b.get_vx(), b.get_vy()};
	Vector v_a = {s->get_x() - b.get_x(), WIN_HEIGHT-b.get_y()};
	Vector v_b = {s->get_x() - (b.get_x() + b.get_vx()), WIN_HEIGHT - (b.get_y() + b.get_vy())};
	double c = vector_inner_product (v_a, v_s) * 
		vector_inner_product (v_b, v_s) ;
	double d = fabs(vector_outer_product (v_s, v_a))/ vector_norm(v_s);
	double r = SLIME_RADIUS + BALL_RADIUS;

	if ( d <= r && ( c<=0 || r > vector_norm (v_a) || r > vector_norm (v_b))){
		double dx_0 = b.get_x() - s->get_x();
		double dy_0 = b.get_y() - WIN_HEIGHT;
		double dist_0 = sqrt(pow(dx_0, 2) + pow(dy_0, 2));
		double angle = acos (dx_0/dist_0);
		double cos2x = cos(2*angle);
		double sin2x = sin(2*angle);
		double vx = - b.get_vx() * cos2x + b.get_vy() * sin2x;
		double vy =   b.get_vx() * sin2x + b.get_vy() * cos2x;		
		b.set_vx(vx);
		b.set_vy(vy);

		ball_owner = t;
		ball_count++;
	}
}

void
GameInfo::collision_penalty ()
{
	Ball &b = ball;
	if (sqrt(pow(b.get_x()-WIN_WIDTH/2,2)+pow(b.get_y() - penalty_y,2)) < 
	    PENALTY_RADIUS + BALL_RADIUS)
	{
		--wait_count;
		if (ball_owner == BLUE ) {
			if (score_blue > 1 && wait_count <= 0) {
				--score_blue;				
				wait_count = 4; // XXX
			}				
		} else {
			if (score_red > 1 && wait_count <= 0) {
				--score_red;
				wait_count = 4; // XXX
			}
		}
	}
}

