#!/usr/bin/python

import sys

import pygtk
pygtk.require("2.0")
import gtk
import glib
import cairo
import math
import random

WIN_WIDTH  = 640
WIN_HEIGHT = 400

GRAVITY           = 0.03
SCORE_INIT        = 2

BALL_RADIUS       = 10
BALL_SERVE_OFFSET = 76.5
BALL_Y_INIT = 20
PENALTY_RADIUS    = 30

SLIME_RADIUS = 40

class Ball():
    def __init__(self):
        self.init(Slime.BLUE)

    def init(self, team):
        if team == Slime.BLUE:
            self.x = BALL_SERVE_OFFSET
        else:
            self.x = WIN_WIDTH - BALL_SERVE_OFFSET

        self.y = BALL_Y_INIT
        self.vx = 0
        self.vy = 0


    def move(self):
        self.vy += GRAVITY
        self.x += self.vx
        self.y += self.vy

        if self.x < 0 + BALL_RADIUS:
            self.x = (BALL_RADIUS - self.x) + BALL_RADIUS
            self.vx = - self.vx
        if self.x > WIN_WIDTH - BALL_RADIUS:
            self.x = (WIN_WIDTH - BALL_RADIUS) - (self.x - (WIN_WIDTH - BALL_RADIUS))
            self.vx = - self.vx

class Vector():
    def __init__(self, x = 0, y = 0):
        self.x = x
        self.y = y

    @staticmethod
    def inner_product(a,b):
        return a.x * b.x + a.y * b.y

    @staticmethod
    def outer_product(a,b):
        return a.x * b.y - b.x * a.y

    def norm(self):
        return math.sqrt(self.x*self.x + self.y*self.y)



class GameInfo():
    (GS_INIT, GS_PLAY,  GS_GOT_BY_BLUE,
     GS_GOT_BY_RED,  GS_SERVICE_BY_BLUE,
     GS_SERVICE_BY_RED, GS_WON_BY_BLUE,
     GS_WON_BY_RED) = range(8)

    def __init__(self):
        self.slime_blue = None
        self.slime_red = None
        self.ball = None
        self.window = None


    def init(self):
        self.ball.init(Slime.BLUE)
        self.slime_blue.init()
        self.slime_red.init()
        
        self.ball_owner = Slime.BLUE
        self.ball_count = 1
        self.wait_count = 0
        self.score_blue = SCORE_INIT
        self.score_red = SCORE_INIT
        self.state = self.GS_SERVICE_BY_BLUE

    def score_move(self, win):
        if self.ball_count > 0:
            if self.wait_count > 0:
                self.wait_count -= 1
            else:
                self.ball_count -= 1
                if self.ball_count == 0:
                    self.wait_count = 50
                else:
                    self.wait_count = 7
                if win == Slime.BLUE:
                    self.score_red -= 1
                    if self.score_red <= 0:
                        self.state = GameInfo.GS_WON_BY_BLUE
                else:
                    self.score_blue -= 1
                    if self.score_blue <= 0:
                        self.state = GameInfo.GS_WON_BY_RED
        else:
            if self.wait_count > 0 :
                self.wait_count -= 1
            else:
                if win == Slime.BLUE:
                    self.state = GameInfo.GS_SERVICE_BY_BLUE
                else:
                    self.state = GameInfo.GS_SERVICE_BY_RED

    def serve_set(self, win):
        if win == Slime.BLUE:
            self.ball.init(Slime.BLUE)
        else:
            self.ball.init(Slime.RED)
        self.slime_blue.init()
        self.slime_red.init()
        self.penalty_y = random.uniform(120, WIN_HEIGHT -  40)
        self.ball_count = 1
        self.wait_count = 0
        self.state = GameInfo.GS_PLAY        

    def collision_ball_slime(self, team):
        b = self.ball
        if team == Slime.BLUE:
            s = self.slime_blue
        else:
            s = self.slime_red
        
        v_s = Vector(b.vx, b.vy)
        v_a = Vector(s.x - b.x, WIN_HEIGHT - b.y)
        v_b = Vector(s.x - (b.x + b.vx), WIN_HEIGHT - (b.y + b.vy))
        c = Vector.inner_product(v_a, v_s) * Vector.inner_product(v_b, v_s)
        try:
            d = abs(Vector.outer_product(v_s, v_a))/ v_s.norm()
        except ZeroDivisionError:
            return
        r = SLIME_RADIUS + BALL_RADIUS
        if d <= r and ( c<=0 or r>v_a.norm() or r > v_b.norm()):
            dx_0 = b.x - s.x
            dy_0 = b.y - WIN_HEIGHT
            dist_0 = Vector(dx_0, dy_0).norm()
            angle = math.acos(dx_0/dist_0)
            cos2x = math.cos(2*angle)
            sin2x = math.sin(2*angle)
            b.vx, b.vy = (-b.vx * cos2x + b.vy * sin2x,
                          b.vx * sin2x + b.vy * cos2x)

            self.ball_owner = team
            self.ball_count += 1

    def collision_penalty(self):
        b = self.ball
        if (Vector(b.x - WIN_WIDTH/2, b.y - self.penalty_y).norm() <
            PENALTY_RADIUS + BALL_RADIUS):
            self.wait_count -= 1
            if self.ball_owner == Slime.BLUE:
                if self.score_blue > 1 and self.wait_count <= 0:
                    self.score_blue -= 1
                    self.wait_count = 4
            else:
                if self.score_red > 1 and self.wait_count <= 0:
                    self.score_red -= 1
                    self.wait_count = 4
                    
            

class GameWin():
    def __init__(self, window):
        self.window = window
        self.cr = window.cairo_create()
        self.cr.set_font_size(16)

        self.pattern_blue = cairo.SolidPattern(0,0,1)
        self.pattern_red = cairo.SolidPattern(1,0,0)
        self.pattern_ball = cairo.SolidPattern(1,1,1)
        self.pattern_penalty = cairo.SolidPattern(0,1,0)

    def draw_back(self):
        self.cr.set_source_rgb(0,0,0)
        self.cr.rectangle(0,0,WIN_WIDTH, WIN_HEIGHT)
        self.cr.fill()

        self.cr.set_source_rgb(1,1,1)
        self.cr.rectangle(WIN_WIDTH/2 - 1, 120, 2, WIN_HEIGHT)
        self.cr.fill()

    def draw_slime(self, slime):
        if slime.team == Slime.BLUE:
            self.cr.set_source(self.pattern_blue)
        else:
            self.cr.set_source(self.pattern_red)
        self.cr.move_to(slime.x, WIN_HEIGHT)
        self.cr.arc(slime.x, WIN_HEIGHT, SLIME_RADIUS,
               math.pi, 0)
        self.cr.fill()

    def draw_ball(self, ball):
        self.cr.set_source(self.pattern_ball)
        self.cr.arc(ball.x, ball.y, BALL_RADIUS,
                    0, 2 * math.pi)
        self.cr.fill()

    def draw_penalty(self, y):
        self.cr.set_line_width(5)
        self.cr.set_source(self.pattern_penalty)
        self.cr.arc(WIN_WIDTH/2, y, PENALTY_RADIUS, 0, 2 * math.pi)
        self.cr.stroke_preserve()
        
        self.cr.set_source_rgb(0,0,0)
        self.cr.fill()

        self.cr.set_source(self.pattern_penalty)
        self.cr.arc(WIN_WIDTH/2, y, PENALTY_RADIUS - 15, 0, 2 * math.pi)
        self.cr.set_line_width(5)
        self.cr.stroke_preserve()

        self.cr.set_source_rgb(0,0,0)
        self.cr.fill()

    def draw_score(self, blue, count, red):
        buf = "Blue: %2d Ball: %3d Red: %2d"% (blue, count,red)
        self.show_text_centering(buf, 20)

    def show_text_centering(self, text, y):
        te = self.cr.text_extents(text)
        self.cr.move_to((WIN_WIDTH-te[2])/2, y)
        self.cr.show_text(text)

class Slime():
    BLUE, RED = range(2)
    def __init__(self, team):
        self.team = team
        if self.team == Slime.BLUE:
            self.limit_left = SLIME_RADIUS
            self.limit_right = WIN_WIDTH / 2 - SLIME_RADIUS
        else:
            self.limit_left = WIN_WIDTH / 2 + SLIME_RADIUS
            self.limit_right = WIN_WIDTH  - SLIME_RADIUS
        self.init()
            
    def init(self):
        if self.team == Slime.BLUE:
            self.x = self.limit_left
            self.vx = 0
        else:
            self.x = self.limit_right
            self.vx = 0

    def move(self):
        self.x += self.vx
        if self.x < self.limit_left:
            self.x = self.limit_left
            self.vx = 0
        if self.x > self.limit_right:
            self.x = self.limit_right
            self.vx = 0

def cb_expose_event(widget, event, gameinfo):
    gw = GameWin(widget.get_window())
    gw.draw_back()
    gw.draw_slime(gameinfo.slime_blue)
    gw.draw_slime(gameinfo.slime_red)
    gw.draw_penalty(gameinfo.penalty_y)
    gw.draw_ball(gameinfo.ball)
    gw.draw_score(gameinfo.score_blue, gameinfo.ball_count,
                  gameinfo.score_red)
    
    if gameinfo.state == GameInfo.GS_INIT:
        pass # OK
    elif gameinfo.state == GameInfo.GS_PLAY:
        pass # OK
    elif gameinfo.state == GameInfo.GS_GOT_BY_BLUE:
        gw.show_text_centering("DROPPED BY RED", 40)
    elif gameinfo.state == GameInfo.GS_GOT_BY_RED:
        gw.show_text_centering ("DROPPED BY BLUE", 40);
    elif gameinfo.state == GameInfo.GS_SERVICE_BY_BLUE:
        pass # OK
    elif gameinfo.state == GameInfo.GS_SERVICE_BY_RED:
        pass # OK
    elif gameinfo.state == GameInfo.GS_WON_BY_BLUE:
        gw.show_text_centering ("PRESS R TO PLAY AGAIN", 80);          
        gw.show_text_centering ("WON BY BLUE", 60);    
    elif gameinfo.state == GameInfo.GS_WON_BY_RED:
        gw.show_text_centering ("PRESS R TO PLAY AGAIN", 80);          
        gw.show_text_centering ("WON BY RED", 60);    
    else:
        raise RuntimeError("ERROR!! Unknown Game State")
    return False


def cb_key_press_event(widget, event, gameinfo):
    key = gtk.gdk.keyval_name(event.keyval)
    if  key == 'a':
        gameinfo.slime_blue.vx = -1
    elif key == 's':
        gameinfo.slime_blue.vx = 1
    elif key == 'Left':
        gameinfo.slime_red.vx = -1
    elif key == 'Right':
        gameinfo.slime_red.vx = 1
    elif key == 'r':
        if (gameinfo.state == GameInfo.GS_WON_BY_BLUE or
            gameinfo.state == GameInfo.GS_WON_BY_RED):
            gameinfo.state = GameInfo.GS_INIT
    else:
        pass # OK
    return False

def event_loop(gameinfo):
    if gameinfo.window.get_window() is None:
        return False
    
    if gameinfo.state == GameInfo.GS_INIT:
        gameinfo.init()
    elif gameinfo.state == GameInfo.GS_PLAY:
        gameinfo.slime_blue.move()
        gameinfo.slime_red.move()
        gameinfo.collision_ball_slime(Slime.BLUE)
        gameinfo.collision_ball_slime(Slime.RED)
        gameinfo.collision_penalty()
        gameinfo.ball.move()

        if gameinfo.ball.y >= WIN_HEIGHT - BALL_RADIUS:
            if gameinfo.ball.x <= WIN_WIDTH / 2:
                gameinfo.state = GameInfo.GS_GOT_BY_RED
            else:
                gameinfo.state = GameInfo.GS_GOT_BY_BLUE

    elif gameinfo.state == GameInfo.GS_GOT_BY_BLUE:
        gameinfo.score_move(Slime.BLUE)
    elif gameinfo.state == GameInfo.GS_GOT_BY_RED:
        gameinfo.score_move(Slime.RED)
    elif gameinfo.state == GameInfo.GS_SERVICE_BY_BLUE:
        gameinfo.serve_set(Slime.BLUE)
    elif gameinfo.state == GameInfo.GS_SERVICE_BY_RED:
        gameinfo.serve_set(Slime.RED)
    elif gameinfo.state == GameInfo.GS_WON_BY_BLUE:
        pass # OK
    elif gameinfo.state == GameInfo.GS_WON_BY_RED:
        pass # OK
    else:
        raise RuntimeError("ERROR!! Unknown Game State\n")

    gameinfo.window.queue_draw()
    return True

def cb_delete_event(widget, event, timeout):
    if timeout:
        glib.source_remove(timeout)
    return False

def main():
    window = gtk.Window()

    gameinfo = GameInfo()
    gameinfo.slime_blue = Slime(Slime.BLUE)
    gameinfo.slime_red =  Slime(Slime.RED)
    gameinfo.ball = Ball()
    gameinfo.window = window
    gameinfo.init()

    window.set_size_request(WIN_WIDTH, WIN_HEIGHT)
    
    window.set_resizable(False)
    window.set_app_paintable(True)
    timeout = glib.timeout_add(10, event_loop, gameinfo)
    window.connect("expose_event", cb_expose_event, gameinfo)
    window.connect("key_press_event", cb_key_press_event, gameinfo)
    window.connect("delete_event", cb_delete_event, timeout)
    window.connect("destroy", gtk.main_quit)
    window.show_all()
    gtk.main()

if __name__ == "__main__":
    main()
