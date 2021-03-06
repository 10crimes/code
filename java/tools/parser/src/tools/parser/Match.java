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
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.List;

import jlib.strings.FakeOutputStream;
import jlib.strings.SomeString;
// import java.io.PrintWriter;
/*
 * import java.awt.TextArea; // debug import java.awt.Frame; // debug import
 * java.awt.FlowLayout; // debug import java.awt.event.ActionListener; // debug
 * import java.awt.event.ActionEvent; // debug import java.awt.Button; // debug
 */
// import jlib.Files;
// import jlib.JString;

public class Match {

	Type type;
	SomeString string;
	List<Match> matches; // sub-matches making up this one, if any (can be null).
	Match parent; // parent match of which this is a sub-match
	SomeString left = null; // new RealString("error: left not initialised!");
	private List<Match> unusedMatches;

	/*
	
	// Other parts of the code assume left to exist, so hiding these constructors where it doesn't!

	Match(Type t, SomeString s) {
		type = t;
		string = s;
		matches = null; // new Vector();
	}

	Match(Type t, SomeString s, List<Match> ms) {
		type = t;
		string = s;
		matches = ms;
	}
	*/

	Match(Type t, SomeString s, SomeString ll) {
		type = t;
		string = s;
		matches = null;
		left = ll;
	}

	Match(Type t, SomeString s, List<Match> ms, SomeString ll) {
		type = t;
		string = s;
		matches = ms;
		left = ll;
	}

	public String toString() { // temporary (can be removed)
		return "Match: " + type + " = \"" + StringHelpers.strip(string) + "\"";
		// FakeOutputStream out=new FakeOutputStream();
		// render(new PrintStream(out),"");
		// return out.store.toString();
		// return "Match.toString(): gone to render(PrintStream,int)";
	}

	// @todo Rename this: printParseTree
	public void printParseTree(PrintStream out, String ind) {
		// System.out.println("Generating Match "+type+" with "+(matches==null?"no match":""+matches.size()));
		
		// Skip display of elements which are purely structural
		boolean hideme = (type instanceof RepeatedRule || type instanceof GroupedDefn);
		// boolean hideme = false;
		
		// boolean hideMatchedText = (matches!=null && matches.size()>0);
		boolean hideMatchedText = false;
		
		String matchedText = ( hideMatchedText ? "" : "\"" + StringHelpers.strip("" + string) + "\" " );
		
		if (!hideme)
			out.print(ind + type + " = " + matchedText);
		if (matches != null && matches.size() > 0) {
			if (!hideme)
				out.print( (hideMatchedText ? "" : "with ") + "[\n");
			String childInd = ind + (hideme ? "" : " ");
			for (int i = 0; i < matches.size(); i++) {
				matches.get(i).printParseTree(out, childInd);
				// if (!hideme || i>0)
					// out.print("\n");
			}
			// System.out.println("a");
			// tmp=JString.replace(tmp,"\n","\n  ");
			// System.out.println("b");
			// String weirdind = (ind + ind);
			// if (weirdind.length() > 0) weirdind = weirdind.substring(1);
			if (!hideme)
				out.print(ind + "]\n");
		} else {
			if (!hideme)
				out.print("\n");
		}
		// if (!hideme)
			// out.print(" leaving \"" + Atom.strip("" + left) + "\"");
	}

	/*
	public List<Type> rulefrommatch() {
		List<Type> v = new Vector<Type>();
		for (int i = 0; i < matches.size(); i++) {
			v.add(matches.get(i).type);
		}
		// Logger.debug("Generated replacement rule from match: "+v);
		return v;
	}
	*/

	/** render needs to learn to deal with RelElements. **/
	public void render(OutputContext ctx, Match btwyourdadis, String target, PrintStream out) {
		if (parent == null) parent = btwyourdadis;

		List<Type> replacementRule = null;
		
		// Find the replacement rule for this type, if possible
		if (type instanceof Atom) {
			Atom a = (Atom) type;
			// ParseContext ctx = new ParseContext();   /** @todo Should be passed in! **/
			RuleSet ruleset = ctx.getGrammar().getrulesetforatom(a.type, ctx);
			replacementRule = ruleset.replacements.get(target);
		}
			
		if (replacementRule == null) {

			// If there is no replacement ruleset, we just render every sub-match.
			// They may have their own replacement rules.
			
			if (matches == null) {
				
				// Logger.warn("No sub matches for "+this);
				// So far I have only seen this for Vars and ""
				out.print( this.string.toString() );
				
			} else {

				for (int i=0; i<matches.size();i++) {
					Match m = matches.get(i);
					m.render(ctx, this, target, out);
				}

			}
			
		} else {

			// if (tmp!=null || target.length()==0)
			// if (tmp==null)
			// return "";
			// List rs = (tmp == null ? rulefrommatch() : (List) tmp);
			// out.print("    // Rendering "+Atom.strip(""+this)+" against "+rs+"\n";
			// out.print("    // Rendering "+a.type+": "+rs+"\n";
			// List<Match> unusedmatches = (List<Match>) matches.clone();
			for (int i = 0; i < replacementRule.size(); i++) {
				Type t = (Type) replacementRule.get(i);
				// renderIn(unusedmatches, t, target, out);
				t.renderMatchAs(ctx, this, target, out);
			}

		}
		
		
		this.unusedMatches = null;
		
	}

	public String renderString(OutputContext ctx, String target) {
		FakeOutputStream out = new FakeOutputStream();
		render(ctx, null, target, new PrintStream(out));
		return out.store.toString();
	}
	
	public List<Match> getUnusedMatches() {
		if (this.unusedMatches == null) {
			/* Whu?  .clone() has left the language?
			if (matches instanceof Cloneable) {
				this.unusedMatches = ((Cloneable)matches).clone();
			} else {
				throw new Error("matches was built from an un-cloneable list type.");
			}
			*/
			this.unusedMatches = new ArrayList<Match>(matches);
		}
		return this.unusedMatches;
	}
	
	public Match grabUnusedMatchMatching(MagicType t) {
		List<Match> unused = getUnusedMatches();
		for (int i=0;i<unused.size();i++) {
			Match m = unused.get(i);
			// if (m.type.equals(t)) {
			if (t.replacementfor(m.type)) {
				unused.remove(i);
				return m;
			}
		}
		return null;
	}

	/*
	public void renderIn(List<Match> unusedmatches, Type outType, String target,
	      PrintStream out) {
		
		// TODO: Do we need all these instanceof checks?  Couldn't we instead
		// create a common interface for each Type to implement, e.g.
		// Type.renderIn(...) ?

		// I suspect we may want to do some error reporting somewhere here,
		// if the desired replacement is not found.  (Done a bit.)

		// TODO: use ReplacementType interface:
		//   outType.render(unusedmatches, this, target, out);
		// or just:
		//   outType.render(this, target, out);
		
		if (outType instanceof Text) {
			out.print(((Text)outType).rendertext());
		} else if (outType instanceof RelElement) {
			((RelElement)outType).render(this, target, out);
		} else if (outType instanceof ActiveReplacement) {
			((ActiveReplacement)outType).render(unusedmatches, this, target, out);
		} else if (outType instanceof ArgReplacement) {
			((ArgReplacement)outType).render(unusedmatches, this, target, out);
		
		} else {
			
			// Some replacements are simply references to matched atoms
			// For each replacement, we must find which matched atom it refers to.
			
			for (int j = 0; j < unusedmatches.size(); j++) {
				// for (int j=0;j<matches.size();j++) {
				// Match m=(Match)matches.get(j);
				Match m = unusedmatches.get(j);
				Type couldbe = (Type)m.type;
				if (outType.replacementfor(couldbe)) {
					if (outType instanceof Atom) {
						m.render(this, target, out);
						// Remove the match, so that subsequent Atoms of same type get
						// the subsequent matches
						// matches.remove(m);
					} else if ((outType instanceof Var)) {
						// *** needed if you don't want all vars printing!
						// && (tmp!=null || target.length()==0)) {
						out.print(Parser.decode("" + m.string));
						// We don't expect two vars the same!
					} else {
						Logger.warn("Unsure how to replace " + outType + " in "
						      + this);
					}
					unusedmatches.remove(j);
					break;
				}
			}

		}

		// outType.renderMatchAs(this, target, out);

		// TODO: If we have an Atom, we may want to descend into ()s []s *s and
		// +s, in case the replacement refers to atoms which appear to the in the
		// "flat" definition, but are actually hidden inside these types.
		
		// out.print("\n<Match.renderIn failed "+t+" on \n"+this+">");
	}
	 */

	/**
	 * At the moment only Atom searching is possible. Variable searching could be
	 * implemented, or you could make the variable into an atom in the grammar.
	 **/

	public Match getUp(String called) {
		if (type instanceof Atom)
		   if (((Atom) type).type.equals(called)) return this;
		if (parent == null) return null;
		return parent.getUp(called);
	}

	public Match getDown(String called) {
		// System.err.println("On "+this);
		if (type instanceof Atom)
		   if (((Atom) type).type.equals(called)) return this;
		if (matches != null) {
			for (int i = 0; i < matches.size(); i++) {
				Match m = matches.get(i);
				// System.err.println(i+"/"+matches.size()+" Match.getDown(): looking at "+m);
				Match res = m.getDown(called);
				// System.err.println("Got "+res);
				if (res != null) return res;
			}
		}
		return null;
	}

}
