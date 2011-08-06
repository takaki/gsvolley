import java.util.Random;

public class GameInfo {
	enum GameState {
		GS_INIT, GS_PLAY, GS_GOT_BY_BLUE, GS_GOT_BY_RED, GS_SERVICE_BY_BLUE, GS_SERVICE_BY_RED, GS_WON_BY_BLUE, GS_WON_BY_RED,
	}

	private static final int SCORE_INIT = 50;;

	final private Slime slime_blue = new Slime(Slime.TEAM.BLUE);
	public Slime getSlime_blue() {
		return slime_blue;
	}

	public Slime getSlime_red() {
		return slime_red;
	}

	final private Slime slime_red = new Slime(Slime.TEAM.RED);
	final private Ball ball = new Ball();

	public Ball getBall() {
		return ball;
	}

	private GameState state;

	public GameState getState() {
		return state;
	}

	public void setState(GameState state) {
		this.state = state;
	}

	private int score_blue = SCORE_INIT;
	public int getScore_blue() {
		return score_blue;
	}

	public int getScore_red() {
		return score_red;
	}

	private int score_red = SCORE_INIT;
	private int ball_count = 1;
	public int getBall_count() {
		return ball_count;
	}

	private Slime.TEAM ball_owner = Slime.TEAM.BLUE;
	private double penalty_y;
	private int wait_count = 0;

	public GameInfo() {
		ball.init(Slime.TEAM.BLUE);
		slime_blue.init();
		slime_red.init();
		ball_count = 1;
		// state = GameState.GS_SERVICE_BY_BLUE; // XXX GS_INIT??
		state = GameState.GS_INIT;
	}

	public void score_move(Slime.TEAM win) {
		if (ball_count > 0) {
			if (wait_count > 0) {
				--wait_count;
			} else {
				--ball_count;
				if (ball_count == 0) {
					wait_count = 50; // XXX
				} else {
					wait_count = 7; // XXX
				}
				if (win == Slime.TEAM.BLUE) {
					--score_red;
					if (score_red <= 0) {
						state = GameState.GS_WON_BY_BLUE;
					}
				} else {
					--score_blue;
					if (score_blue <= 0) {
						state = GameState.GS_WON_BY_RED;
					}
				}
			}
		} else {
			if (wait_count > 0) {
				--wait_count;
			} else {
				if (win == Slime.TEAM.BLUE) {
					state = GameState.GS_SERVICE_BY_BLUE;
				} else {
					state = GameState.GS_SERVICE_BY_RED;
				}
			}
		}
	};

	public void serve_set(Slime.TEAM win) {
		if (win == Slime.TEAM.BLUE) {
			ball.init(Slime.TEAM.BLUE);
		} else {
			ball.init(Slime.TEAM.RED);
		}
		slime_blue.init();
		slime_red.init();
		Random r = new Random();

		penalty_y = ((GameWin.WIN_HEIGHT - 40) - 120) * r.nextDouble() + 120;

		ball_count = 1;
		wait_count = 0;
		state = GameState.GS_PLAY;
	}

	private double inner_product(double a[], double b[]) {
		return (a[0] * b[0] + a[1] * b[1]);
	};

	double outer_product(double a[], double b[]) {
		return (a[0] * b[1] - b[0] * a[1]);
	};

	double norm(double a[]) {
		return Math.sqrt(a[0] * a[0] + a[1] * a[1]);
	};

	void collision_ball_slime(Slime.TEAM t) {
		final Ball b = ball;
		final Slime s;
		if (t == Slime.TEAM.BLUE) {
			s = slime_blue;
		} else {
			s = slime_red;
		}
		double v_s[] = { b.getVx(), b.getVy() };
		double v_a[] = { s.getX() - b.getX(), GameWin.WIN_HEIGHT - b.getY() };
		double v_b[] = { s.getX() - (b.getX() + b.getVx()),
				GameWin.WIN_HEIGHT - (b.getY() + b.getVy()) };
		double c = inner_product(v_a, v_s) * inner_product(v_b, v_s);
		double d = Math.abs(outer_product(v_s, v_a)) / norm(v_s);
		double r = Slime.RADIUS + Ball.RADIUS;

		if (d <= r && (c <= 0 || r > norm(v_a) || r > norm(v_b))) {
			double dx_0 = b.getX() - s.getX();
			double dy_0 = b.getY() - GameWin.WIN_HEIGHT;
			double[] v = { dx_0, dy_0 };
			double dist_0 = norm(v);
			double angle = Math.acos(dx_0 / dist_0);
			double cos2x = Math.cos(2 * angle);
			double sin2x = Math.sin(2 * angle);
			double vx = -b.getVx() * cos2x + b.getVy() * sin2x;
			double vy = b.getVx() * sin2x + b.getVy() * cos2x;
			b.setVx(vx);
			b.setVy(vy);

			ball_owner = t;
			ball_count++;
		}
	}

	public void collision_penalty() {
		final Ball b = ball;
		double v[] = { b.getX() - GameWin.WIN_WIDTH / 2, b.getY() - penalty_y };

		if (norm(v) < GameWin.PENALTY_RADIUS + Ball.RADIUS) {
			--wait_count;
			if (ball_owner == Slime.TEAM.BLUE) {
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

	public double getPenalty_y() {
		return penalty_y;
	}

}
