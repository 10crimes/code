/* All my source code is freely distributable under the GNU public licence.
   If you make money with this code, please give me some!
   If you find this code useful, or have any queries, please feel free to
   contact me: pclark@cs.bris.ac.uk / joeyclark@usa.net
   Paul "Joey" Clark, hacking for humanity, Feb 1999
   http://www.cs.bris.ac.uk/~pclark */


// 4d julia sliced through d-axis
// Rotated view

#include <joeylib.c>

// z=z^2+c

#define smallest 0.01
#define forget 0.5
#define elevation pi/16.0

Quaternion current;
float rotation;

int julquat(Quaternion z) {
  int k=0;
  bool over=false;
  do {
    k++;
    z=z*z+current;
    if (k>10)
      over=true;
  } while (!over && !(z.mod()>2.0));
  if (over)
    k=0;
  return k;
}

Quaternion searchline(Quaternion front,Quaternion back) {
  Quaternion t,mid;
  t.setto(front-back);
  if (t.mod()<smallest)
    return back;
  mid.setto((front+back)/2.0);
  // if (mid.c>forget)
  // return Quaternion(0,0,forget,0);
  if (julquat(mid)==0)
    return searchline(front,mid);
  else
  return searchline(mid,back);
}

void main() {
  int scrwid=320;
  int scrhei=200;
  int indent=(scrwid-scrhei)/2;
  float cenx=0;
  float ceny=0;
  float wid=1.0;
  float hei=1.0;
  float left=cenx-wid;
  float right=cenx+wid;
  float top=ceny-hei;
  float bottom=ceny+hei;
  float front=-1.0;
  float back=1.0;
  float scale=2.0/(2.0*front);
  float crazy=0.1234567;
  int steps=100;
  int jump=1;
  int frames=30;
  JBmp *b=new JBmp(scrwid,scrhei);
  allegrosetup(scrwid,scrhei);
  makepalette(&greypalette);
  _farsetsel(screen->seg);
  PALETTE pal;
  get_palette(pal);
  randomise();
  Map2d<float> *map=new Map2d<float>(scrhei,scrhei);
  current=*new Quaternion(-0.488193901,0.572800587,0,0);
  Quaternion qfrom=*new Quaternion(-0.12,0.7815,0,0);
  Quaternion qto=*new Quaternion(-1.1396,0.2361,0,0);
  V3d tmpnear,tmpfar;
  Quaternion near,far,last,next,res;
  for (int frame=0;frame<=frames;frame++) {
    float thru=(float)frame/(float)frames;
    current=qfrom*(1.0-thru)+qto*thru;
    float dslice=0.5*(1.0-thru);
    rotation=-pi/3.0+pi/8.0*thru;
    
    for (int i=0;i<scrhei;i+=jump) {
      for (int j=0;j<scrhei;j+=jump) {
        // b->setpixelud(indent+i,j,255);
        // b->writetoscreen();
        int col=0;
        tmpnear=V3d(left+2.0*wid*i/(float)scrhei,top+2.0*hei*j/(float)scrhei,front);
        tmpfar=V3d(tmpnear.x*2.0,tmpnear.y*2.0,back);
        tmpnear.changeto(V3d::rotate(tmpnear,V3d(0,1,0),rotation));
        tmpfar.changeto(V3d::rotate(tmpfar,V3d(0,1,0),rotation));
        tmpnear.changeto(V3d::rotate(tmpnear,V3d(1,0,0),elevation));
        tmpfar.changeto(V3d::rotate(tmpfar,V3d(1,0,0),elevation));
        near.setto(Quaternion(tmpnear.x,tmpnear.y,tmpnear.z,dslice));
        far.setto(Quaternion(tmpfar.x,tmpfar.y,tmpfar.z,dslice));
        map->pos[i][j]=crazy;
        last.setto(near-(far-near));
        for (int k=0;k<steps;k++) {
          float dk=(float)k/(float)steps;
          next.setto(near*(1.0-dk)+far*dk);
          if (julquat(next)==0) {
            res.setto(searchline(last,next));
            // col=255.0*(1.0-(res.c-front)/(forget-front));
            map->pos[i][j]=res.c;
            break;
          }
          last=next;
        }
        if (i>=jump && j>=jump) {
          if (map->pos[i-jump][j-jump]==crazy || map->pos[i-jump][j]==crazy || map->pos[i][j-jump]==crazy)
            col=0;
          else {
            float diff=2.0*map->pos[i-jump][j-jump]-map->pos[i-jump][j]-map->pos[i][j-jump];
            col=chop(128.0+diff*1600.0,0,255);
          }
          for (int x=0;x<jump;x++) {
            for (int y=0;y<jump;y++) {
              b->setpixel(indent+i+x,j+y,col);
            }
          }
        }
        if (key[KEY_SPACE])
          exit(0);
      }
      b->writetoscreen();
    }
    save_bitmap(getnextfilename("bmp"),screen,pal);
  }
  allegro_exit();
}
