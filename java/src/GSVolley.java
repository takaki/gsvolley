import org.gnome.gtk.Gtk;

public class GSVolley {
	public static void main(String[] args) {
		final GameWin w;
		Gtk.init(args);
		w = new GameWin();
		w.showAll();
		Gtk.main();
	}

}
