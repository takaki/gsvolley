/*
 * ball.h
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

#ifndef BALL_H
#define BALL_H

#include "slime.h"
#include <glib.h>




typedef
struct {
	double x;
	double y;
	double vx;
	double vy;
} Ball;


Ball *ball_new(void);
void ball_free(Ball *b);
void ball_init(Ball *b, TEAM t);
void ball_move(Ball *b);


#endif /* BALL_H */