#ifndef GIFSAVE_H
#define GIFSAVE_H

#include "Common.h"

class GifWrapper{
public:
  GifWrapper();
  
  bool AddFrame(Byte* data, int sx, int sy, float dt);
  
  bool Save(const char* filename);
  
};

#endif
