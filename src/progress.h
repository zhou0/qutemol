#include <cstdio>
#include <cstdlib>
#include <cstring>
#ifdef WIN32
#include <windows.h>
#endif
#include "qopengl.h"


bool UpdateProgress(int i);

void StartProgress(char* str, int N);

void EndProgress();