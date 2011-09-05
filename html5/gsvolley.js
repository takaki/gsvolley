onload = function() {
    setInterval('gamewin.event_loop()', 10);
};

var WIN_WIDTH  = 640;
var WIN_HEIGHT = 400;

var GRAVITY           = 0.03;
var SCORE_INIT        = 50;

var BALL_RADIUS       = 10;
var BALL_SERVE_OFFSET = 76.5;
var BALL_Y_INIT = 20;
var PENALTY_RADIUS    = 30;

var SLIME_RADIUS = 40;

var Team = {
    Blue: 0,
    Red:  1
}

var GS =  {
    INIT: 0,
    PLAY: 1,  
    GOT_BY_BLUE: 2,
    GOT_BY_RED: 3,  
    SERVICE_BY_BLUE: 4,
    SERVICE_BY_RED: 5, 
    WON_BY_BLUE: 6,
    WON_BY_RED: 7
}


var Ball = function (team) {
    this.initialize(team);
}


Ball.prototype = {
    initialize : function(team) {
	if (team == Team.Blue) {
	    this.x = BALL_SERVE_OFFSET;
	} else {
	    this.x = WIN_WIDTH - BALL_SERVE_OFFSET;
	}
	this.y = BALL_Y_INIT;
	this.vx = 0;
	this.vy = 0;
    },
    move: function() {
	this.vy += GRAVITY;
	this.x += this.vx;
	this.y += this.vy;
	
	if (this.x < 0 + BALL_RADIUS) {
	    this.x = (BALL_RADIUS - this.x) + BALL_RADIUS;
	    this.vx = - this.vx;
	}
	if (this.x > WIN_WIDTH - BALL_RADIUS) {
	    this.x = (WIN_WIDTH - BALL_RADIUS) - 
		(this.x - (WIN_WIDTH - BALL_RADIUS))
	    this.vx = - this.vx;
	}
    }
};

var Slime = function(team) {
    this.initialize(team);
}
Slime.prototype = {
    initialize : function(team) {
	this.team = team;
	if (this.team == Team.Blue) {
	    this.limit_left = SLIME_RADIUS;
	    this.limit_right = WIN_WIDTH / 2 - SLIME_RADIUS;
	} else {
	    this.limit_left  = WIN_WIDTH / 2 + SLIME_RADIUS;
	    this.limit_right = WIN_WIDTH - SLIME_RADIUS;
	}
	this.reset();
    },
    reset: function() {
	if (this.team == Team.Blue ) {
	    this.x = SLIME_RADIUS;
	} else {
	    this.x = WIN_WIDTH - SLIME_RADIUS;
	}
	this.vx = 0;
    },
    move: function() {
	this.x += this.vx;
	if (this.x < this.limit_left) {
	    this.x = this.limit_left;
	    this.vx = 0;
	}
	if (this.x > this.limit_right) {
	    this.x = this.limit_right;
	    this.vx = 0;
	}
    }

};

var GameInfo = function() {
    this.initialize();
}
GameInfo.prototype = {
    initialize: function() {
	this.slime_blue = new Slime(Team.Blue);
	this.slime_red = new Slime(Team.Red);
	this.ball = new Ball(Team.Blue);
	this.score_blue = SCORE_INIT;
	this.score_red = SCORE_INIT;
	this.ball_owner = Team.Blue;
	this.wait_count = 0;
	
	this.ball_count = 1;
	this.state = GS.INIT;
	this.penalty_y = 0;
    },

    score_move: function(win) {
	if (this.ball_count > 0) {
	    if (this.wait_count > 0) {
		--this.wait_count;
	    } else {
		--this.ball_count;
		if (this.ball_count == 0) {
		    this.wait_count = 50;
		} else {
		    this.wait_count = 7;
		}
		if (win == Team.Blue) {
		    --this.score_red;
		    if (this.score_red < 0) {
			this.state = GS.WON_BY_BLUE;
		    }
		} else {
		    --this.score_blue;
		    if (this.score_blue < 0 ){
			this.state = GS.WON_BY_RED;
		    }
		}
	    }
	} else {
	    if (this.wait_count > 0) {
		--this.wait_count;
	    } else {
		if (win == Team.Blue) {
		    this.state = GS.SERVICE_BY_BLUE;
		} else {
		    this.state = GS.SERVICE_BY_RED;
		}
	    }
	}
    },
    serve_set: function(win) {
	if (win == Team.Blue) {
	    this.ball = new Ball(Team.Blue);
	} else{
	    this.ball = new Ball(Team.Red);
	}
	this.slime_blue.reset();
	this.slime_red.reset();
	this.penalty_y = ((WIN_HEIGHT - 40) - 120) * Math.random() + 120;

	this.ball_count = 1;
	this.wait_count = 0;
	this.state = GS.PLAY;
    },
    collision_ball_slime: function(t) {
	var b = this.ball;
	var s;
	if (t == Team.Blue) {
	    s = this.slime_blue
	} else{
	    s = this.slime_red
	}
	var v_s = new Vector(b.x, b.y);
	var v_a = new Vector(s.x - b.x, WIN_HEIGHT - b.y);
	var v_b = new Vector(s.x - (b.x + b.vx), WIN_HEIGHT - (b.y + b.vy))
	var c = inner_product(v_a, v_s) * inner_product(v_b, v_s)
	var d = Math.abs(outer_product(v_s, v_a)) / norm(v_s)
	var r = SLIME_RADIUS + BALL_RADIUS
	if (d <= r && (c <=0 ||  r > norm(v_a) || r > norm(v_b))){
	    var dx_0 = b.x - s.x
	    var dy_0 = b.y - WIN_HEIGHT
	    var v = new Vector(dx_0, dy_0)
	    var dist_0 = norm(v)
	    var angle = Math.acos(dx_0/dist_0)
	    var cos2x = Math.cos(2*angle)
	    var sin2x = Math.sin(2*angle)
	    var vx = - b.vx * cos2x + b.vy * sin2x
	    var vy =   b.vx * sin2x + b.vy * cos2x
	    this.ball.vx = vx
	    this.ball.vy = vy
            this.ball_owner = t
	    this.ball_count += 1
	}
    },

    collision_penalty: function() {
	var b = this.ball
	var v = new Vector(b.x - WIN_WIDTH / 2, b.y - this.penalty_y)
	if (norm(v) < PENALTY_RADIUS + BALL_RADIUS) {
	    this.wait_count -= 1
	    if ( this.ball_owner == Team.Blue) {
		if ( this.score_blue > 1 && this.wait_count <= 0 ) {
		    this.score_blue -= 1
		    this.wait_count = 4
		}
	    } else {
		if (this.score_red > 1 && this.wait_count <= 0) {
		    this.score_red -= 1
		    this.wait_count = 4
		}
	    }
        }
    }
}

var Vector = function(x,y) {
    this.x = x;
    this.y = y;
}

var inner_product = function (a, b) {
    return a.x * b.x + a.y*b.y;
}
  
var outer_product = function(a,b) {
    return a.x * b.y - b.x * a.y;
}

var norm = function(a) {
    return Math.sqrt(inner_product(a,a))
}

var GameWin = function() {
    this.initialize();
}


GameWin.prototype = {
    initialize: function() {
	this.gameinfo = new GameInfo();
    },
    draw: function () {
	var draw_back  = function() {
	    ctx.fillStyle = "black";
	    ctx.fillRect(0,0,WIN_WIDTH, WIN_HEIGHT);
	    
	    ctx.fillStyle = "white";
	    ctx.fillRect(WIN_WIDTH/2-1, 120, 2, WIN_HEIGHT);
	}
	var draw_slime = function(s) {
	    if (s.team == Team.Blue ) {
		ctx.fillStyle = "blue";
	    } else {
		ctx.fillStyle = "red";
	    }
	    ctx.beginPath();
	    ctx.arc(s.x, WIN_HEIGHT, SLIME_RADIUS, Math.PI, 0, false)
	    ctx.closePath();
	    ctx.fill();
	}
	var draw_ball = function(b) {
	    ctx.fillStyle = "white";
	    ctx.beginPath();
	    ctx.arc(b.x, b.y, BALL_RADIUS, 
		    0, Math.PI * 2, false);
	    ctx.fill();
	    ctx.closePath();
	    ctx.stroke();
	}
	var draw_penalty = function(y) {
	    ctx.save();
	    ctx.lineWidth = 5.0;
	    ctx.strokeStyle = "green";

	    ctx.beginPath();
	    ctx.arc(WIN_WIDTH/2, y, PENALTY_RADIUS, 0, 2 * Math.PI)
	    ctx.stroke();
	    ctx.closePath();

	    ctx.beginPath();
	    ctx.arc(WIN_WIDTH/2, y, PENALTY_RADIUS - 15, 0, 2 * Math.PI)
	    ctx.stroke();
	    ctx.closePath();

	    ctx.restore();

	}
	var draw_score = function(blue, count, red) {
	    var buf = "Blue: " + blue + " Ball: " + count + " Red: " + red;
	    show_text_centering(buf, 20);
	}
	var show_text_centering = function(text, y) {
	    ctx.font = "24px 'Times New Roman'";
	    ctx.fillText(text,100, y);
	}


	/* canvas要素のノードオブジェクト */
	var canvas = document.getElementById('canvassample');
	/* canvas要素の存在チェックとCanvas未対応ブラウザの対処 */
	if ( ! canvas || ! canvas.getContext ) {
	    return false;
	}
	var ctx = canvas.getContext('2d');

	draw_back();
	draw_slime(this.gameinfo.slime_blue);
	draw_slime(this.gameinfo.slime_red);
	draw_ball(this.gameinfo.ball);
	draw_penalty(this.gameinfo.penalty_y);
	draw_score(this.gameinfo.score_blue, this.gameinfo.ball_count,
		   this.gameinfo.score_red);
	switch(this.gameinfo.state) {
	case GS.INIT:
	    break;
	case GS.PLAY:
	    break;
	case GS.GOT_BY_BLUE:
            show_text_centering("DROPPED BY RED", 40)
	    break;
	case GS.GOT_BY_RED:
            show_text_centering("DROPPED BY BLUE", 40)
	    break;
	case GS.SERVICE_BY_BLUE:
	    break;
	case GS.SERVICE_BY_RED:
	    break;
	case GS.WON_BY_BLUE:
            show_text_centering("PRESS R TO PLAY AGAIN", 80);          
            show_text_centering("WON BY BLUE", 60);    
	    break;
	case GS.WON_BY_RED:
            show_text_centering("PRESS R TO PLAY AGAIN", 80);          
            show_text_centering("WON BY RED", 60);    
	    break;
	default:
            alert("ERROR!! Unknown Game State / " + this.gameinfo.state)
	}
    },
    event_loop: function() {
	switch(this.gameinfo.state) {
	case GS.INIT:
            this.gameinfo.state = GS.SERVICE_BY_BLUE
	    break;
	case GS.PLAY:
            this.gameinfo.slime_blue.move();
            this.gameinfo.slime_red.move();
	    this.gameinfo.collision_ball_slime(Team.Blue);
            this.gameinfo.collision_ball_slime(Team.Red);
            this.gameinfo.collision_penalty();
            this.gameinfo.ball.move();
            if (this.gameinfo.ball.y >= WIN_HEIGHT - BALL_RADIUS) {
		if (this.gameinfo.ball.x <= WIN_WIDTH / 2) {
		    this.gameinfo.state = GS.GOT_BY_RED;
		} else {
		    this.gameinfo.state = GS.GOT_BY_BLUE;
		}
	    }
	    break;
	case GS.GOT_BY_BLUE:
            this.gameinfo.score_move(Team.Blue)
	    break;
	case GS.GOT_BY_RED:
            this.gameinfo.score_move(Team.Red)
	    break;
	case GS.SERVICE_BY_BLUE:
            this.gameinfo.serve_set(Team.Blue)
	    break;
	case GS.SERVICE_BY_RED:
            this.gameinfo.serve_set(Team.Red)
	    break;
	case GS.WON_BY_BLUE:
	    // 
	    break;
	case GS.WON_BY_RED:
	    // 
	    break;
	default:
            alert("ERROR!! Unknown Game State / " + this.gameinfo.state)
	}
	this.draw();
    }
}
	
$(window).keydown(function(e){
    if (e.keyCode == 65) {
	gamewin.gameinfo.slime_blue.vx = -1 ;
	return false
    } else if (e.keyCode == 83) {
	gamewin.gameinfo.slime_blue.vx = 1;
	return false
    } else if (e.keyCode == 37) {
	gamewin.gameinfo.slime_red.vx = -1;
	return false
    } else if (e.keyCode == 39) {
	gamewin.gameinfo.slime_red.vx = 1;
	return false
    } else if (e.keyCode == 82) {
	if (gamewin.gameinfo.state == GS.WON_BY_BLUE ||
	    gamewin.gameinfo.state == GS.WON_BY_RED) {
	    gamewin.gameinfo = new GameInfo();
	    
	}
	return true;
    }
    return true;
});

var gamewin = new GameWin();
