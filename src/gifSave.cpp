#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <GL/glew.h>
#include <GL/glu.h>

#include <gif_lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

typedef unsigned char Byte;

#include "gifSave.h"

static vector<vector<GifByteType> > frames;
static vector<int> delay;
static ColorMapObject* outputPalette;

GifWrapper::GifWrapper(){
  frames.clear();
  delay.clear();
}

static int gifsx, gifsy;
bool GifWrapper::AddFrame(Byte* data, int sx, int sy, float dt){
  gifsx=sx; gifsy=sy;
  unsigned int npix=sx*sy;
  int paletteSize=256;
  vector<GifByteType> output(npix);
  if (frames.size()==0) {
    vector<GifByteType> r(npix),g(npix),b(npix);
    for (int i=0, j=0; i<npix; i++){
      r[i]=data[j++]; g[i]=data[j++]; b[i]=data[j++];
    }
#if GIFLIB_MAJOR >= 5
    outputPalette = GifMakeMapObject(paletteSize, NULL);
    if (GifQuantizeBuffer(sx, sy, &paletteSize, &(r[0]),&(g[0]),&(b[0]), &(output[0]), outputPalette->Colors) == GIF_ERROR) return false;
#else
    outputPalette = MakeMapObject(paletteSize, NULL);
    if (QuantizeBuffer(sx, sy, &paletteSize, &(r[0]),&(g[0]),&(b[0]), &(output[0]), outputPalette->Colors) == GIF_ERROR) return false;
#endif
  } else {
    for (int i=0, j=0; i<npix; i++){
      int best=0; int bestdist=1000000;
      for (int c=0; c<256; c++){
        int dist= (data[j]-outputPalette->Colors[c].Red)*(data[j]-outputPalette->Colors[c].Red)+
                  (data[j+1]-outputPalette->Colors[c].Green)*(data[j+1]-outputPalette->Colors[c].Green)+
                  (data[j+2]-outputPalette->Colors[c].Blue)*(data[j+2]-outputPalette->Colors[c].Blue);
        if (dist<bestdist) {bestdist=dist; best=c;}
      }
      output[i]=best; j+=3;
    }
  }
  frames.push_back(output);
  delay.push_back(int(dt*100));
  return true;
}

bool GifWrapper::Save(const char* filename){
#if GIFLIB_MAJOR >= 5
  int error;
  GifFileType* GifFile = EGifOpenFileName(filename, false, &error);
#else
  GifFileType* GifFile = EGifOpenFileName(filename, false);
#endif
  if (!GifFile) return false;
  if (EGifPutScreenDesc(GifFile, gifsx, gifsy, 8, 0, outputPalette) == GIF_ERROR) return false;
  for (size_t ni=0; ni<frames.size(); ni++) {
    char suffix[4]; int d=delay[ni];
    suffix[0] = 0; suffix[1] = d & 0xff; suffix[2] = (d >> 8) & 0xff; suffix[3] = 0;
    if (EGifPutExtension(GifFile, GRAPHICS_EXT_FUNC_CODE, 4, suffix) == GIF_ERROR) return false;
    if (EGifPutImageDesc(GifFile, 0, 0, gifsx, gifsy, false, NULL) == GIF_ERROR) return false;
    for (int y = 0, j=(gifsy-1)*gifsx; y < gifsy; y++, j-=gifsx) {
      if (EGifPutLine(GifFile, &(frames[ni][j]), gifsx) == GIF_ERROR) return false;
    }
  }
#if GIFLIB_MAJOR == 5 && GIFLIB_MINOR >= 1 || GIFLIB_MAJOR > 5
  if (EGifCloseFile(GifFile, &error) == GIF_ERROR) return false;
#else
  if (EGifCloseFile(GifFile) == GIF_ERROR) return false;
#endif
  return true;       
}