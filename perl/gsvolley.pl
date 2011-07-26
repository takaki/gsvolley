#!/usr/bin/perl

use strict ;
use Gtk2;
use Cairo;
use Math::Trig;
use Gtk2::Gdk::Keysyms;

my ($BLUE, $RED) = (0..1);
my ($GS_INIT,
    $GS_PLAY,        
    $GS_GOT_BY_BLUE,
    $GS_GOT_BY_RED,
    $GS_SERVICE_BY_BLUE,
    $GS_SERVICE_BY_RED,
    $GS_WON_BY_BLUE,
    $GS_WON_BY_RED) = (0..10);



my $WIN_WIDTH   = 640;
my $WIN_HEIGHT  = 400;
my $GRAVITY     = 0.03;
my $SCORE_INIT  = 50;

my $BALL_RADIUS       = 10;
my $BALL_SERVE_OFFSET = 76.5;
my $BALL_Y_INIT       = 20;

my $PENALTY_RADIUS    = 30;
    
my $SLIME_RADIUS = 40 ;


package Ball;

sub new {
    my $class = shift;
    my ($team) = @_;
    my $self = {"team"=>$team};
    my $x;
    if ($team == $BLUE) {
	$self->{x} = $BALL_SERVE_OFFSET;
    } else {
	$self->{x} = $WIN_WIDTH - $BALL_SERVE_OFFSET;
    }
    $self->{y} = $BALL_Y_INIT;
    $self->{vx} = 0;
    $self->{vy} = 0;

    bless $self, $class;
    return $self;
}

sub move {
    my $self = shift;
    $self->{vy} += $GRAVITY;
    $self->{x} += $self->{vx};
    $self->{y} += $self->{vy};

    if ($self->{x} < 0 + $BALL_RADIUS ) {
	$self->{x} = ($BALL_RADIUS - $self->{x}) + $BALL_RADIUS;
	$self->{vx} = - $self->{vx};
    }
    if ($self->{x} > $WIN_WIDTH - $BALL_RADIUS ) {
	$self->{x} = ($WIN_WIDTH - $BALL_RADIUS) - 
	    ($self->{x} - ($WIN_WIDTH - $BALL_RADIUS));
	$self->{vx} = - $self->{vx};
    }
    
}

package Slime;
sub new {
    my $class = shift;
    my ($team) = @_;
    my ($limit_left, $limit_right);

    if ($team == $BLUE) {
	$limit_left = $SLIME_RADIUS;
	$limit_right = $WIN_WIDTH / 2 - $SLIME_RADIUS;
    } else {
	$limit_left = $WIN_WIDTH / 2 + $SLIME_RADIUS;
	$limit_right = $WIN_WIDTH  - $SLIME_RADIUS;
    }
    
    my $self = {
	"team" => $team,
	"limit_left" => $limit_left,
	"limit_right" => $limit_right,
	"x" => undef,
	"vx" => undef,
    };


    bless $self, $class;
    $self->init;
    return $self;
}
    
sub init {
    my $self = shift;
    if ($self->{team} == $BLUE) {
	$self->{x} = $SLIME_RADIUS;
    } else {
	$self->{x} = $WIN_WIDTH - $SLIME_RADIUS;
    }
    $self->{vx} = 0
}

sub move {
    my $self = shift;
    $self->{x} += $self->{vx};
    if ($self->{x} < $self->{limit_left}) {
	$self->{x} = $self->{limit_left};
	$self->{vx} = 0;
    } 
    if ($self->{x} > $self->{limit_right}) {
	$self->{x} = $self->{limit_right};
	$self->{vx} = 0;
    }


}

package GameInfo;

sub new {
    my $class = shift;
    my $self = {};
    $self->{slime_blue} = Slime->new($BLUE);
    $self->{slime_red} = Slime->new($RED);
    $self->{ball} = Ball->new($BLUE);
    $self->{score_blue} = $SCORE_INIT;
    $self->{score_red} = $SCORE_INIT;
    $self->{ball_owner} = $BLUE;
    $self->{wait_count} = 0;
    $self->{ball_count} = 1;
    $self->{state} = $GS_INIT;
    $self->{penalty_y} = 0;

    bless $self, $class;
    return $self;
}

sub score_move {
    my $self = shift;
    my ($win) = @_;
    if ($self->{ball_count} > 0) {
	if ($self->{wait_count} > 0) {
	    --$self->{wait_count};
	} else {
	    --$self->{ball_count};
	    if ($self->{ball_count} == 0) {
		$self->{wait_count} = 50;
	    } else {
		$self->{wait_count} = 7;
	    }
	    if ($win == $BLUE) {
		--$self->{score_red};
		if ($self->{score_red} < 0) {
		    $self->{state} = $GS_WON_BY_BLUE;
		}
	    } else {
		--$self->{score_blue};
		if ($self->{score_blue} < 0) {
		    $self->{state} = $GS_WON_BY_RED;
		}
	    }
	}
    } else {
	if ($self->{wait_count} > 0) {
	    --$self->{wait_count};
	} else {
	    if ($win == $BLUE) {
		$self->{state} = $GS_SERVICE_BY_BLUE;
	    } else {
		$self->{state} = $GS_SERVICE_BY_RED;
	    }
	}
    }
}
sub serve_set {
    my $self = shift;
    my ($win) = @_;
    if ($win == $BLUE) {
	$self->{ball} = Ball->new($BLUE);
    } else {
	$self->{ball} = Ball->new($RED);
    }
    $self->{slime_blue}->init;
    $self->{slime_red}->init;
    $self->{penalty_y} = (($WIN_HEIGHT - 40) - 120) * rand() + 120;
    
    $self->{ball_count} = 1;
    $self->{wait_count} = 0;
    $self->{state} = $GS_PLAY;
}

sub inner_product {
    my ($a, $b) = @_;
    return $a->{x} * $b->{x} + $a->{y} * $b->{y};
}
sub outer_product {
    my ($a, $b) = @_;
    return $a->{x} * $b->{y} - $b->{x} * $a->{y};
}

sub norm {
    my ($a) = @_;
    return sqrt(inner_product($a, $a));
}

sub collision_ball_slime {
    my $self = shift;
    my ($t) = @_;
    my $b = $self->{ball};
    my $s = undef;
    if ($t == $BLUE) {
	$s = $self->{slime_blue};
    } else {
	$s = $self->{slime_red};
    }
    my $v_s = { "x" => $b->{x}, 
		"y" => $b->{y} };
    my $v_a = { "x" => $s->{x} - $b->{x},
		"y" => $WIN_HEIGHT - $b->{y}};
    my $v_b = { "x" =>     $s->{x} - ($b->{x} + $b->{vx}),
		"y" => $WIN_HEIGHT - ($b->{y} + $b->{vy})};
    my $c = inner_product($v_a, $v_s) * inner_product($v_b, $v_s);
    my $d = abs(outer_product($v_s, $v_a)) / norm($v_s);
    my $r = $SLIME_RADIUS + $BALL_RADIUS;
    if ($d <= $r and ($c <= 0 or $r > norm($v_a) or $r > norm($v_b))){
	my $dx_0 = $b->{x} - $s->{x};
	my $dy_0 = $b->{y} - $WIN_HEIGHT;
	my $v = {"x" => $dx_0,  "y" => $dy_0};
	my $dist_0 = norm($v);
	my $angle = Math::Trig::acos($dx_0/$dist_0);
	my $cos2x = cos(2*$angle);
	my $sin2x = sin(2*$angle);
	my $vx = - $b->{vx} * $cos2x + $b->{vy} * $sin2x;
	my $vy =   $b->{vx} * $sin2x + $b->{vy} * $cos2x;
	$b->{vx} = $vx;
	$b->{vy} = $vy;

	$self->{ball_owner} = $t;
	++$self->{ball_count};
    }
}

sub collision_penalty {
    my $self = shift;
    my $b = $self->{ball};
    my $v = {"x" => $b->{x} - $WIN_WIDTH / 2,
	     "y" => $b->{y} - $self->{penalty_y}};
    if (norm($v) < $PENALTY_RADIUS + $BALL_RADIUS) {
	--$self->{wait_count};
	if ($self->{ball_owner} == $BLUE) {
	    if ($self->{score_blue} > 1 and $self->{wait_count} <= 0 ) {
		--$self->{score_blue};
		$self->{wait_count} = 4;
	    }
	} else {
	    if ($self->{score_red} > 1 and $self->{wait_count} <= 0 ) {
		--$self->{score_red};
		$self->{wait_count} = 4;
	    }
	}
    }
}

package GameWin;
use base ("Gtk2::Window");

sub new {
    my $class = shift;
    my $self = Gtk2::Window->new;

    $self->{gameinfo} = GameInfo->new;
    $self->{pattern_blue}    = Cairo::SolidPattern->create_rgb(0,0,1);
    $self->{pattern_red}     = Cairo::SolidPattern->create_rgb(1,0,0);
    $self->{pattern_ball}    = Cairo::SolidPattern->create_rgb(1,1,1);
    $self->{pattern_penalty} = Cairo::SolidPattern->create_rgb(0,1,0);

    $self->set_title("gsvolley");
    $self->set_size_request($WIN_WIDTH, $WIN_HEIGHT);
    $self->set_resizable(0);
    $self->set_app_paintable(1);

    $self->signal_connect(expose_event => sub  {
	my ($widget, $event) = @_;
	# my $cr = Gtk2::Gdk::Cairo::Context->create($widget->window());
	$self->{cr} = Gtk2::Gdk::Cairo::Context->create($widget->window());
	$self->{cr}->set_source_rgb(0,0,0);
	$self->{cr}->set_font_size(16);
	
	sub draw_back {
	    $self->{cr}->set_source_rgb(0,0,0);
	    $self->{cr}->rectangle(0,0,$WIN_WIDTH, $WIN_HEIGHT);
	    $self->{cr}->fill;

	    $self->{cr}->set_source_rgb(1,1,1);
	    $self->{cr}->rectangle($WIN_WIDTH/2-1, 120, 2, $WIN_HEIGHT);
	    $self->{cr}->fill;
	};
	sub draw_slime {
	    my ($s) = @_;
	    if ($s->{team} == $BLUE) {
		$self->{cr}->set_source($self->{pattern_blue});
	    } else {
		$self->{cr}->set_source($self->{pattern_red});
	    }
	    $self->{cr}->move_to($s->{x}, $WIN_HEIGHT);
	    $self->{cr}->arc($s->{x}, $WIN_HEIGHT, $SLIME_RADIUS,
			     Math::Trig::pi, 0);
	    $self->{cr}->fill;
	}

	sub draw_penalty {
	    my ($y) = @_;
	    $self->{cr}->set_line_width(5);
	    $self->{cr}->set_source($self->{pattern_penalty});
	    $self->{cr}->arc($WIN_WIDTH/2, $y, $PENALTY_RADIUS, 
			     0, 2 * Math::Trig::pi);
	    $self->{cr}->stroke_preserve;
	    
	    $self->{cr}->set_source_rgb(0,0,0);
	    $self->{cr}->fill;
	    
	    $self->{cr}->set_source($self->{pattern_penalty});
	    $self->{cr}->arc($WIN_WIDTH/2, $y, $PENALTY_RADIUS - 15, 
			     0, 2 * Math::Trig::pi);
	    $self->{cr}->set_line_width(5);
	    $self->{cr}->stroke_preserve;
	    
	    $self->{cr}->set_source_rgb(0,0,0);
	    $self->{cr}->fill;
	}

	sub draw_ball {
	    my ($b) = @_;
	    $self->{cr}->set_source($self->{pattern_ball});
	    $self->{cr}->arc($b->{x}, $b->{y}, $BALL_RADIUS, 0, 
			     2 * Math::Trig::pi);
	    $self->{cr}->fill;
	}

	sub draw_score {
	    my ($blue, $count, $red) = @_;
	    my $buf = sprintf "Blue: %2d Ball: %3d Red: %2d", 
	    $blue, $count, $red;
	    show_text_centering($buf, 20);
	}
	sub show_text_centering {
	    my ($text, $y) = @_;
	    my $te = $self->{cr}->text_extents($text);
	    $self->{cr}->move_to(($WIN_WIDTH - $te->{width})/2, $y);
	    $self->{cr}->show_text($text);
	}

	draw_back;
	draw_slime($self->{gameinfo}->{slime_blue});
	draw_slime($self->{gameinfo}->{slime_red});
	draw_penalty($self->{gameinfo}->{penalty_y});
	draw_ball($self->{gameinfo}->{ball});
	draw_score($self->{gameinfo}->{score_blue},
		   $self->{gameinfo}->{ball_count},
		   $self->{gameinfo}->{score_red});
	
	my $state = $self->{gameinfo}->{state};
	if ($state == $GS_INIT) {
	    ;
	} elsif ($state == $GS_PLAY) {
	    ;
	} elsif ($state == $GS_GOT_BY_BLUE) {
	    show_text_centering("DROPPED BY RED", 40);
	} elsif ($state == $GS_GOT_BY_RED) {
	    show_text_centering("DROPPED BY BLUE", 40);
	} elsif ($state == $GS_SERVICE_BY_BLUE) {
	} elsif ($state == $GS_SERVICE_BY_RED) {
	} elsif ($state == $GS_WON_BY_BLUE) {
	    show_text_centering("PRESS R TO PLAY AGAIN", 80);
	    show_text_centering("WON BY BLUE", 60);    
	} elsif ($state == $GS_WON_BY_RED) {
	    show_text_centering("PRESS R TO PLAY AGAIN", 80);          
	    show_text_centering("WON BY RED", 60);    
	} else {
	    die "ERROR!! Unknown Game State / " + $state;
	}
			  });

    $self->signal_connect(key_press_event => sub {
	my ($widget, $ev) = @_;
	my $keyval = $ev->keyval;
	if ($keyval == $Gtk2::Gdk::Keysyms{a}) {
	    $self->{gameinfo}->{slime_blue}->{vx} = -1;
	} elsif ($keyval == $Gtk2::Gdk::Keysyms{s}) {
	    $self->{gameinfo}->{slime_blue}->{vx} = 1;
	} elsif ($keyval == $Gtk2::Gdk::Keysyms{Left}) {
	    $self->{gameinfo}->{slime_red}->{vx} = -1;
	} elsif ($keyval == $Gtk2::Gdk::Keysyms{Right}) {
	    $self->{gameinfo}->{slime_red}->{vx} = 1;
	} elsif ($keyval == $Gtk2::Gdk::Keysyms{Escape}) {
	    Gtk2->main_quit;
	} elsif ($keyval == $Gtk2::Gdk::Keysyms{r}) {
	    if ($self->{gameinfo}->{state} == $GS_WON_BY_BLUE or
		$self->{gameinfo}->{state} == $GS_WON_BY_RED) {
		$self->{gameinfo} = GameInfo->new
	    }
	}
			  });

    $self->signal_connect(delete_event => sub {
	Gtk2->main_quit;
			  });
    $self->signal_connect(destroy => sub {
	Gtk2->main_quit;
			  });

    Glib::Timeout->add(10, sub {
	my $state = $self->{gameinfo}->{state};
	if ($state == $GS_INIT) {
	    $self->{gameinfo}->{state} = $GS_SERVICE_BY_BLUE;
	} elsif ($state == $GS_PLAY) {
	    $self->{gameinfo}->{slime_blue}->move;
	    $self->{gameinfo}->{slime_red}->move;
	    $self->{gameinfo}->collision_ball_slime($BLUE);
	    $self->{gameinfo}->collision_ball_slime($RED);
	    $self->{gameinfo}->collision_penalty;
	    $self->{gameinfo}->{ball}->move;
	    if ($self->{gameinfo}->{ball}->{y} >= $WIN_HEIGHT - $BALL_RADIUS) {
		if ($self->{gameinfo}->{ball}->{x} <= $WIN_WIDTH / 2) {
		    $self->{gameinfo}->{state} = $GS_GOT_BY_RED;
		} else { 
		    $self->{gameinfo}->{state} = $GS_GOT_BY_BLUE;
		}
	    }
	} elsif ($state == $GS_GOT_BY_BLUE) {
	    $self->{gameinfo}->score_move($BLUE);
	} elsif ($state == $GS_GOT_BY_RED) {
	    $self->{gameinfo}->score_move($RED);
	} elsif ($state == $GS_SERVICE_BY_BLUE) {
	    $self->{gameinfo}->serve_set($BLUE);
	} elsif ($state == $GS_SERVICE_BY_RED) {
	    $self->{gameinfo}->serve_set($RED);
	} elsif ($state == $GS_WON_BY_BLUE) {
	    #
	} elsif ($state == $GS_WON_BY_RED) {
	    # 
	} else {
	    die "ERROR!! Unknown Game State / " + $self->{gameinfo};
	}
	$self->queue_draw;
	1;
		       });
 

    return bless $self;
}



package main;


my $b = new Ball "blue";

Gtk2::init;
my $window = GameWin->new;
$window->show_all;
Gtk2->main();


