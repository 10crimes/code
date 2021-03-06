package tools.parser;

/*
 * This source code is freely distributable under the GNU public licence. I
 * would be delighted to hear if have made use of this code. If you make money
 * with this code, please give me some! If you find this code useful, or have
 * any queries, please feel free to contact me: pclark@cs.bris.ac.uk /
 * joey@neuralyte.org Paul "Joey" Clark, hacking for humanity, Feb 99
 * www.cs.bris.ac.uk/~pclark / www.changetheworld.org.uk
 */

// import java.lang.*;
import java.util.*;

import java.awt.TextArea; // debug
import java.awt.Frame; // debug
import java.awt.FlowLayout; // debug
import java.awt.event.ActionListener; // debug
import java.awt.event.ActionEvent; // debug
import java.awt.Button; // debug

import java.io.*;

import jlib.Files;
import jlib.JString;
import jlib.strings.*;
import jlib.ArgParser;
import jlib.Profile;

// import jlib.JReflect;
import java.lang.reflect.*;

import org.neuralyte.Logger;

public class Parser implements ActionListener {

	// Debugging settings
	static boolean debugging = false;
	static boolean debuggingWin = false;
	static boolean debuggingText = false;
	static boolean debuggingOut = false;
	static boolean debugPath = false;

	// Debugging data
	public static String all;
	public static Frame dbf;
	public static TextArea dbta;
	public static TextArea dbdta;
	public static Button dbb;
	public static boolean stopped = false;
	public static String debugdata = "";
	public static int lastselend = 0;
	public static int k = 0;
	public static Vector<Match> allMatches = new Vector<Match>();
	public static String lastMatchAttempt;

	public final static void main(String[] argv) throws FileNotFoundException {

		ArgParser a = new ArgParser(argv);

		if (a.contains("-win", "Show parse window")) {
			debuggingWin = true;
		}
		if (a.contains("-debug", "Write debug data to stdout")) {
			debugging = true;
			debuggingText = true;
			debuggingOut = true;
			debugPath = true;
		}
		if (a.contains("-debugwin", "Show debug window")) {
			debugging = true;
			debuggingWin = true;
			debuggingText = true;
			debugPath = true;
		}
		String grammar = a.get("grammar");
		String file = a.get("file to parse");

		List<String> targets = a.getRest("target",
		      "output type of form <target>[:<file to append>]");
		// String target=a.getOr("target","null");
		// if (target.equals("null"))
		// target=null; // ( argv.length<3 ? null : argv[2]);
		a.done();

		// Since this app writes meaningful data on stdout, we will send log
		// messages elsewhere.
		Logger.outputStream = System.err;
		
		Parser p = new Parser();
		Grammar g = p.setupgrammar(grammar);
		String toparse = Files.readStringfromfile(file);
		
		all = toparse;
		if (debuggingWin) {
			dbta = new TextArea(all);
			dbta.setEditable(false);
			dbdta = new TextArea("");
			dbdta.setEditable(false);
			dbdta.setSelectionStart(0);
			// dbdta.setColumns(20);
			dbb = new Button("Start/Stop");
			dbb.addActionListener(p);
			dbf = new Frame();
			dbf.setSize(600, 600);
			dbf.setLayout(new FlowLayout());
			dbf.add(dbta);
			dbf.add(dbdta);
			dbf.add(dbb);
			dbf.setVisible(true);
			dbdta.setSize(400, 300);

			try {
				// visualjava.VisualJava.showObject(dbf);
				Class< ? > argTypes = Object.class;
				Class.forName("visualjava.VisualJava").getMethod("showObject", argTypes ).invoke(null, dbdta);
			} catch (Exception e) {
				org.neuralyte.Logger.error(e);
			}
			
		}
		
		ParseContext ctx = new ParseContext(g);
		
		// @todo Apply preprocessors
		
		Match m = parseString(toparse, ctx);

		if (m == null)
			System.out.println("Failed to match.");
		else {

			// if (target==null)
			if (targets.size() == 0) {
				m.printParseTree(System.out, "");
			} else {
				OutputContext octx = new OutputContext(g);
				for (int k = 0; k < targets.size(); k++) {
					String targetcom = (String) targets.get(k);
					if (targetcom.indexOf(":") == -1) {
						m.render(octx, null, targetcom, System.out);
					} else {
						String target = JString.before(targetcom, ":");
						String outfile = JString.after(targetcom, ":");
						PrintStream out = new PrintStream(new FileOutputStream(
						      outfile));
						m.render(octx, null, target, out);
						out.flush();
						out.close();
					}
				}
			}

		}

		// System.err.println(" # substrings created: "+SubString.sscnt);
		// System.err.println("# substrings realised: "+SubString.ssrealedcnt);
		// System.err.println("               that's: "+(int)(100*SubString.ssrealedcnt/SubString.sscnt)+"%");
		// System.err.println("        # realstrings: "+RealString.realcnt);
		// System.err.println(SubString.ssrealedcnt+" ("+(int)(100*SubString.ssrealedcnt/SubString.sscnt)+"%) of the "+SubString.sscnt+" generated substrings were realised.");
		// System.err.println("Parse took "+(Profile.timeSpentOn("parse")/1000)+" seconds");
		boolean failure = false;
		if (m == null) {
			failure = true;
		} else if (m.left == null) {
			failure = true; // unlikely
		} else if (m.left.length() > 0) {
			failure = true;
		}

		if (failure) {

			System.err.println();
			System.err.println("Failure.");
			System.err.println("Last match attempt:");
			System.err.println(lastMatchAttempt);
			if (m == null || m.left == null || toparse == null)
				System.err.println("m=" + m + " toparse=" + toparse);
			else {
				System.err.println("Error: failed to match last "
						+ m.left.length() + " characters = "
						+ (100 * m.left.length() / toparse.length()) + "%");
				System.err.println("Did match up to "
						+ JString.lineChar(toparse, m.left + ""));
			}
			if (ctx.closestFailure != null) {
				System.err.println("At furthest reach, "+ctx.closestFailure);
			}

			String profReport = Profile.reportInHTML();
			PrintStream profOut = new PrintStream(new FileOutputStream(new File(
			"/tmp/jparse_profile.html")));
			profOut.print(profReport);
			profOut.close();

			System.exit(1);
			
		}
		
	}

	/**
	 * Note that even if you receive a Match, it may not be complete. Check
	 * match.left.length() to see if there are trailing chars which were not
	 * parsed.
	 * 
	 * At a later date we may act if that is the case, e.g. by returning a null Match.
	 * @param ctx 
	**/
	public static Match parseString(String toparse, ParseContext ctx) {
		
	   Type t = new Atom("Main");
		Profile.clear();
		// Profile.start("parse");
		Match m = null;
		try {
			m = t.match(new SubString(toparse), ctx);
			// Profile.stop("parse");
			// Profile.start("register");
			registerAllMatches(m);
			// Profile.stop("register");
		} catch (Exception e) {
			System.out.println("" + e);
			e.printStackTrace();
		}
	   return m;
   }

	public static void registerAllMatches(Match m) {
		if (m == null) return;
		allMatches.add(m);
		if (m.matches == null) return;
		for (int i = 0; i < m.matches.size(); i++) {
			if (m.matches.get(i) != null)
			   registerAllMatches((Match) m.matches.get(i));
		}
	}

	public Grammar setupgrammar(String gram) {
		String whole = "tools.parser." + gram + "Grm";
		try {
			// Class c = JReflect.classcalled(whole);
			Class c = Class.forName(whole);
			// System.out.println("Got class "+c);
			Method m = c.getMethod("setupgrammar", new Class[0]);
			// System.out.println("Got method "+m);
			Object dummy = m.invoke(null, new Object[0]);
			return (Grammar)dummy;
		} catch (Exception e) {
			System.out.println("Problem initialising grammar \"" + gram + "\": "
			      + e);
			e.printStackTrace();
			return null;
		}
		// grmGrm.setupgrammar();
	}

	public void actionPerformed(ActionEvent e) {
		stopped = !stopped;
		if (!stopped) Parser.debuggingText = !Parser.debuggingText;
	}

	public static void gotto(int i) {
		// dbta.setSelectionStart(0);
		if (i != lastselend) {
			lastselend = i;
			dbta.setSelectionEnd(i);
		}
	}

	public static void doing(SomeString s) {
		k = (k + 1) % 20;
		// k=0;
		if (k == 0) {
			doing(0, all.length() - s.length());
			/*
			 * int i=all.indexOf(""+s); int j=all.indexOf(""+s,i+1); //int i=new
			 * RealString(all).indexOf(s); //int j=new
			 * RealString(all).indexOf(s,i+1); if (j==-1) doing(0,i);
			 */
		}
		while (stopped) {
		}
	}

	public static void doing(int i, int j) {
		// dbta.setSelectionStart(0);
		// if (i!=lastselend) {
		// lastselend=i;
		if (debugging) {
			dbta.setSelectionStart(i);
			dbta.setSelectionEnd(j);
		}
		// }
	}

	public static void report(Object a, Object b) {
		report("" + a + b);
	}

	public static void report(Object a, Object b, Object c) {
		report("" + a + b + c);
	}

	public static void report(Object a, Object b, Object c, Object d) {
		report("" + a + b + c + d);
	}

	public static void report(Object a, Object b, Object c, Object d, Object e) {
		report("" + a + b + c + d + e);
	}

	public static void report(Object a, Object b, Object c, Object d, Object e,
	      Object f) {
		report("" + a + b + c + d + e + f);
	}

	public static void report(Object a, Object b, Object c, Object d, Object e,
	      Object f, Object g) {
		report("" + a + b + c + d + e + f + g);
	}

	public static void report(Object a, Object b, Object c, Object d, Object e,
	      Object f, Object g, Object h) {
		report("" + a + b + c + d + e + f + g + h);
	}

	public static void report(Object a, Object b, Object c, Object d, Object e,
	      Object f, Object g, Object h, Object i) {
		report("" + a + b + c + d + e + f + g + h + i);
	}

	public static void report(String s) {
		if (debuggingText) {
			if (debuggingOut) {
				String tmp = s;
				if (tmp.endsWith("\n")) tmp = tmp.substring(0, tmp.length() - 1);
				System.out.println(tmp);
			} else {
				if (stopped) {
					dbdta.setText(debugdata);
					// dbdta.append(debugdata);
					debugdata = "";
					while (stopped) {
					}
				}
				debugdata += s;
			}
		}
	}

	public static String decode(String text) {
		text = JString.replace(text, "!qt!", "\\\"");
		text = JString.replace(text, "\\n", "\\n");
		return text;
	}
}
