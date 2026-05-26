#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <GL/glew.h>
#include <GL/glu.h>

#include "Common.h"
#include "CgUtil.h"

#include <math.h>

#include <vector>

#include <vcg/space/point3.h>
#include <vcg/space/color4.h>

#include "gen_normal.h"
#include <wrap/gui/trackball.h>

using namespace vcg;
using namespace std;

#include "CubeMapSamp.h"
#include "OctaMapSamp.h"
#include "Mol.h"
//#include "AOgpu.h"

#include "HardSettings.h"
#include "MyCanvas.h"

#include "ShadowMap.h"

extern Mol mol;

GeoSettings geoSettings; // singleton

void GeoSettings::Apply() {
   if (mol.IsReady()) {
     if (mode==BALL_N_STICKS) {
       mol.SetBallAndSticks(stickRadius);
     }
     if (mode==LICORICE) {
       mol.SetLicorice(licoRadius);
     }
     if (mode==SPACE_FILL) mol.SetSpaceFill();
   }
   
   cgSettings.setGeoSettings(*this);
   cgSettings.UpdateShaders();
   
   UpdateShadowmap();
   
   mol.ResetAO();
}

void SetColMode(float mode){
  mol.SetColMode(mode);
}

void ResetColMode(){
  float tmp=mol.colMode;
  mol.colMode=-1;
  mol.SetColMode(tmp);
}

float GetColMode(){
  return mol.colMode;
}

void UpdateNormalMaps() {
  mol.BuildNormalMaps();
}

void RebuildMol() {
  mol.Bind();
}

void UpdateAtomColor() {
  mol.UpdateAtomColor();
}

void StartAO() {
  mol.ResetAO();
}

void StopAO() {
  //mol.StopAO();
}

static int lastT=0;

long int getTicks() {
  return 0; // TODO!
}

long int TakeTime(FILE *f, char *st){
  return 0;
}

long int TakeTotalTime(){
  return 0;
}

void ResetAll() {
  UpdateShadowmap();
  mol.ResetAO();
}

void UpdateShadowmap(){
  shadowSettings.setGeoSettings(geoSettings);
  shadowSettingsAcc.setGeoSettings(geoSettings);
  shadowSettings.UpdateShaders();
  shadowSettingsAcc.UpdateShaders();
  mol.ResetAO();
}

void ChangeColorSchema(int i) {
  mol.ChangeColorSchema(i);
  mol.UpdateAtomColor();
  mol.ResetAO();
}

void Cycle(float &c, float min, float max, float step) {
  c+=step;
  if (c>max) c=min;
  if (c<min) c=max;
}

Byte* GetSnapshot(int sx, int sy, bool alpha) {
  Byte* res = new Byte[sx*sy*4];
  glReadPixels(0,0,sx,sy,GL_RGBA,GL_UNSIGNED_BYTE,res);
  return res;
}
