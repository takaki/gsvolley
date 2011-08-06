

public class Ball {
	private static final double SERVE_OFFSET = 76.5;
	private static final double Y_INIT = 20;
	static final double RADIUS = 10;
	private static final double GRAVITY = 0.03;
	private double x = 0;
	private double y = 0;
	private double vx = 0;
	public void setVx(double vx) {
		this.vx = vx;
	}
	public void setVy(double vy) {
		this.vy = vy;
	}
	public double getX() {
		return x;
	}
	public double getY() {
		return y;
	}
	public double getVx() {
		return vx;
	}
	public double getVy() {
		return vy;
	}
	private double vy = 0;
	
	public void init(Slime.TEAM t) {
		if ( t == Slime.TEAM.BLUE ) {
			x = SERVE_OFFSET;
		} else {
			x = GameWin.WIN_WIDTH - SERVE_OFFSET;
		}
		y = Y_INIT;
		vx = 0;
		vy = 0;
	}
	public void move() {
		vy += GRAVITY;
		x += vx;
		y += vy;
		
		if ( x < 0 + RADIUS) {
			x = (RADIUS - x) + RADIUS;
			vx = - vx;

		};
		if ( x > GameWin.WIN_WIDTH - RADIUS) {
			x = (GameWin.WIN_WIDTH - RADIUS) - 
			(x - (GameWin.WIN_WIDTH - RADIUS)); 	
			vx = - vx; 
		}
	}
}
