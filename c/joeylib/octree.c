int maxvosinoctree=10;

class Renderable;
V3d posforrend(Renderable *);

template <class Object>
class Octree {
public:
  V3d cen;
  float size; // Distance from edge to cen
  float myradius;
  float largestobject;
  bool subbed;
  List<Object> vos; // List of visible objects
  int totalobjs;
  Octree<Object> ***sub;
  V3d (*posfor)(Object);

  float closestdistanceto(V3d pos,float bestsofar) {
    if (subbed) {
      // Do best subblob first
      int bi,bj,bk;
      getijkfor(pos,&bi,&bj,&bk);
      for (int i=0;i<=1;i++)
      for (int j=0;j<=1;j++)
      for (int k=0;k<=1;k++)
        if (i==bi && j==bj && k==bk)
          if (sub[i][j][k].cubeclosest(pos)<=bestsofar)
            bestsofar=sub[i][j][k].closestdistanceto(pos,bestsofar);
      // Now do rest
      for (int i=0;i<=1;i++)
      for (int j=0;j<=1;j++)
      for (int k=0;k<=1;k++)
        if (!(i==bi && j==bj && k==bk))
          if (sub[i][j][k].cubeclosest(pos)<=bestsofar)
            bestsofar=sub[i][j][k].closestdistanceto(pos,bestsofar);
    } else {
      for (int i=1;i<=vos.len;i++) {
        float dist=V3d::dist(posfor(vos.num(i)),pos);
        if (dist<bestsofar)
          bestsofar=dist;
      }
//      printf("Test %f\n",bestsofar);
    }
    return bestsofar;
  }

  float cubeclosest(V3d v) {
    if (v.x>=cen.x-size && v.x<=cen.x+size &&
        v.y>=cen.y-size && v.y<=cen.y+size &&
        v.z>=cen.z-size && v.z<=cen.z+size)
      return 0.0;
    int i,j,k;
    getijkfor(v,&i,&j,&k);
    V3d corner=V3d(cen.x-size,cen.y-size,cen.z-size)+V3d((float)i*size*2.0,(float)j*size*2.0,(float)k*size*2.0);
    return V3d::dist(v,corner);
  }

  float closestdistanceto(V3d pos) {
    return closestdistanceto(pos,100000.0);
  }

  void clear() {
    freedom();
    subbed=false;
    vos=List<Object>();
    totalobjs=0;
    sub=NULL;
  }

  void freedom() {
    vos.freedom();
    if (sub!=NULL)
    for (int i=0;i<2;i++) {
      for (int j=0;j<2;j++)
        destroy(sub[i][j]);
      free(sub[i]);
    }
    free(sub);
  }
  
  static V3d posforv3d(V3d v) {
    return v;
  }
  
  Octree() {
    subbed=false;
    vos=List<Object>();
    totalobjs=0;
    sub=NULL;
    largestobject=0.1;
    cen=V3d(0,0,0);
    size=10000;
    myradius=sqrt(3.0*size*size);
//    posfor=&posforrend;
  }
  
  Octree(V3d t1,float t2,V3d fn(Object)) {
    vos=List<Object>();
    totalobjs=0;
    subbed=false;
    largestobject=0.1;
    
    cen=t1; size=t2;
    myradius=sqrt(3.0*size*size);
    posfor=fn;
  }
  
  Octree(V3d t1,float t2) {
    subbed=false;
    vos=List<Object>();
    totalobjs=0;
    sub=NULL;
    cen=t1; size=t2;
    myradius=sqrt(3.0*size*size);
    posfor=&posforv3d;
  }

  void getijkfor(V3d v,int *i,int *j,int *k) {
        if (v.x<cen.x)
          *i=0;
        else
          *i=1;
        if (v.y<cen.y)
          *j=0;
        else
          *j=1;
        if (v.z<cen.z)
          *k=0;
        else
          *k=1;
  }
  
  void add(Object vo) {
    // Is it in me?  Let's not bother checking, and pretend it is.
    totalobjs++;
    // cen.print();
    // printf("%i,%i ",totalobjs,vos.len);
    V3d v=posfor(vo);
    // v.print();
    if (v.x>=cen.x-size && v.x<=cen.x+size &&
      v.y>=cen.y-size && v.y<=cen.y+size &&
    v.z>=cen.z-size && v.z<=cen.z+size) {
      int i,j,k;
      
      if (subbed) {

        // This method assumes should be in me
        int i,j,k;
        getijkfor(v,&i,&j,&k);
        sub[i][j][k].add(vo);
        
      } else {
        
        vos.add(vo);
        if (vos.len>maxvosinoctree) {
          // Sub-divide octree
          subbed=true;
          sub=new (Octree<Object>**)[2];
          for (i=0;i<=1;i++) {
            sub[i]=new (Octree<Object>*)[2];
            for (j=0;j<=1;j++) {
              sub[i][j]=new Octree<Object>[2];
              for (k=0;k<=1;k++) {
                sub[i][j][k]=Octree(cen+V3d((i*2.0-1.0)*size/2.0,(j*2.0-1.0)*size/2.0,(k*2.0-1.0)*size/2.0),size/2.0,posfor);
                for (int n=1;n<=vos.len;n++)
                sub[i][j][k].add(vos.num(n));
              }
            }
          }
          vos=List<Object>(); // used to be null =) <- eh?
        }
      }
    } else {
      // v.print();
      // printf(" does not go in ");
      // cen.print();
      // printf(" size %f\n",size);
    }
  }
  
  void operator+(Object vo) {
    add(vo);
  }
  
  void addobjstolist(List<Object> *n) {
    // cen.print(); printf(" ");
    if (subbed) {
      for (int i=0;i<=1;i++)
      for (int j=0;j<=1;j++)
      for (int k=0;k<=1;k++)
      sub[i][j][k].addobjstolist(n);
    } else
    n->add(&vos);
  }
  
  void addobjstoorderedlist(OrderedList<Object> *n,V3d cen) {
    if (subbed) {
      for (int i=0;i<=1;i++)
      for (int j=0;j<=1;j++)
      for (int k=0;k<=1;k++)
      sub[i][j][k].addobjstoorderedlist(n,cen);
    } else {
      for (int i=1;i<=vos.len;i++) {
        Object *o=vos.p2num(i);
        n->add(o,V3d::dist(posfor(*o),cen));
      }
    }
  }
  
  void addobjstoorderedlistclipping(OrderedList<Object> *n,Frustrum *f) {
    if (subbed) {
      for (int i=0;i<=1;i++)
      for (int j=0;j<=1;j++)
      for (int k=0;k<=1;k++)
      sub[i][j][k].addobjstoorderedlistclipping(n,f);
    } else {
      for (int i=1;i<=vos.len;i++) {
        Object *o=vos.p2num(i);
        if (f->include(posfor(*o),0)==2)
          n->add(o,V3d::dist(posfor(*o),cen));
      }
    }
  }
  
  List<Object> *getsimplelist() {
    List<Object> *n=new List<Object>(totalobjs);
    addobjstolist(n);
    return n;
  }
  
  void collectclipping(Frustrum *f,List<Object> *n) {
    if (subbed) {
      for (int i=0;i<=1;i++)
      for (int j=0;j<=1;j++)
      for (int k=0;k<=1;k++) {
        Octree<Object> *s=&sub[i][j][k];
        int r=f->include(s->cen,s->myradius+s->largestobject);
        if (r==2)
          s->addobjstolist(n);
        if (r==1)
          s->collectclipping(f,n);
        // Otherwise ignore
      }
    } else {
      addobjstolist(n);
    }
  }
  
  void collectorderedclipping(Frustrum *f,OrderedList<Object> *n) {
    if (subbed) {
      for (int i=0;i<=1;i++)
      for (int j=0;j<=1;j++)
      for (int k=0;k<=1;k++) {
        Octree<Object> s=sub[i][j][k];
        int r=f->include(s.cen,s.myradius+s.largestobject);
        if (r==2)
          s.addobjstoorderedlist(n,f->cen);
        if (r==1)
          s.collectorderedclipping(f,n);
        // Otherwise ignore
      }
    } else {
      addobjstoorderedlistclipping(n,f);
    }
  }
  
  List<Object> *getclippedlist(Frustrum f) {
    List<Object> *n=new List<Object>(totalobjs);
    collectclipping(&f,n);
    return n;
  }
  
  OrderedList<Object> *getorderedlist(Frustrum f) { // earliest in list are closest to eye
    OrderedList<Object> *n=new OrderedList<Object>();
    collectorderedclipping(&f,n);
    return n;
  }
  
  
  /* Polygon3d firstintersection(Line l) {
       // V3d start,V3d dir) {
         if (l.contains(this)>0) {
           if (subbed) {
             OrderedList ol=new OrderedList();
             for (int i=0;i<=1;i++) {
               for (int j=0;j<=1;j++) {
                 for (int k=0;k<=1;k++) {
                   ol.add(sub[i][j][k],V3d.dist(l.start,sub[i][j][k].cen));
             } } 
           }
           Polygon3d ifound=null;
           for (int i=1;i<=8 && ifound==null;i++) {
             ifound=((Octree)(ol.num(i))).firstintersection(l);
             }
           return ifound;
         } else {
           // forall objects, map to object's space and fire a ray.
           for (int i=1;i<=vos.len;i++) {
             DO o=(DO)vos.num(i);
             if (l.contains(o)>0) {
               List tdv=o.threedview();
               for (int j=1;j<=tdv.len;j++) {
                 Polygon3d p=(Polygon3d)tdv.num(j);
                 if (l.contains(p)>0)
                   return p;
               }
             }
           }
         }
       }
       return null;
     }
     public float boundrad() {
       return myradius+largestobject;
     }
     public void getpolygonsinfrustrum(Frustrum f,OrderedList l) {
       int octres=f.v.contains(this);
       if (octres==1)
         addall(f,l);
       if (octres==2) {
         if (subbed) {
           for (int i=0;i<=1;i++) {
             for (int j=0;j<=1;j++) {
               for (int k=0;k<=1;k++) {
                 sub[i][j][k].getpolygonsinfrustrum(f,l);
           } } 
         }
       } else {
         for (int i=1;i<=vos.len;i++) {
           DO obj=(DO)vos.num(i);
           int objres=f.v.contains(obj);
           if (objres==1)
             addobject(f,obj,l);
           if (objres==2)
             checkobject(obj,f,l);
         }
       }
     }
   }
   void addall(Frustrum f,OrderedList l) {
     if (subbed) {
       for (int i=0;i<=1;i++) {
         for (int j=0;j<=1;j++) {
           for (int k=0;k<=1;k++) {
             sub[i][j][k].addall(f,l);
       } } 
     }
   } else {
     for (int i=1;i<=vos.len;i++) {
       DO obj=(DO)vos.num(i);
       addobject(f,obj,l);
     }
   }
 }
 void addobject(Frustrum f,DO obj,OrderedList ol) {
   List l=obj.threedview();
   for (int i=1;i<=l.len;i++) {
     Polygon3d p=(Polygon3d)l.num(i);
     addpoly(f,p,ol);
   }
 }		
 void checkobject(DO obj,Frustrum f,OrderedList ol) {
   List l=obj.threedview();
   for (int i=1;i<=l.len;i++) {
     Polygon3d p=(Polygon3d)l.num(i);
     if (f.v.contains(p)>0)
       addpoly(f,p,ol);
   }
 }
 void addpoly(Frustrum f,Polygon3d p,OrderedList l) {
   l.add(p,V3d.dist(p.cen(),f.cen));
 }
 public void getpolygonsinview(View v,Scene s) {
   // inside visibility radius
   if (V3d.dist(cen,v.pos)<=myradius+World.visibility(v.pos)) {
     // inside view angle
     // if (myMaths.abs(V3d.ang(V3d.sub(cen,v.pos),v.ori.z()))<v.angx) {
       if (!subbed) {
         for (int i=1;i<=vos.len;i++) {
           DO obj=(DO)vos.num(i);
           List polys=obj.threedview();
           System.out.println(obj.type+" "+polys.len);
           for (int j=1;j<=polys.len;j++) {
             Polygon3d p=(Polygon3d)polys.num(j);
             // One-sided polygons must be facing toward viewer
             // System.out.println("Polygon goes from "+p);
             p=p.fixto(obj);
             // System.out.println(p.cen());
             if (V3d.normdot(V3d.sub(p.cen(),v.pos),v.ori.z())>0) {
               if (p.type.equals("light"))
                 s.lights.add(p);
               else
               s.polygons.add(p,V3d.dist(p.cen(),v.pos));
             }
           }
         }
       } else {
         for (int i=0;i<=1;i++) {
           for (int j=0;j<=1;j++) {
             for (int k=0;k<=1;k++) {
               sub[i][j][k].getpolygonsinview(v,s);
         } } 
       }
     }
   } //
 }
} 
*/

// Old stuff

// if (world.octree.firstintersectionfromto(light.cen,p.cen())==null) {
/* public void getobjectsinview(View v,OrderedTree ot) {
   if (V3d.dist(cen,v.pos)<=Math.sqrt(3*size*size)+world.visibility(v.pos)) {
     if (V3d.ang(V3d.sub(cen,v.pos),v.ori.z())<v.ang) {
       if (!subbed) {
         for (int i=1;i<=vos.len;i++) {
           ot.add(vos.num(i),V3d.dist(cen,((DO)vos.num(i)).pos));
         }
       } else {
         for (int i=0;i<=1;i++) {
           for (int j=0;j<=1;j++) {
             for (int k=0;k<=1;k++) {
               sub[i][j][k].getobjectsinview(v,ot);
         } } 
       }
     }
   } 
 }
}*/
/* public OrderedList getpolygonsinplanes(List planes) {
 boolean outside=false;
 for (int i=1;i<=planes.len && !outside;i++) {
   Plane p=(Plane)planes.num(i);
   if (V3d.dot(p.normal,V3d.sub(cen,p.point)))
     public Polygon3d firstintersectionfromto(V3d a,V3d b) { //unfinished
     return null;
   }
 }
}*/
};


