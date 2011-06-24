/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) TANIGUCHI Takaki 2011 <takaki@media-as.org>
 * 
 * gsvolley is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * gsvolley is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <config.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#include "defs.h"
#include "slime.h"
#include "ball.h"
#include "gamewin.h"
#include "gameinfo.h"

// guint timer_id;

static gboolean
cb_expose_event(GtkWidget *widget,
                cairo_t *cr,
                gpointer user_data)
{
	if (! gtk_cairo_should_draw_window (cr, gtk_widget_get_window(widget))){
		return FALSE;
	}
	GameInfo *gameinfo = user_data;
	GameWin *gw = gamewin_new(cr);
	
	gamewin_draw_back(gw);
	gamewin_draw_slime(gw, gameinfo->slime_blue);
	gamewin_draw_slime(gw, gameinfo->slime_red);
	gamewin_draw_penalty (gw, gameinfo->penalty_y);
	gamewin_draw_ball(gw, gameinfo->ball);
	gamewin_draw_score(gw, gameinfo->score_blue, gameinfo->ball_count,
	                   gameinfo->score_red);

	switch(gameinfo -> state) {
		case GS_INIT: 
			break;
		case GS_PLAY: 
			break;
		case GS_GOT_BY_BLUE:
			gamewin_show_text_centering (gw, "DROPPED BY RED", 40);		
			break;	
		case GS_GOT_BY_RED:
			gamewin_show_text_centering (gw, "DROPPED BY BLUE", 40);		
			break;	
		case GS_SERVICE_BY_BLUE:
			break;
		case GS_SERVICE_BY_RED:
			break;
		case GS_WON_BY_BLUE:
			gamewin_show_text_centering (gw, "PRESS R TO PLAY AGAIN", 80);		
			gamewin_show_text_centering (gw, "WON BY BLUE", 60);		
			break;
		case GS_WON_BY_RED:
			gamewin_show_text_centering (gw, "PRESS R TO PLAY AGAIN", 80);		
			gamewin_show_text_centering (gw, "WON BY RED", 60);		
			break;
		default:
			g_error("ERROR!! Unknown Game State\n");
			break;
	}
	gamewin_free(gw);

	return FALSE;
	
}

static gboolean
cb_key_press_event(GtkWidget *widget,
                   GdkEventKey *event,
                   gpointer user_data)
{
	GameInfo *gi = user_data;
	switch(event->keyval) {
		case GDK_KEY_a:
			gi->slime_blue->vx = -1;
			break;
		case GDK_KEY_s:
			gi->slime_blue->vx = 1;
			break;
		case GDK_KEY_Left:
			gi->slime_red->vx = -1;
			break;
		case GDK_KEY_Right:
			gi->slime_red->vx = 1;
			break;
		case GDK_KEY_r:
			if (gi->state == GS_WON_BY_BLUE ||
			    gi->state == GS_WON_BY_RED) {
					gi->state = GS_INIT;
				}
			break;
		default:
			break;
	}

	return FALSE;
}

static gboolean
event_loop(gpointer data)
{
	GameInfo *gameinfo = data;

	switch(gameinfo -> state) {
		case GS_INIT: 
			gameinfo_init(gameinfo);
			break;
		case GS_PLAY: 
			slime_move(gameinfo->slime_blue);
			slime_move(gameinfo->slime_red);
			gameinfo_collision_ball_slime (gameinfo, BLUE);
			gameinfo_collision_ball_slime (gameinfo, RED);
			gameinfo_collision_penalty(gameinfo);
			ball_move(gameinfo->ball);
			
			if (gameinfo->ball->y >= WIN_HEIGHT - BALL_RADIUS) {
				if ( gameinfo->ball->x <= WIN_WIDTH / 2 ) {
					gameinfo->state = GS_GOT_BY_RED;
				} else {
					gameinfo->state = GS_GOT_BY_BLUE;
				}
			}
			break;
		case GS_GOT_BY_BLUE:
			gameinfo_score_move(gameinfo, BLUE);
			break;	
		case GS_GOT_BY_RED:
			gameinfo_score_move(gameinfo, RED);
			break;	
		case GS_SERVICE_BY_BLUE:
			gameinfo_serve_set(gameinfo, BLUE);
			break;
		case GS_SERVICE_BY_RED:
			gameinfo_serve_set(gameinfo, RED);
			break;
		case GS_WON_BY_BLUE:
			break;
		case GS_WON_BY_RED:
			break;
		default:
			g_error("ERROR!! Unknown Game State\n");
			break;
	}
			
	gtk_widget_queue_draw (gameinfo->window);
	
	return TRUE;
}

static gint
cb_delete_event(GtkWidget *widget, 
                GdkEventExpose *event,
                gpointer data)
{
	gint *timeout = data;
	if (*timeout) {
		g_source_remove(*timeout);
	}
	return FALSE;
}
                
int
main (int argc, char *argv[])
{
 	GtkWidget *window;

	gtk_init (&argc, &argv);
	GameInfo gameinfo = {
		slime_new(BLUE),
		slime_new(RED),
		ball_new()
	};

	gameinfo_init(&gameinfo);
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gameinfo.window = window;
	gtk_widget_set_size_request (window, WIN_WIDTH, WIN_HEIGHT);
	gtk_window_set_has_resize_grip(GTK_WINDOW(window), FALSE);
	gtk_widget_set_app_paintable(window, TRUE);

	int timeout = g_timeout_add(10, event_loop, &gameinfo);
	
	g_signal_connect(window, "draw", G_CALLBACK(cb_expose_event), &gameinfo);
	g_signal_connect(window, "key_press_event", G_CALLBACK(cb_key_press_event), &gameinfo);
	g_signal_connect(window, "delete_event", G_CALLBACK(cb_delete_event), &timeout);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit),
	                 NULL);
	gtk_widget_show (window);
	
	gtk_main ();

	return 0;
}
