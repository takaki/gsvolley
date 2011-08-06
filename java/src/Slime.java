
public class Slime {
	enum TEAM {BLUE, RED};
	static final int RADIUS = 40;
	final private TEAM team;
	public TEAM getTeam() {
		return team;
	}

	final private int limit_left;
	final private int limit_right;
	private int x;
	public int getX() {
		return x;
	}

	private int vx;
	public int getVx() {
		return vx;
	}
	public void setVx(int vx) {
		this.vx = vx;
	}
	Slime(TEAM t) {
		team = t;
		if (this.team == TEAM.BLUE) {
			limit_left = RADIUS;
			limit_right = GameWin.WIN_WIDTH / 2 - RADIUS;
			} else {
				limit_left = GameWin.WIN_WIDTH / 2 + RADIUS;
				limit_right = GameWin.WIN_WIDTH - RADIUS;
			}
		init();
	}
	public void init() {
		if (team == TEAM.BLUE) {
			x = RADIUS;
			vx = 0;
		} else {
			x = GameWin.WIN_WIDTH - RADIUS;
			vx = 0;
		}
	}
	
	public void move() {
		x += vx;
		if ( x < limit_left ) {
			x = limit_left;
			vx = 0;
		}
		if ( x > limit_right) {
			x = limit_right;
			vx = 0;
		}
	}
	
}
