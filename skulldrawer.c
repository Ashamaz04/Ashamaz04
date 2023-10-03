#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lodepng.h"

unsigned char* loadPng(const char*filename, unsigned int* width, unsigned int* height) {
  unsigned char*image = NULL;
  int error = lodepng_decode32_file(&image, width, height, filename);
  if(error){
    printf("error %u: %s\n", error, lodepng_error_text(error));
  }
  return image;
}

void writePng(const char*filename, const unsigned char*image, unsigned width, unsigned height) {
  unsigned char*png;
  size_t pngsize;

  int error = lodepng_encode32(&png, &pngsize, image, width, height);
  if(!error) {
    lodepng_save_file(png, pngsize, filename);
  }
  
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
  
  free(png);
  
}

void bright(int w, int h, unsigned char* p0, unsigned char* p1) {
    int i;
	for(i = 0; i < w * h; i++) {
        p1[i] = p0[4 * i];    
    }
}

void contrast_of_b_and_w(int w, int h, unsigned char* p1){
    int i, j;
	for(i = 2; i < h - 1; i++){
        for(j = 2; j < w - 1; j++){          
            if(p1[w * i + j] < 90) {
                p1[w * i + j] = 0;
            }
            if(p1[w * i + j] > 170) {
                p1[w * i + j] = 255;
            }
        }
    }
    return;
}


void pattern(int w, int h, unsigned char*p2, unsigned char*p3){
   int i;
    for(i=0; i < w * h; i++) {
        p3[i*4] = 2 * p2[i];
        p3[i*4+1] = p2[i]/2;
        p3[i*4+2] = p2[i] ;
        p3[i*4+3] = 255;
    }
    return;
}

void Gauss_distribution(int w, int h, unsigned char* p1, unsigned char* p2){
     int i, j;
	 for(i = 2; i < h - 1; i++){
        for(j = 2; j < w - 1; j++){
           p2[w*i+j]=
              0.0924 * p1[w*(i+1)+(j-1)]
             +0.1192 * p1[w*(i+1)+j]
             +0.0924 * p1[w*(i+1)+(j+1)]
             +0.1192 * p1[w*i+(j-1)]
             +0.1538 * p1[w*i+j]
             +0.1192 * p1[w*i+(j+1)]
             +0.0924 * p1[w*(i-1)+(j-1)]
             +0.1192 * p1[w*(i-1)+j]
             +0.0924 * p1[w*(i-1)+(j+1)];
        }
     }
   return;
}

void rotate_90(int h, int w, unsigned char* src, unsigned char* dest) {
    int i, j;
    unsigned char tmp;
    for(i = 0; i < h; i++) {
        for(j = 0; j < w; j++) {
            dest[4 * (j * h + i)] = src[4 * (i * w + j)];
            dest[4 * (j * h + i) + 1] = src[4 * (i * w + j) + 1];
            dest[4 * (j * h + i) + 2] = src[4 * (i * w + j) + 2];
            dest[4 * (j * h + i) + 3] = src[4 * (i * w + j) + 3];
        }
    }
    for(i = 0; i < h / 2; i++) {
        for(j = 0; j < w; j++) {
            tmp = dest[4 * (j * h + i)];
            dest[4 * (j * h + i)] = dest[4 * (j * h + h - 1 - i)];
            dest[4 * (j * h + h - 1 - i)] = tmp;
            
            tmp = dest[4 * (j * h + i) + 1];
            dest[4 * (j * h + i) + 1] = dest[4 * (j * h + h - 1 - i) + 1];
            dest[4 * (j * h + h - 1 - i) + 1] = tmp;
            
            tmp = dest[4 * (j * h + i) + 2];
            dest[4 * (j * h + i) + 2] = dest[4 * (j * h + h - 1 - i) + 2];
            dest[4 * (j * h + h - 1 - i) + 2] = tmp;
            
            tmp = dest[4 * (j * h + i) + 3];
            dest[4 * (j * h + i) + 3] = dest[4 * (j * h + h - 1 - i) + 3];
            dest[4 * (j * h + h - 1 - i) + 3] = tmp;
        }
    }
}

void background_to_green(int h, int w, unsigned char* src) {
    int i;
    for(i = 0; i < w * h; i++) {
        if(src[i*4] == 0 &&
           src[i*4+1] == 0 &&
           src[i*4+2] == 0)
        {
            src[i*4+1] = 255;
        }
    }
}

int main() {
    const char* filename = "teeth.png";
    unsigned int w, h;
    unsigned char* p = loadPng(filename, &w, &h);
    if (p == NULL){
        printf("I can't read %s. Error.\n", filename);
        return -1;
    }
    unsigned char *p1=(unsigned char*)malloc(h*w*sizeof(unsigned char));
    unsigned char *p2=(unsigned char*)malloc(h*w*sizeof(unsigned char));
    unsigned char *p3=(unsigned char*)malloc(h*w*4*sizeof(unsigned char));
    unsigned char *p_rot=(unsigned char*)malloc(h*w*4*sizeof(unsigned char));
    bright(w, h, p, p1);
    contrast_of_b_and_w(w, h, p1);
    Gauss_distribution(w, h, p1, p2);
    pattern(w, h, p2, p3);
    rotate_90(h, w, p3, p_rot);
    background_to_green(w, h, p_rot);
    writePng("rotate.png", p_rot, h, w);

    free(p1);
    free(p2);
    free(p3);
    free(p_rot);
    return 0;
}
