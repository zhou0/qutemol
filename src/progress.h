#include <cstdio>
#include <cstdlib>
#include <cstring>
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glew.h>
#include <GL/glu.h>


bool UpdateProgress(int i);

void StartProgress(char* str, int N);

void EndProgress();