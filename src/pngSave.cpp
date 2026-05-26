#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <GL/glew.h>
#include <GL/glu.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <vector>
#include <iostream>
#include <png.h>
#include "progress.h"
#include "Common.h"

using namespace std;

void downsample2x2(unsigned char * data, int sx, int sy){
  int j=0;
  for (int y=0; y<sy/2; y++)
  for (int x=0; x<sx/2; x++)
  for (int c=0; c <4; c++) {
    data[j++]=(
      int(data[((y*2+0)*sx+(x*2+0))*4+c])+
      int(data[((y*2+0)*sx+(x*2+1))*4+c])+
      int(data[((y*2+1)*sx+(x*2+0))*4+c])+
      int(data[((y*2+1)*sx+(x*2+1))*4+c])+
      2
      )/4;
  }
}

void downsample2x2NoAlpha(unsigned char * data, int sx, int sy){
  int j=0;
  for (int y=0; y<sy/2; y++)
  for (int x=0; x<sx/2; x++)
  for (int c=0; c <3; c++) {
    data[j++]=(
      int(data[((y*2+0)*sx+(x*2+0))*3+c])+
      int(data[((y*2+0)*sx+(x*2+1))*3+c])+
      int(data[((y*2+1)*sx+(x*2+0))*3+c])+
      int(data[((y*2+1)*sx+(x*2+1))*3+c])+
      2
      )/4;
  }
}

bool savePNG(const char *filename, Byte *data, int sx, int sy, bool alpha) {
  FILE *fp;
  png_structp png_ptr;
  png_infop info_ptr;

  fp = fopen(filename, "wb");
  if (fp == NULL) return false;

  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    fclose(fp);
    return false;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    fclose(fp);
    png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
    return false;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    fclose(fp);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return false;
  }

  png_init_io(png_ptr, fp);

  png_set_IHDR(png_ptr, info_ptr, sx, sy,
    8, (alpha)?PNG_COLOR_TYPE_RGB_ALPHA:PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  vector<png_bytep> row_pointers(sy);
  for (int y=0; y<sy; y++)
    row_pointers[sy-1-y] = (png_bytep)(data + y*sx*((alpha)?4:3));

  png_write_image(png_ptr, &row_pointers[0]);
  png_write_end(png_ptr, NULL);

  fclose(fp);
  png_destroy_write_struct(&png_ptr, &info_ptr);

  return true;
}

bool PNGSaveWithAlpha( const char * filename, const Byte * data, int sx, int sy, int reverse) {
  return savePNG(filename, (Byte*)data, sx, sy, true);
}
