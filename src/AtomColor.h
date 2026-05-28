#include <cstdio>
#include <cstdlib>
#include <cstring>
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glew.h>
#include <GL/glu.h>



int getAtomColor(const char* atomicElement);
float getAtomRadius(const char* atomicElement);
float getAtomCovalentRadius(const char* atomicElement);

int getChainColor(int chianIndex);

#define MAX_COVALENT_RADIUS 1.688f

                         
bool readArtFile(const char* filename);                                  
                                                           
                                                           
                                                           
                                                           
                                                           
                                                           
                                                           
