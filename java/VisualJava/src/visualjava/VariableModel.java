package visualjava;

import dlang.lang.tokens.DVariable;

/** joey Nov 5, 2004 4:05:06 PM
 * @deprecated WHY?!
 * Extends DVariable just for prototyping purposes!
 */
public class VariableModel extends DVariable {

    public Class type;
    public String name;
    public Object value;

    public VariableModel(Class t,String n,Object v) {
        super(n);
        type = t;
        name = n;
        value = v;
    }
    public VariableModel(String n,Object v) {
        this(v.getClass(),n,v);
    }

    public Class getType() {
        return type;
    }
    public String getName() {
        return name;
    }
    public Object getValue() {
        return value;
    }

}
