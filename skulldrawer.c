#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lodepng.h"

char* loadPng(const char*filename, int*width, int*height) {
  unsigned char*image = NULL;
  int error = lodepng_decode32_file(&image, width, height, filename);
  if(error){
    printf("error %u: %s\n", error, lodepng_error_text(error));
  }
  return (image);
}

void writePng(const char*filename, const unsigned char*image, unsigned width, unsigned height) {
  unsigned char*png;
  long unsigned int pngsize;

  int error = lodepng_encode32(&png, &pngsize, image, width, height);
  if(!error) {
    lodepng_save_file(png, pngsize, filename);
  }
  
  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
  
  free(png);
  
}

void bright(int w, int h,unsigned char*p0,unsigned char *p1){
    int i;
	for( i=0;i<w*h;i++){
         
    p1[i]=0.2*p0[4*i]+0.7*p0[4*i+1]+0.4*p0[4*i+2];
      
    }
    return;
}

void contrast_of_b_and_w(int w,int h,unsigned char*p1){
    int i,j;
	for( i=2;i<h-1;i++){
        for( j=2;j<w-1;j++){
          
            if(p1[w*i+j]<90){
                p1[w*i+j]=0;
            }
            if(p1[w*i+j]>170){
                p1[w*i+j]=255;
            }
        }
    }
    return;
}

void pattern(int w, int h,unsigned char*p2,unsigned char*p3){
   int i;
    for( i=0;i<w*h;i++) {
        p3[i*4] = 270;
        p3[i*4+1] = 2*p2[i];
        p3[i*4+2] = p2[i];
        p3[i*4+3] = 255;
    }
    return;
}


void Gauss_distribution(int w,int h,unsigned char*p1, unsigned char*p2){
     int i,j;
	 for( i = 2;i < h-1 ;i++){
        for( j = 2;j < w-1 ;j++){
           p2[w*i+j]=
              0.09*p1[w*(i+1)+(j-1)]
             +0.121*p1[w*(i+1)+j]
             +0.012*p1[w*(i+1)+(j+1)]
             +0.092*p1[w*i+(j-1)]
             +0.146*p1[w*i+j]
             +0.092*p1[w*i+(j+1)]
             +0.121*p1[w*(i-1)+(j-1)]
             +0.192*p1[w*(i-1)+j]
             +0.192*p1[w*(i-1)+(j+1)];
        }
     }
   return;
}

int main() {
    char*filename = "skull.png";
    int w,h;
    char*p = loadPng(filename, &w, &h);
    if (p == NULL){
        printf("I can't read %s. Error.\n", filename);
        return -1;
    }
    unsigned char *p1=(unsigned char*)malloc(h*w*sizeof(unsigned char));
    unsigned char *p2=(unsigned char*)malloc(h*w*sizeof(unsigned char));
    unsigned char *p3=(unsigned char*)malloc(h*w*4*sizeof(unsigned char));
    contrast_of_b_and_w(w,h,p1);
    bright(w,h,p,p1);
    Gauss_distribution(w,h,p1,p2);
    pattern(w,h,p2,p3);
    char*new_image="skull-modified.png";
    print(new_image,p3,w,h);

    p1(free);
    p2(free);
    p3(free);
    
    return 0;
}
