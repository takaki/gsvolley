#!/usr/bin/ruby

require 'gtk2'
require 'glib2'
require 'cairo'

WIN_WIDTH  = 640
WIN_HEIGHT = 400

GRAVITY           = 0.03
SCORE_INIT        = 50

BALL_RADIUS       = 10
BALL_SERVE_OFFSET = 76.5
BALL_Y_INIT = 20
PENALTY_RADIUS    = 30

SLIME_RADIUS = 40


class Ball
  attr_reader :x, :y, :vx, :vy
  attr_writer :vx, :vy
  def initialize(team)
    if team == :blue
      @x = BALL_SERVE_OFFSET
    else
      @x = WIN_WIDTH - BALL_SERVE_OFFSET
    end
    @y = BALL_Y_INIT
    @vx = 0
    @vy = 0
  end
  
  def move
    @vy += GRAVITY
    @x += @vx
    @y += @vy
    
    if @x < 0 + BALL_RADIUS
      @x = (BALL_RADIUS - @x) + BALL_RADIUS
      @vx = -@vx
    end
    if @x > WIN_WIDTH - BALL_RADIUS
      @x = WIN_WIDTH - BALL_RADIUS - 
        (@x - (WIN_WIDTH - BALL_RADIUS))
      @vx = -@vx
    end
  end
end

class Slime
  attr_reader :team, :x
  attr_writer :vx
  def initialize(t)
    @team = t
    if @team == :blue
      @limit_left = SLIME_RADIUS
      @limit_right = WIN_WIDTH / 2 - SLIME_RADIUS
    else
      @limit_left = WIN_WIDTH / 2 + SLIME_RADIUS
      @limit_right = WIN_WIDTH  - SLIME_RADIUS
    end
    init
  end
  def init
    if @team == :blue
      @x = SLIME_RADIUS
    else
      @x = WIN_WIDTH - SLIME_RADIUS
    end
    @vx = 0
  end

  def move
    @x += @vx
    if @x < @limit_left 
      @x = @limit_left
      @vx = 0
    end
    if @x > @limit_right
      @x = @limit_right
      @vx = 0
    end
  end
end

class GameInfo
  attr_reader :slime_blue, :slime_red, :penalty_y, :state, :ball,
  :score_blue, :score_red, :ball_count
  attr_writer :state
  def initialize
    @slime_blue = Slime.new(:blue)
    @slime_red = Slime.new(:red)
    @ball = Ball.new(:blue)
    @score_blue = SCORE_INIT
    @score_red = SCORE_INIT
    @ball_owner = :blue
    @wait_count = 0
    
    # @slime_blue.init
    # @slime_red.init
    @ball_count = 1
    @state = :gs_init
    @penalty_y = 0
  end

  def score_move(win)
    if @ball_count > 0 
      if @wait_count > 0
        @wait_count -= 1
      else
        @ball_count -= 1
        if @ball_count == 0
          @wait_count = 50
        else
          @wait_count = 7
        end
        if win == :blue
          @score_red -= 1
          if @score_red < 0 
            @state = :gs_won_by_blue
          end
        else
          @score_blue -= 1
          if @score_blue < 0 
            @state = :gs_won_by_red
          end
        end
      end
    else
      if @wait_count > 0 
        @wait_count -= 1
      else
        if win == :blue
          @state = :gs_service_by_blue
        else
          @state = :gs_service_by_red
        end
      end
    end
  end

  def serve_set(win) 
    if win == :blue
      @ball = Ball.new(:blue)
    else
      @ball = Ball.new(:red)
    end
    @slime_blue.init
    @slime_red.init
    @penalty_y = ((WIN_HEIGHT - 40) - 120) * rand() + 120

    @ball_count = 1
    @wait_count = 0
    @state = :gs_play
  end
  
  def inner_product(a, b)
    return a[0] * b[0] + a[1]*b[1]
  end
  
  def outer_product(a,b)
    return a[0] * b[1] - b[0] * a[1]
  end

  def norm(a)
    return Math::sqrt(inner_product(a,a))
  end
  
  def collision_ball_slime(t)
    b = @ball
    if t == :blue
      s = @slime_blue
    else
      s = @slime_red
    end
    v_s = [b.x, b.y]
    v_a = [s.x - b.x, WIN_HEIGHT - b.y]
    v_b = [s.x - (b.x + b.vx), WIN_HEIGHT - (b.y + b.vy)]
    c = inner_product(v_a, v_s) * inner_product(v_b, v_s)
    d = outer_product(v_s, v_a).abs / norm(v_s)
    r = SLIME_RADIUS + BALL_RADIUS

    if d <= r and (c <=0 or r > norm(v_a) or r > norm(v_b))
      dx_0 = b.x - s.x
      dy_0 = b.y - WIN_HEIGHT
      v = [dx_0, dy_0]
      dist_0 = norm(v)
      angle = Math.acos(dx_0/dist_0)
      cos2x = Math.cos(2*angle)
      sin2x = Math.sin(2*angle)
      vx = - b.vx * cos2x + b.vy * sin2x
      vy =   b.vx * sin2x + b.vy * cos2x
      b.vx = vx
      b.vy = vy
      
      @ball_owner = t
      @ball_count += 1
    end
  end
  
  def collision_penalty
    b = @ball
    v = [b.x - WIN_WIDTH / 2, b.y - @penalty_y]
    if norm(v) < PENALTY_RADIUS + BALL_RADIUS
      @wait_count -= 1
      if @ball_owner == :blue
        if @score_blue > 1 and @wait_count <= 0 
          @score_blue -= 1
          @wait_count = 4
        end
      else
        if @score_red > 1 and @wait_count <= 0
          @score_red -= 1
          @wait_count = 4

        end
        
      end
    end
  end
end      
        
    
class GameWin < Gtk::Window
  def initialize
    super()
    @gameinfo = GameInfo.new
    @pattern_blue = Cairo::SolidPattern.new(0,0,1,1)
    @pattern_red = Cairo::SolidPattern.new(1,0,0,1)
    @pattern_ball = Cairo::SolidPattern.new(1,1,1,1)
    @pattern_penalty = Cairo::SolidPattern.new(0,1,0,1)

    set_size_request(WIN_WIDTH, WIN_HEIGHT)
    set_resizable(false)
    set_app_paintable(true)

    signal_connect("expose_event") do |widget, event|
      #  cb_expose_event
      def draw_back
        @cr.set_source(0,0,0)
        @cr.rectangle(0,0, WIN_WIDTH, WIN_HEIGHT);
        @cr.fill

        @cr.set_source(1,1,1)
        @cr.rectangle(WIN_WIDTH/2-1, 120, 2, WIN_HEIGHT);
        @cr.fill
      end

      def draw_slime(s)
        if s.team == :blue
          @cr.set_source(@pattern_blue)
        else
          @cr.set_source(@pattern_red)
        end
        @cr.move_to(s.x, WIN_HEIGHT)
        @cr.arc(s.x, WIN_HEIGHT, SLIME_RADIUS, Math::PI, 0)
        @cr.fill
      end
      
      def draw_ball(b)
        @cr.set_source(@pattern_ball)
        @cr.arc(b.x, b.y, BALL_RADIUS, 0, 2 * Math::PI)
        @cr.fill
      end
      
      def draw_penalty(y)
        @cr.set_line_width(5.0)
        @cr.set_source(@pattern_penalty)
        @cr.arc(WIN_WIDTH/2, y, PENALTY_RADIUS, 0, 2 * Math::PI)
        @cr.stroke_preserve

        @cr.set_source_rgb(0,0,0)
        @cr.fill

        @cr.set_source(@pattern_penalty)
        @cr.arc(WIN_WIDTH/2, y, PENALTY_RADIUS - 15, 0, 2 * Math::PI)
        @cr.set_line_width(5)
        @cr.stroke_preserve

        @cr.set_source_rgb(0,0,0)
        @cr.fill
      end
      
      def draw_score(blue, count,red)
        buf = "Blue: %2d Ball: %3d Red: %2d" % [blue, count,red]
        show_text_centering(buf, 20)
      end
      
      def show_text_centering(text, y)
        te = @cr.text_extents(text)
        @cr.move_to((WIN_WIDTH-te.width)/2, y)
        @cr.show_text(text)
      end

      @cr = widget.window.create_cairo_context
      @cr.set_font_size(16)

      draw_back
      draw_slime(@gameinfo.slime_blue)
      draw_slime(@gameinfo.slime_red)
      draw_penalty(@gameinfo.penalty_y)
      draw_ball(@gameinfo.ball)
      draw_score(@gameinfo.score_blue, @gameinfo.ball_count, @gameinfo.score_red)
      case @gameinfo.state
      when :gs_init
        #
      when :gs_play
        # 
      when :gs_got_by_blue
        show_text_centering("DROPPED BY RED", 40)
      when :gs_got_by_red
        show_text_centering("DROPPED BY BLUE", 40)
      when :gs_service_by_blue
        #
      when :gs_service_by_red
        # 
      when :gs_won_by_blue
        show_text_centering("PRESS R TO PLAY AGAIN", 80);          
        show_text_centering("WON BY BLUE", 60);    
      when :gs_won_by_red
        show_text_centering("PRESS R TO PLAY AGAIN", 80);          
        show_text_centering("WON BY RED", 60);    
      else
        raise "ERROR!! Unknown Game State / " + @gameinfo.state
      end
      false
    end
    signal_connect("key_press_event") do |widget, ev|
      # cb_key_press_event
      case ev.keyval
      when Gdk::Keyval::GDK_KEY_a
        @gameinfo.slime_blue.vx = -1
      when Gdk::Keyval::GDK_KEY_s
        @gameinfo.slime_blue.vx = 1
      when Gdk::Keyval::GDK_KEY_Left
        @gameinfo.slime_red.vx = -1
      when Gdk::Keyval::GDK_KEY_Right
        @gameinfo.slime_red.vx = 1
      when Gdk::Keyval::GDK_KEY_Escape
        Gtk::main_quit
      when Gdk::Keyval::GDK_KEY_r
        if @gameinfo.state == :gs_won_by_blue or
            @gameinfo.state == :gs_won_by_red
          @gameinfo = GameInfo.new
        end
      else
      end
      false
    end
    signal_connect("delete_event") do
      Gtk::main_quit
    end
    signal_connect("destroy") do 
      Gtk::main_quit()
    end
    show_all()

    GLib::Timeout.add(10) do
      case @gameinfo.state
      when :gs_init
        @gameinfo.state = :gs_service_by_blue
      when :gs_play
        @gameinfo.slime_blue.move
        @gameinfo.slime_red.move()
        @gameinfo.collision_ball_slime(:blue)
        @gameinfo.collision_ball_slime(:red)
        @gameinfo.collision_penalty()
        @gameinfo.ball.move()
        if @gameinfo.ball.y >= WIN_HEIGHT - BALL_RADIUS
          if @gameinfo.ball.x <= WIN_WIDTH / 2
            @gameinfo.state = :gs_got_by_red
          else
            @gameinfo.state = :gs_got_by_blue
          end
        end
      when :gs_got_by_blue
        @gameinfo.score_move(:blue)
      when :gs_got_by_red
        @gameinfo.score_move(:red)
      when :gs_service_by_blue
        @gameinfo.serve_set(:blue)
      when :gs_service_by_red
        @gameinfo.serve_set(:red)
      when :gs_won_by_blue
        #
      when :gs_won_by_red
        # 
      else
        raise "ERROR!! Unknown Game State / " + @gameinfo.state.to_s
      end
      
      self.queue_draw()
      true
    end

  end
end

def main
  Gtk::init()
  window = GameWin.new()
  Gtk::main()
  return true
end

main()

