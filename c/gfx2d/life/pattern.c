#include <joeylib.c>

#define scrwid 320
#define scrhei 200


#define ss 5
//#define cs ((int)(pow(ss,9))
#define cs ss*ss*ss*ss*ss*ss*ss*ss*ss

void main() {
  JBmp *j=new JBmp(scrwid,scrhei);
  JBmp *k=new JBmp(scrwid,scrhei);
  int *change=new int[cs];
  srand((int)time(NULL));
  for (int i=0;i<cs;i++) {
    change[i]=myrnd()*ss;
  }
  for (int x=0;x<scrwid;x++)
  for (int y=0;y<scrhei;y++) {
    j->bmp[y][x]=myrnd()*ss;
  }
  allegrosetup(scrwid,scrhei);
  for (int i=0;i<ss;i++) {
    mypalette(i,myrnd(),myrnd(),myrnd());
  }
  do {
    j->writetoscreen();
    k=j;
    for (int x=0;x<scrwid;x++)
    for (int y=0;y<scrhei;y++) {
      // Collect neighbours
      int c=0;
      int n=0;
      for (int dx=-1;dx<=1;dx++)
      for (int dy=-1;dy<=1;dy++) {
        n=n+pow(ss,c)*k->pointat(x+dx,y+dy);
        c++;
      }
      j->bmp[y][x]=change[n];
    }
  } while (!key[KEY_ESC]);
}
