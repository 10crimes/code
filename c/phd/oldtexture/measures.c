float measureedgecount(List<Pixel> *ps) {
  bool local=false;
  if (ps->len==1) {
    Pixel p=ps->num(1);
    ps=p2pixelsincircle(p,eacrad);
    local=true;
  }
  float cnt=0;
  for (int i=1;i<=ps->len;i++) {
    Pixel p=ps->num(i);
    if (edgemag->inmap(p))
      cnt+=edgemag->getpos(p);
  }
  cnt=cnt/(float)ps->len;
  if (local)
    destroy(ps);
  return cnt;
}

float measureaverageintensity(List<Pixel> *ps) {
  bool local=false;
  if (ps->len==1) {
    Pixel p=ps->num(1);
    ps=p2pixelsincircle(p,eacrad);
    local=true;
  }
  float cnt=0;
  for (int i=1;i<=ps->len;i++) {
    Pixel p=ps->num(i);
    cnt+=greyscale->getpos(p);
  }
  cnt=cnt/(float)ps->len;
  if (local)
    destroy(ps);
  return cnt;
}

// float measureRGBcorrelation(List<Pixel> *ps) {
  
float measureoldedgeanglecancelling(List<Pixel> *ps) {
  bool local=false;
  printf("H\n");
  if (ps->len==1) {
    Pixel p=ps->num(1);
//    printf("Reading %i %i\n",p.x,p.y);
    ps=p2pixelsincircle(p,eacrad);
    local=true;
  }
  printf("H1\n");
  V2d v=V2d(0,0);
  float cnt=0;
  for (int i=1;i<=ps->len;i++) {
  printf("H%i\n",i);
    Pixel p=ps->num(i);
    if (edgemag->inmap(p)) {
      v=v+edgemag->getpos(p)*V2d(sin(edgeang->getpos(p)),cos(edgeang->getpos(p)));
      cnt+=edgemag->getpos(p);
    }
  }
  if (local)
    destroy(ps);
  return v.mod()/cnt;
}

float measureedgeanglecancelling(List<Pixel> *ps) {
  // Generate histogram of edges
//  float *hs=new float[eacq];
  bool local=false;
printf("MEAC1\n");
  if (ps->len==1) {
printf("MEAC1\n");
    Pixel p=ps->num(1);
printf("MEAC2\n");
    ps=p2pixelsincircle(p,eacrad);
printf("MEAC3\n");
    local=true;
  }
printf("MEAC3\n");
  float *hs=(float *)malloc(eacq*sizeof(float));
  for (int i=0;i<eacq;i++)
    hs[i]=0;
  float total=0;
  V2d v=V2d(0,0);
printf("MEAC7\n");
  for (int i=1;i<=ps->len;i++) {
    Pixel p=ps->num(i);
    if (edgemag->inmap(p)) {
      int j=eacq/2*(1+edgeang->getpos(p)/pi);
      if (j>=eacq)
        j=eacq-1;
      hs[j]+=edgemag->getpos(p);
      total+=edgemag->getpos(p);
      v=v+edgemag->getpos(p)*V2d::angle(edgeang->getpos(p));
    }
  }
  if (local)
    destroy(ps);
  float offness=0;
  for (int i=0;i<eacq/2;i++)
    offness+=mysquare(hs[i]-hs[i+eacq/2]);
  free(hs);
  // Good start, disintegrates to rubbish
  return sqrt(offness*(float)eacq)/total;
  // Some definite rejection
//  return -sqrt(offness*(float)eacq)/(float)ps->len;
}

GHist *gethist(List<Pixel> *ps) {
  GHist *gh=new GHist(glq);
  for (int i=1;i<=ps->len;i++)
    if (greyscale->inmap(ps->num(i)))
      gh->addocc(greyscale->getpos(ps->num(i)));
  return gh;
}

float measureglvariance(List<Pixel> *ps) {
  bool local=false;
  if (ps->len==1) {
    Pixel p=ps->num(1);
    ps=p2pixelsincircle(p,glhistrad);
    local=true;
  }
  GHist *gh=gethist(ps);
  if (local)
    destroy(ps);
  float v=gh->glvariance();
  destroy(gh);
  return v;
}

GHist *gethist(Pixel q) {
  if (ghs->getpos(q)==NULL) {
    Pixel p=q/ghistscale;
    List<Pixel> *ps=p2pixelsincircle(p,glhistrad);
    ghs->setpos(q,gethist(ps));
    destroy(ps);
  }
  return ghs->getpos(q);
}

float measurehiststability(Pixel p) {
  Pixel q=p*ghistscale;
  if (q.x>=1 && q.y>=1 && q.x<ghs->width-1 && q.y<ghs->height-1) {
    float d=0;
    for (int i=-1;i<=1;i++)
    for (int j=-1;j<=1;j++)
      if (i!=0 || j!=0) {
//        printf("%s\n",(q+Pixel(i,j)).toString());
        d+=gethist(q)->difference(gethist(q+Pixel(i,j)));
      }
    return d/8.0;
  } else
    return 0;
}

float measurehiststability(int l,int t,int r,int bot) {
  int mx=(l+r)/2;
  int my=(t+bot)/2;
  List<Pixel> *ps=p2pixelsinrectangle(l,t,mx,my);
  GHist *a=gethist(ps);
  destroy(ps);
  ps=p2pixelsinrectangle(mx,t,r,my);
  GHist *b=gethist(ps);
  destroy(ps);
  ps=p2pixelsinrectangle(l,my,mx,bot);
  GHist *c=gethist(ps);
  destroy(ps);
  ps=p2pixelsinrectangle(mx,my,r,bot);
  GHist *d=gethist(ps);
  destroy(ps);
  // Could be combined differently, or each squared
  float diff=sqrt(a->difference(b)+b->difference(d)+d->difference(c)+c->difference(a)+a->difference(c)+b->difference(d));
  destroy(a);
  destroy(b);
  destroy(c);
  destroy(d);
  return -diff; // Could be reciprocal
}

float measurehiststability(List<Pixel> *ps) {
//  printf("%i points\n",ps->len);
//  Region *r=new Region(ps);
  if (ps->len==1)
    return measurehiststability(ps->num(1));
  Region *r=new Region(ps);
  return measurehiststability(r->leftmost(),r->topmost(),r->rightmost(),r->bottommost());
  error("I do not know how to measure histogram stability of a randomly shaped region");
}
