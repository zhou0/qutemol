#ifndef COMMON_H
#define COMMON_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <GL/glew.h>
#include <GL/glu.h>

#include <vector>

extern int winx, winy;

typedef unsigned char Byte;

// SDL functions: remiplement these
//void swapbuffers();
//void refresh();
long int getTicks();



// drawing & geom settings...
class CgUtil;
class GeoSettings;

extern CgUtil cgSettings;
extern GeoSettings geoSettings; // singleton


void ResetAll();
void UpdateNormalMaps();

void UpdateAtomColor();

void StartAO();
void StopAO();

void RebuildMol();

long int TakeTime(FILE *f, char *st);
long int TakeTotalTime();
void  Cycle(  float &c, float min, float max, float step);

void SetColMode(float mode);
float GetColMode();
void ResetColMode();

void ChangeColorSchema(int i=-1);



//extern std::vector<Byte> texture;

//bool LoadImagePPM( const char * filename , std::vector<Byte> &im);
//bool SaveImagePPM( const char * filename , const std::vector<Byte> &im, int sizex, int sizey);

Byte* GetSnapshot(int sx, int sy, bool alpha);

void UpdateShadowmap();
extern CgUtil shadowSettings;
extern CgUtil shadowSettingsAcc;

#endif
