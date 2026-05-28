#include <cstdio>
#include <cstdlib>
#include <cstring>
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glew.h>
#include <GL/glu.h>



class GifWrapper{
public:
  GifWrapper();
  
  bool AddFrame(Byte* data, int sx, int sy, float dt);
  
  bool Save(const char* filename);
  
};