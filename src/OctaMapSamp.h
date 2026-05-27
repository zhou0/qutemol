#ifndef OCTAMAPSAMP_H
#define OCTAMAPSAMP_H

#include "Common.h"
#include <vcg/space/point3.h>
#include <vector>

using namespace vcg;
using namespace std;

class OctaMapSamp{
public:
  
  int TotTexSizeX(){
    return size;
  }

  int TotTexSizeY(){
    return size;
  }

  static int size; // dim lato

  static vector<int> map;     // mappa 2d di indici a dir
  static vector<Point3f> dir;    // direzioni (uniche!)
  
  static void Init(int s);

  static int Index(int x, int y){ return x+y*size; }

  static void Get(int x, int y, Point3f &res){
    res=dir[map[Index(x,y)]];
  }
  
  void DuplicateTexels(vector<Byte> &t, int s, int tx, int ty){
  }

  void FillTexture(vector<Byte> &t, int s, int tx, int ty,
                   const vector<int> &sum, const vector<int> &div){
    for (int y=0; y<s; y++)
    for (int x=0; x<s; x++) {
      int i=Index(x,y);
      //Byte shade=(int)( col[ Index( x , y ) ] );
      int k=( (ty*s+y)*size*s+(tx*s+x) )*3;
      for (int q=0; q<3; q++) {
        //t[k++]= (Byte)q[q];
      }
    }
  }

  void Smooth(vector<Byte> &t, int s, int tx, int ty){
  }

  static void FillTexture(vector<Byte> &texture, const vector<int> &sumtable,
                          const vector<int> &divtable);
};

#endif
