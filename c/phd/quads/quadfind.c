bool withinrange(V2d a,V2d b) {
  return (V2d::dist(a,b)<cdclip);
}

bool parallelenough(QuadsLine2d p,QuadsLine2d q,bool swp,bool swq) {
  if (swp)
    p.swapends();
  if (!swq)
    q.swapends();
  // return true;
  return (V2d::normdot(p.a-p.b,q.a-q.b)>dotabove);
}

bool badintersection(QuadsLine2d *l,QuadsLine2d *o,int wv,int e) {
  QuadsLine2d rl=*l;
  if (wv==1)
    rl.swapends();
  QuadsLine2d ro=*o;
  if (e==2)
    ro.swapends();
  float t=rl.findintersectionnum(ro);
  if (t<1.0-creepintoline) // last
    return true;
  t=ro.findintersectionnum(rl);
  if (t>creepintoline) // first
    return true;
  return false;
}

int tryjoint(QuadsLine2d *p,QuadsLine2d *q,V2d x,V2d y,int pab,int qab) {
  if (withinrange(x,y)) {
    p->ls.p2num(pab)->add(q);
    p->es.p2num(pab)->add(qab);
    q->ls.p2num(qab)->add(p);
    q->es.p2num(qab)->add(pab);
//    jbmp.line(q,7);
    return 1;
  }
  return 0;
}

List<List<QuadsLine2d *> > fq;
List<List<bool> > fwww;

void quadsearch(List<QuadsLine2d *> *q,List<bool> *www,List<List<QuadsLine2d> > *qs) {
  // printf("%i",q->len);
  
  if (q->len==4) {
    
    // Check last line meets first, and 2 and 4 are parallel
    int cl=( www->num(4) ? 1 : 2 );
    int cf=( www->num(1) ? 2 : 1 );
    V2d l=q->num(4)->end(cl);
    V2d f=q->num(1)->end(cf);
    if (withinrange(f,l) &&
      parallelenough(*q->num(2),*q->num(4),www->num(2),www->num(4)) &&
      !badintersection(q->num(4),q->num(1),cl,cf)) {
      
      // Realise the potential polygon by intersecting edges
      bool bad=false;
      List<V2d> realise=List<V2d>(4);
      // List<QuadsLine2d> realise=List<QuadsLine2d>(4);

      for (int i=1;i<=4;i++) {
        
        QuadsLine2d a=*q->num(i);
        // printf("\n%s",a.toString());
        QuadsLine2d b=*q->wrapnum(i+1);
        if (www->num(i))
          a.swapends();
        if (www->wrapnum(i+1))
          b.swapends();
        V2d i=a.findintersection(b);
        if (!withinrange(i,a.b) || !withinrange(i,b.a))
          bad=true;
        realise.add(i);
        
        /* QuadsLine2d *at=q->num(i);
           QuadsLine2d a=QuadsLine2d(at->a,at->b);
           if (www->num(i))
             a.swapends();
           realise.add(a);*/
        
      }
      
      // Check no vertex makes a concave corner
      Polygon2d p=Polygon2d(realise);
      bool concave=false;
      for (int i=1;i<=4;i++) {
        Polygon2d q=p.subpath(wrap(i+1,4),wrap(i-1,4));
        if (q.contains(p.vs.num(i)))
          concave=true;
      }
      if (!concave && !bad) {
        
        // qs->add(realise);
        
        List<QuadsLine2d> tmp=List<QuadsLine2d>(4);
        for (int i=1;i<=4;i++) {
          tmp.add(QuadsLine2d(realise.num(i),realise.wrapnum(i+1)));
          // printf("\n%s",tmp.num(tmp.len).toString());
        }
        
        qs->add(tmp);
        
        fq.add(*q);
        fwww.add(*www);
        
      }
      
    }
    
  } else {
    
    bool goahead=true;
    
    // If just added line 3, check 3 and 1 are parallel
    if (q->len==3)
      if (!parallelenough(*q->num(1),*q->num(3),www->num(1),www->num(3)))
      goahead=false;
    
    if (goahead) {
      
      QuadsLine2d *l=q->num(q->len);
      int wv=( www->num(q->len) ? 1 : 2 );
      List<QuadsLine2d *> *ls=l->ls.p2num(wv);
      List<int> *es=l->es.p2num(wv);
      // printf("(%i):[",ls->len);
      
      // Consider neighbours of last line
      for (int i=1;i<=ls->len;i++) {
        
        QuadsLine2d *o=ls->num(i);
        int e=es->num(i);
        bool ok=true;
        
        // Check if the line has already been fully searched
        if (o->allusedup)
          ok=false;
        
        // Check it isn't one already in the quad, and it doesn't cross any previous
        // Actually, we allow them to cross, and see if it's resolved later
        for (int j=1;j<=q->len;j++) {
          QuadsLine2d *t=q->num(j);
//          if (t==o || o->crosses(t))
          if (t==o)
            ok=false;
        }
        
        // Check the intersections of last and next line are not inside
        if (badintersection(l,o,wv,e))
          ok=false;
        
        if (ok) {
          
          // jbmp.line(4.8*o->a,4.8*o->b,15);
          // jbmp.display();
          q->add(o);
          www->add(( e==2 ? true : false ));
          quadsearch(q,www,qs);
          q->removenum(q->len);
          www->removenum(www->len);
          // jbmp.line(4.8*o->a,4.8*o->b,0);
          
        }
      }
    }
  }
  
}

List<QuadsLine2d> *getquad(List<QuadsLine2d> *l,List<QuadsLine2d> *sf,float cd) {
  // printf("%i %i\n",l->len,sf->len);
  if (sf->len==4)
    if (V2d::dist(sf->num(1).a,sf->num(sf->len).b)<cd)
    return sf;
  else
  return NULL;
  QuadsLine2d last=sf->num(sf->len);
  for (int i=1;i<=l->len;i++) {
    QuadsLine2d c=l->num(i);
    if (V2d::dist(last.b,c.b)<cd)
      c.swapends();
    if (V2d::dist(last.b,c.a)<cd) {
      sf->add(c);
      l->removenum(i);
      List<QuadsLine2d> *q=getquad(l,sf,cd);
      // if (q.len==4)
      if (q!=NULL)
        return q;
      l->add(c);
      l->swapelements(i,l->len);
      sf->removenum(sf->len);
    }
  }
  return NULL;
}      

void display(List<QuadsLine2d> *l) {
  // b.writetoscreen();
}

List<QuadsLine2d> readgfset(String fname) {
  printf("=)\n");
  FILE *fp=fopen(fname,"r");
  printf("=)\n");
  List<QuadsLine2d> l=List<QuadsLine2d>();
  printf("=)\n");
  
  String s=getlinefromfile(fp);
  while (!Seq(Sleft(s,4),"@SET")) {
    printf(">");
    s=getlinefromfile(fp);
  }
  s=getlinefromfile(fp);
  s=getlinefromfile(fp);
  s=getlinefromfile(fp);
  while (!Seq(Sleft(s,1),"@")) {
    printf(".");
    // Dummies
    int n,w;
    float t,r,sx,sy,ex,ey,p;
    sscanf(s,"%i %f %f %f %f %f %f %i %f",&n,&t,&r,&sx,&sy,&ex,&ey,&w,&p);
    l.add(QuadsLine2d(V2d(sx,sy),V2d(ex,ey)));
    // printf("%s\n",l.num(l.len).toString());
    s=getlinefromfile(fp);
  }
  fclose(fp);
  return l;
}

bool aboveminlen(QuadsLine2d l) {
  return (l.length>=minlen);
}

List<Polygon2d> findquads(List<QuadsLine2d> l) {
    
  printf("%i lines\n",l.len);

  // Strip out small lines
  l.filterby(&aboveminlen);

  printf("Stripped small lines, now %i\n",l.len);
  
  // Build graph
  printf("Building graph (%i)\n",l.len);
  float ave=0;
  for (int i=1;i<=l.len;i++) {
    // jbmp.clear();
    printf(".");
    QuadsLine2d *p=l.p2num(i);
//    jbmp.line(p,15);
    int cnt=0;
    for (int j=i+1;j<=l.len;j++) {
      QuadsLine2d *q=l.p2num(j);
      cnt+=tryjoint(p,q,p->a,q->a,1,1);
      cnt+=tryjoint(p,q,p->a,q->b,1,2);
      cnt+=tryjoint(p,q,p->b,q->a,2,1);
      cnt+=tryjoint(p,q,p->b,q->b,2,2);
    }
    ave=ave+cnt;
    // jbmp.writetoscreen();
  // do { } while (!key[KEY_SPACE]);
  }
  printf("\n");
  ave=2.0*ave/(float)l.len; // 2.0 since each connection only counts once, but has two end vertices
  printf("Average connection of a line is %f\n",ave);
  if (ave>200) {
    printf("I can't process that!");
    exit(0);
  }
  
  /* // Display each line and its neighbours
     for (int i=1;i<=l.len;i++) {
       QuadsLine2d *p=l.p2num(i);
       jbmp.clear();
       jbmp.line(p,5);
       for (int j=1;j<=2;j++) {
         List<QuadsLine2d *> ls=p->ls.num(j);
         for (int k=1;k<=ls.len;k++) {
           jbmp.line(ls.num(k),15);
         }
       }
       jbmp.writetoscreen();
       waitforkeypress();
     }*/
  
  // Find quadrilaterals
  printf("Finding quadrilaterals...\n");
  List<List<QuadsLine2d> > qs=List<List<QuadsLine2d> >();
  for (int i=1;i<=l.len;i++) {
    
    List<QuadsLine2d *> q=List<QuadsLine2d *>(4);
    List<bool> www=List<bool>(4);
    q.add(l.p2num(i));
    www.add(false);
    quadsearch(&q,&www,&qs);
    
    // Should now remove line l.num(i) from further searches
    l.p2num(i)->allusedup=true;
    
    /* // Needn't do this since symettrical
       q=List<QuadsLine2d *>(4);
       www=List<bool>(4);
       q.add(&l.num(i));
       www.add(true);
       quadsearch(&q,&www,&qs); */
    
  }
  
  printf("\n");
  
  printf("Converting format...");
  List<Polygon2d> ps;
  for (int i=1;i<=qs.len;i++) {
    List<QuadsLine2d> *q=qs.p2num(i);
    if (q!=NULL) {
      /* for (int i=1;i<=q->len;i++) {
           int j=1+(i%q->len);
           q->num(i).b.print();
           printf(",");
           q->num(j).a.print();
           printf(" = ");
           q->p2num(i)->b=(q->num(i).b+q->num(j).a)/2.0;
           q->num(i).b.print();
           // printf(" && ");
           q->p2num(j)->a=q->p2num(i)->b;
           // q->num(j).a.print();
           printf("\n");
         }*/
      List<V2d> vs;
      vs.add(q->num(1).a);
      vs.add(q->num(2).a);
      vs.add(q->num(3).a);
      vs.add(q->num(4).a);
      Polygon2d p=Polygon2d(vs);
      ps.add(p);
      // printf("%s\n",p.toString());
      // printf("Quad found! %f\n",p.area());
      // printf("<%i>",p.vs.len);
    }
  }
  printf("done.\n");

  return ps;
}

