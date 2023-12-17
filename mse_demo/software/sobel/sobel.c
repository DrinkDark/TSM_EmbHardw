/*
 * sobel.c
 *
 *  Created on: Sep 12, 2015
 *      Author: theo
 */

#include <stdlib.h>
#include <stdio.h>
#include "io.h"
#include "system.h"

const char gx_array[3][3] = {{-1,0,1},
                             {-2,0,2},
                             {-1,0,1}};
const char gy_array[3][3] = { {1, 2, 1},
                              {0, 0, 0},
                             {-1,-2,-1}};

short *sobel_x_result;
short *sobel_y_result;
unsigned short *sobel_rgb565;
unsigned char *sobel_result;
int sobel_width;
int sobel_height;

void init_sobel_arrays(int width , int height) {
	int loop;
	sobel_width = width;
	sobel_height = height;
	if (sobel_x_result != NULL)
		free(sobel_x_result);
	sobel_x_result = (short *)malloc(width*height*sizeof(short));
	if (sobel_y_result != NULL)
		free(sobel_y_result);
	sobel_y_result = (short *)malloc(width*height*sizeof(short));
	if (sobel_result != NULL)
		free(sobel_result);
	sobel_result = (unsigned char *)malloc(width*height*sizeof(unsigned char));
	if (sobel_rgb565 != NULL)
		free(sobel_rgb565);
	sobel_rgb565 = (unsigned short *)malloc(width*height*sizeof(unsigned short));
	for (loop = 0 ; loop < width*height ; loop++) {
		sobel_x_result[loop] = 0;
		sobel_y_result[loop] = 0;
		sobel_result[loop] = 0;
		sobel_rgb565[loop] = 0;
	}
}

short sobel_mac( unsigned char *pixels,
                 int x,
                 int y,
                 const char *filter,
                 unsigned int width ) {
   short result = 0;

   //dy = -1
   result += filter[0]*
             pixels[(y-1)*width+(x-1)];
   result += filter[1]*
             pixels[(y-1)*width+x];
   result += filter[2]*
             pixels[(y-1)*width+(x+1)];
   //dy = 0
   result += filter[3]*
             pixels[y*width+(x-1)];
   result += filter[4]*
             pixels[y*width+x];
   result += filter[5]*
             pixels[y*width+(x+1)];
   //dy = 1
   result += filter[6]*
             pixels[(y+1)*width+(x-1)];
   result += filter[7]*
             pixels[(y+1)*width+x];
   result += filter[8]*
             pixels[(y+1)*width+(x+1)];

   return result;
}


void sobel_x(unsigned char *source) {
   int x, y;

   for (y = 1; y < (sobel_height - 1); y++) {
      for (x = 1; x < (sobel_width - 1); x++) {
         unsigned char *currentRow = source + y * sobel_width;
         short *outputPixel = sobel_x_result + y * sobel_width + x;

         *outputPixel =
            currentRow[x - 1 - sobel_width] * gx_array[0][0] +
            currentRow[x - sobel_width] * gx_array[0][1] +
            currentRow[x + 1 - sobel_width] * gx_array[0][2] +
            currentRow[x - 1] * gx_array[1][0] +
            currentRow[x] * gx_array[1][1] +
            currentRow[x + 1] * gx_array[1][2] +
            currentRow[x - 1 + sobel_width] * gx_array[2][0] +
            currentRow[x + sobel_width] * gx_array[2][1] +
            currentRow[x + 1 + sobel_width] * gx_array[2][2];
      }
   }
}



void sobel_x_with_rgb( unsigned char *source ) {
   int x,y;
   short result;

   for (y = 1 ; y < (sobel_height-1) ; y++) {
      for (x = 1 ; x < (sobel_width-1) ; x++) {
    	  result = sobel_mac(source,x,y,gx_array,sobel_width);
          sobel_x_result[y*sobel_width+x] = result;
          if (result < 0) {
        	  sobel_rgb565[y*sobel_width+x] = ((-result)>>2)<<5;
          } else {
        	  sobel_rgb565[y*sobel_width+x] = ((result>>3)&0x1F)<<11;
          }
      }
   }
}

void sobel_y(unsigned char *source) {
   int x, y;

   for (y = 1; y < (sobel_height - 1); y++) {
      for (x = 1; x < (sobel_width - 1); x++) {
         unsigned char *currentRow = source + y * sobel_width;
         short *outputPixel = sobel_y_result + y * sobel_width + x;

         *outputPixel =
            currentRow[x - 1 - sobel_width] * gy_array[0][0] +
            currentRow[x - sobel_width] * gy_array[0][1] +
            currentRow[x + 1 - sobel_width] * gy_array[0][2] +
            currentRow[x - 1] * gy_array[1][0] +
            currentRow[x] * gy_array[1][1] +
            currentRow[x + 1] * gy_array[1][2] +
            currentRow[x - 1 + sobel_width] * gy_array[2][0] +
            currentRow[x + sobel_width] * gy_array[2][1] +
            currentRow[x + 1 + sobel_width] * gy_array[2][2];
      }
   }
}



void sobel_y_with_rgb( unsigned char *source ) {
   int x,y;
   short result;

   for (y = 1 ; y < (sobel_height-1) ; y++) {
      for (x = 1 ; x < (sobel_width-1) ; x++) {
    	  result = sobel_mac(source,x,y,gy_array,sobel_width);
         sobel_y_result[y*sobel_width+x] = result;
         if (result < 0) {
       	  sobel_rgb565[y*sobel_width+x] = ((-result)>>2)<<5;
         } else {
       	  sobel_rgb565[y*sobel_width+x] = ((result>>3)&0x1F)<<11;
         }
      }
   }
}

void sobel_threshold(short threshold) {
	int x,y,arrayindex;
	short sum,value;
	for (y = 1 ; y < (sobel_height-1) ; y++) {
		for (x = 1 ; x < (sobel_width-1) ; x++) {
			arrayindex = (y*sobel_width)+x;
			value = sobel_x_result[arrayindex];
			sum = (value < 0) ? -value : value;
			value = sobel_y_result[arrayindex];
			sum += (value < 0) ? -value : value;
			sobel_result[arrayindex] = (sum > threshold) ? 0xFF : 0;
		}
	}
}

/*void sobel(unsigned char *source, short threshold) {
    int x, y;

    for (y = 1; y < (sobel_height - 1); y++) {
        unsigned char l1[sobel_width];
        unsigned char l2[sobel_width];
        unsigned char l3[sobel_width];

        memcpy(l1, source + (y - 1) * sobel_width, sobel_width);
        memcpy(l2, source + y * sobel_width, sobel_width);
        memcpy(l3, source + (y + 1) * sobel_width, sobel_width);

        for (x = 1; x < (sobel_width - 1); x++) {
            int arrayIndex = y * sobel_width + x;

            short gx = 	l1[x - 1] * gx_array[0][0] +
            			l1[x] * gx_array[0][1] +
						l1[x + 1] * gx_array[0][2] +
						l2[x - 1] * gx_array[1][0] +
						l2[x] * gx_array[1][1] +
						l2[x + 1] * gx_array[1][2] +
						l3[x - 1] * gx_array[2][0] +
						l3[x] * gx_array[2][1] +
						l3[x + 1] * gx_array[2][2];

            short gy = 	l1[x - 1] * gy_array[0][0] +
            			l1[x] * gy_array[0][1] +
						l1[x + 1] * gy_array[0][2] +
						l2[x - 1] * gy_array[1][0] +
						l2[x] * gy_array[1][1] +
						l2[x + 1] * gy_array[1][2] +
						l3[x - 1] * gy_array[2][0] +
						l3[x] * gy_array[2][1] +
						l3[x + 1] * gy_array[2][2];

            short sum = abs(gx) + abs(gy);

            sobel_result[arrayIndex] = ALT_CI_THRESHOLD_CI_0(sum, threshold);
        }
    }
}*/

void sobel(unsigned char *source, short threshold) {
    int x, y;

    for (y = 1; y < (sobel_height - 1); y++) {
        unsigned char l11;
        unsigned char l12;
        unsigned char l13;
        unsigned char l21;
        unsigned char l22;
        unsigned char l23;
        unsigned char l31;
        unsigned char l32;
        unsigned char l33;

        memcpy(l11, source + (y - 1), sizeof(l11));
        memcpy(l12, source + (y - 1) + sizeof(l11), sizeof(l11));
        memcpy(l13, source + (y - 1) + 2 * sizeof(l11), sizeof(l11));

        memcpy(l21, source + y, sizeof(l11));
        memcpy(l22, source + y + sizeof(l11), sizeof(l11));
        memcpy(l23, source + y + 2 * sizeof(l11), sizeof(l11));

        memcpy(l31, source + (y + 1), sizeof(l11));
        memcpy(l32, source + (y + 1) + sizeof(l11), sizeof(l11));
        memcpy(l33, source + (y + 1) + 2 * sizeof(l11), sizeof(l11));


        for (x = 1; x < (sobel_width - 1); x++) {
            int arrayIndex = y * sobel_width + x;

            short gx = 	l11 * gx_array[0][0] +
            			l12 * gx_array[0][1] +
						l13 * gx_array[0][2] +
						l21 * gx_array[1][0] +
						l22 * gx_array[1][1] +
						l23 * gx_array[1][2] +
						l31 * gx_array[2][0] +
						l32 * gx_array[2][1] +
						l33 * gx_array[2][2];

            short gy = 	l11 * gy_array[0][0] +
            			l12 * gy_array[0][1] +
						l13 * gy_array[0][2] +
						l21 * gy_array[1][0] +
						l22 * gy_array[1][1] +
						l23 * gy_array[1][2] +
						l31 * gy_array[2][0] +
						l32 * gy_array[2][1] +
						l33 * gy_array[2][2];

            short sum = abs(gx) + abs(gy);

            sobel_result[arrayIndex] = ALT_CI_THRESHOLD_CI_0(sum, threshold);
        }
    }
}


unsigned short *GetSobel_rgb() {
	return sobel_rgb565;
}

unsigned char *GetSobelResult() {
	return sobel_result;
}
