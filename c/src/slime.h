/***************************************************************************
 *            
  * slime.h
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
  *
  ****************************************************************************/

#ifndef SLIME_H
#define SLIME_H

#include <cairo.h>

#define SLIME_RADIUS 40

typedef enum {
	BLUE,
	RED,
} TEAM;

typedef struct {
	TEAM team;
	int x;
	int vx;
	int score;
	int limit_left;
	int limit_right;
} Slime;


Slime* slime_new(TEAM t);
void slime_free(Slime *s);

void slime_init(Slime *s);
void slime_move(Slime *s);



#endif /* SLIME_H */