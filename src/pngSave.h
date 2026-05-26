#ifndef PNGSAVE_H
#define PNGSAVE_H

#ifdef __cplusplus
extern "C" {
#endif

bool savePNG(const char *filename, unsigned char *data, int sx, int sy, bool alpha);
bool PNGSaveWithAlpha( const char * filename, const unsigned char * data, int sx, int sy, int reverse = 0);
void downsample2x2(unsigned char * data, int sx, int sy);
void downsample2x2NoAlpha(unsigned char * data, int sx, int sy);

#ifdef __cplusplus
}
#endif

#endif
