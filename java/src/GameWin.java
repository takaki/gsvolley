import java.util.Timer;
import java.util.TimerTask;

import org.freedesktop.cairo.Context;
import org.freedesktop.cairo.SolidPattern;
import org.gnome.gdk.Event;
import org.gnome.gdk.EventKey;
import org.gnome.gdk.Keyval;
import org.gnome.gtk.DrawingArea;
import org.gnome.gtk.Gtk;
import org.gnome.gtk.Widget;
import org.gnome.gtk.Window;
import org.gnome.pango.FontDescription;
import org.gnome.pango.Layout;
import org.gnome.pango.LayoutLine;

public class GameWin extends Window {
	static final int WIN_WIDTH = 640;
	static final int WIN_HEIGHT = 480;
	public static final double PENALTY_RADIUS = 30;

	private GameInfo gameinfo = new GameInfo();

	final private SolidPattern pattern_blue = new SolidPattern(0, 0, 1, 1);
	final private SolidPattern pattern_red = new SolidPattern(1, 0, 0, 1);
	final private SolidPattern pattern_ball = new SolidPattern(1, 1, 1, 1);
	final private SolidPattern pattern_penalty = new SolidPattern(0, 1, 0, 1);

	class EventLoop extends TimerTask {
		final GameWin w;

		EventLoop(GameWin w) {
			this.w = w;
		}

		public void run() {
			switch (gameinfo.getState()) {
			case GS_INIT:
				gameinfo.setState(GameInfo.GameState.GS_SERVICE_BY_BLUE);
				break;
			case GS_PLAY:
				gameinfo.getSlime_blue().move();
				gameinfo.getSlime_red().move();
				gameinfo.collision_ball_slime(Slime.TEAM.BLUE);
				gameinfo.collision_ball_slime(Slime.TEAM.RED);
				gameinfo.collision_penalty();
				gameinfo.getBall().move();

				if (gameinfo.getBall().getY() >= GameWin.WIN_HEIGHT
						- Ball.RADIUS) {
					if (gameinfo.getBall().getX() <= GameWin.WIN_WIDTH / 2) {
						gameinfo.setState(GameInfo.GameState.GS_GOT_BY_RED);
					} else {
						gameinfo.setState(GameInfo.GameState.GS_GOT_BY_BLUE);
					}
				}
				break;
			case GS_GOT_BY_BLUE:
				gameinfo.score_move(Slime.TEAM.BLUE);
				break;
			case GS_GOT_BY_RED:
				gameinfo.score_move(Slime.TEAM.RED);
				break;
			case GS_SERVICE_BY_BLUE:
				gameinfo.serve_set(Slime.TEAM.BLUE);
				break;
			case GS_SERVICE_BY_RED:
				gameinfo.serve_set(Slime.TEAM.RED);
				break;
			case GS_WON_BY_BLUE:
				break;
			case GS_WON_BY_RED:
				break;
			default:
				System.err.println("ERROR!! Unknown Game State");
				break;
			}
			w.queueDraw();
		}
	}

	class MyExpose implements Widget.Draw {
		private Context cr;

		private void draw_back() {
			cr.setSource(0, 0, 0);
			cr.rectangle(0, 0, WIN_WIDTH, WIN_HEIGHT);
			cr.fill();

			cr.setSource(1, 1, 1);
			cr.rectangle(WIN_WIDTH / 2 - 1, 120, 2, WIN_HEIGHT);
			cr.fill();
		}

		private void draw_slime(Slime s) {
			if (s.getTeam() == Slime.TEAM.BLUE) {
				cr.setSource(pattern_blue);
			} else {
				cr.setSource(pattern_red);
			}
			cr.moveTo(s.getX(), WIN_HEIGHT);
			cr.arc(s.getX(), WIN_HEIGHT, Slime.RADIUS, Math.PI, 0);
			cr.fill();
		}

		private void draw_ball(Ball b) {
			cr.setSource(pattern_ball);
			cr.arc(b.getX(), b.getY(), Ball.RADIUS, 0, 2 * Math.PI);
			cr.fill();
		}

		private void draw_penalty(double y) {
			cr.setLineWidth(5.0);

			cr.setSource(pattern_penalty);
			cr.arc(WIN_WIDTH / 2, y, PENALTY_RADIUS, // XXX
					0, 2 * Math.PI);
			cr.strokePreserve();
		
			cr.setSource(0, 0, 0);
			cr.fill();

			cr.setSource(pattern_penalty);
			cr.arc(WIN_WIDTH / 2, y, PENALTY_RADIUS - 15, // XXX
					0, 2 * Math.PI);
			cr.setLineWidth(5.0);
			cr.strokePreserve();

			cr.setSource(0, 0, 0);
			cr.fill();

		}

		private void draw_score(int blue, int count, int red) {
			String buf = "";
			buf += "Blue: " + blue + " Ball: " + count + " Red: " + red;
			show_text_centering(buf, 20);
		}

		void show_text_centering(String text, double y) {
			final Layout layout = new Layout(cr);
			final FontDescription desc = new FontDescription("16");
			layout.setFontDescription(desc);
			layout.setText(text);
			final LayoutLine first = layout.getLineReadonly(0);
			double width = first.getExtentsLogical().getWidth();
			cr.moveTo((WIN_WIDTH - width) / 2, y);
			cr.showLayout(layout);
		}
		@Override
		public boolean onDraw(Widget arg0, Context arg1) {
			// TODO Auto-generated method stub
			cr = arg1;// new Context(widget.getWindow());
			
			draw_back();
			draw_slime(gameinfo.getSlime_blue());
			draw_slime(gameinfo.getSlime_red());
			draw_penalty(gameinfo.getPenalty_y());
			draw_ball(gameinfo.getBall());
			draw_score(gameinfo.getScore_blue(), gameinfo.getBall_count(),
					gameinfo.getScore_red());

			switch (gameinfo.getState()) {
			case GS_INIT:
				break;
			case GS_PLAY:
				break;
			case GS_GOT_BY_BLUE:
				show_text_centering("DROPPED BY RED", 40);
				break;
			case GS_GOT_BY_RED:
				show_text_centering("DROPPED BY BLUE", 40);
				break;
			case GS_SERVICE_BY_BLUE:
				break;
			case GS_SERVICE_BY_RED:
				break;
			case GS_WON_BY_BLUE:
				show_text_centering("PRESS R TO PLAY AGAIN", 80);
				show_text_centering("WON BY BLUE", 60);
				break;
			case GS_WON_BY_RED:
				show_text_centering("PRESS R TO PLAY AGAIN", 80);
				show_text_centering("WON BY RED", 60);
				break;
			default:
				System.err.println("ERROR!! Unknown Game State");
				break;
			}

			return false;
		}

	
	}

	public GameWin() {

		final DrawingArea d = new DrawingArea();
		this.add(d);
		this.setSizeRequest(WIN_WIDTH, WIN_HEIGHT);
		this.setResizable(false);
		Timer timer = new Timer(true);

		d.connect(new MyExpose());
		this.connect(new Widget.KeyPressEvent() {
			@Override
			public boolean onKeyPressEvent(Widget w, EventKey ev) {
				Keyval kv = ev.getKeyval();
				if (kv == Keyval.a) {
					gameinfo.getSlime_blue().setVx(-1);
				} else if (kv == Keyval.s) {
					gameinfo.getSlime_blue().setVx(1);
				} else if (kv == Keyval.Left) {
					gameinfo.getSlime_red().setVx(-1);
				} else if (kv == Keyval.Right) {
					gameinfo.getSlime_red().setVx(1);
				} else if (kv == Keyval.Escape ) {
					Gtk.mainQuit();
				} else if (kv == Keyval.r) {
					if (gameinfo.getState() == GameInfo.GameState.GS_WON_BY_BLUE
							|| gameinfo.getState() == GameInfo.GameState.GS_WON_BY_RED) {
						// gameinfo.set_state(GS_INIT);
						gameinfo = new GameInfo();
					}
				} else {
					;
				}

				return false;
			}
		});
		this.connect(new Window.DeleteEvent() {

			@Override
			public boolean onDeleteEvent(Widget arg0, Event arg1) {
				Gtk.mainQuit();
				return false;
			}
		});
		EventLoop task = new EventLoop(this);
		timer.scheduleAtFixedRate(task, 0, 10);

		// gameinfo.init(); // XXX
	}

}
