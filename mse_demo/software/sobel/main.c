#include <stdio.h>
#include <system.h>
#include <stdlib.h>
#include <io.h>
#include "lcd_simple.h"
#include "grayscale.h"
#include "i2c.h"
#include "camera.h"
#include "vga.h"
#include "dipswitch.h"
#include "sobel.h"
#include "sys/alt_timestamp.h"
#include "alt_types.h"

int main()
{
  void *buffer1,*buffer2,*buffer3,*buffer4;
  unsigned short *image;
  unsigned char *grayscale;
  unsigned char current_mode;
  unsigned char mode;

  //ProfileTimer timestamp variable
  alt_u32 start_grayscale;
  alt_u32 stop_grayscale;
  alt_u32 start_sobel_x;
  alt_u32 stop_sobel_x;
  alt_u32 start_sobel_y;
  alt_u32 stop_sobel_y;
  alt_u32 start_sobel_threshold;
  alt_u32 stop_sobel_threshold;

  alt_u32 meanCycles = 0;
  float meanFPS = 0;
  int nbrMean = 0;

  init_LCD();
  init_camera();
  vga_set_swap(VGA_QuarterScreen|VGA_Grayscale);
  alt_timestamp_start();

  printf("Hello from Nios II!\n");
  cam_get_profiling();
  buffer1 = (void *) malloc(cam_get_xsize()*cam_get_ysize());
  buffer2 = (void *) malloc(cam_get_xsize()*cam_get_ysize());
  buffer3 = (void *) malloc(cam_get_xsize()*cam_get_ysize());
  buffer4 = (void *) malloc(cam_get_xsize()*cam_get_ysize());
  cam_set_image_pointer(0,buffer1);
  cam_set_image_pointer(1,buffer2);
  cam_set_image_pointer(2,buffer3);
  cam_set_image_pointer(3,buffer4);
  enable_continues_mode();
  init_sobel_arrays(cam_get_xsize()>>1,cam_get_ysize());
  do {
	  if (new_image_available() != 0) {
		  if (current_image_valid()!=0) {
			  current_mode = DIPSW_get_value();
			  mode = current_mode&(DIPSW_SW1_MASK|DIPSW_SW3_MASK|DIPSW_SW2_MASK);
			  image = (unsigned short*)current_image_pointer();
		      switch (mode) {
		      case 0 : transfer_LCD_with_dma(&image[16520],
		                	cam_get_xsize()>>1,
		                	cam_get_ysize(),0);
		      	  	   if ((current_mode&DIPSW_SW8_MASK)!=0) {
		      	  		  vga_set_swap(VGA_QuarterScreen);
		      	  		  vga_set_pointer(image);
		      	  	   }
		      	  	   break;
		      case 1 : conv_grayscale((void *)image,
		    		                  cam_get_xsize()>>1,
		    		                  cam_get_ysize());
		               grayscale = get_grayscale_picture();
		               transfer_LCD_with_dma(&grayscale[16520],
		      		                	cam_get_xsize()>>1,
		      		                	cam_get_ysize(),1);
		      	  	   if ((current_mode&DIPSW_SW8_MASK)!=0) {
		      	  		  vga_set_swap(VGA_QuarterScreen|VGA_Grayscale);
		      	  		  vga_set_pointer(grayscale);
		      	  	   }
		      	  	   break;
		      case 2 : conv_grayscale((void *)image,
		    		                  cam_get_xsize()>>1,
		    		                  cam_get_ysize());
		               grayscale = get_grayscale_picture();
		               sobel_x_with_rgb(grayscale);
		               image = GetSobel_rgb();
		               transfer_LCD_with_dma(&image[16520],
		      		                	cam_get_xsize()>>1,
		      		                	cam_get_ysize(),0);
		      	  	   if ((current_mode&DIPSW_SW8_MASK)!=0) {
		      	  		  vga_set_swap(VGA_QuarterScreen);
		      	  		  vga_set_pointer(image);
		      	  	   }
		      	  	   break;
		      case 3 : conv_grayscale((void *)image,
		    		                  cam_get_xsize()>>1,
		    		                  cam_get_ysize());
		               grayscale = get_grayscale_picture();
		               sobel_x(grayscale);
		               sobel_y_with_rgb(grayscale);
		               image = GetSobel_rgb();
		               transfer_LCD_with_dma(&image[16520],
		      		                	cam_get_xsize()>>1,
		      		                	cam_get_ysize(),0);
		      	  	   if ((current_mode&DIPSW_SW8_MASK)!=0) {
		      	  		  vga_set_swap(VGA_QuarterScreen);
		      	  		  vga_set_pointer(image);
		      	  	   }
		      	  	   break;
		      default: alt_timestamp_start();
		    	  	   start_grayscale = alt_timestamp();
		    	  	   conv_grayscale((void *)image,
	                                  cam_get_xsize()>>1,
	                                  cam_get_ysize());
                       grayscale = get_grayscale_picture();
		    	  	   stop_grayscale = alt_timestamp();
		    	  	   printf("Grayscale : %d cycles/pixel\n", ((stop_grayscale - start_grayscale) / 196608));

		    	  	   /*start_sobel_x = alt_timestamp();
                       sobel_x(grayscale);
                       stop_sobel_x = alt_timestamp();
                       printf("Sobel x : %d cycles/pixel \n", ((stop_sobel_x - start_sobel_x) / 196608));

                       start_sobel_y = alt_timestamp();
					   sobel_y(grayscale);
					   stop_sobel_y = alt_timestamp();
					   printf("Sobel y : %d cycles/pixel \n", ((stop_sobel_y - start_sobel_y) / 196608));

					   start_sobel_threshold = alt_timestamp();
					   sobel_threshold(128);
					   stop_sobel_threshold = alt_timestamp();
					   printf("Sobel threshold : %d cycles/pixel \n", ((stop_sobel_threshold - start_sobel_threshold) / 196608));
					   */

		    	  	   start_sobel_x = alt_timestamp();
		    	  	   sobel(grayscale, 128);
		    	  	   stop_sobel_x = alt_timestamp();
		    	  	   printf("Sobel complete: %d cycles/pixel \n", ((stop_sobel_x - start_sobel_x) / 196608));

                       grayscale=GetSobelResult();
		               transfer_LCD_with_dma(&grayscale[16520],
		      		                	cam_get_xsize()>>1,
		      		                	cam_get_ysize(),1);
		      	  	   if ((current_mode&DIPSW_SW8_MASK)!=0) {
		      	  		  vga_set_swap(VGA_QuarterScreen|VGA_Grayscale);
		      	  		  vga_set_pointer(grayscale);
		      	  	   }

		      	  	   printf("Total : %d cycles/pixel\n", ((stop_grayscale - start_grayscale)+(stop_sobel_x - start_sobel_x)+(stop_sobel_y - start_sobel_y)+(stop_sobel_threshold - start_sobel_threshold)) / 196608);
		      	  	   printf("FPS : %.*f \n",2 , (float)ALT_CPU_FREQ / ((stop_grayscale - start_grayscale)+(stop_sobel_x - start_sobel_x)+(stop_sobel_y - start_sobel_y)+(stop_sobel_threshold - start_sobel_threshold)));
		      	  	   meanCycles += (((stop_grayscale - start_grayscale)+(stop_sobel_x - start_sobel_x)+(stop_sobel_y - start_sobel_y)+(stop_sobel_threshold - start_sobel_threshold)) / 196608);
		      	  	   meanFPS += (float)ALT_CPU_FREQ / ((stop_grayscale - start_grayscale)+(stop_sobel_x - start_sobel_x)+(stop_sobel_y - start_sobel_y)+(stop_sobel_threshold - start_sobel_threshold));
		      	  	   nbrMean++;
		      	  	   printf("Mean : %d cycles/pixel, %.*f FPS -> for %d samples\n\n", (meanCycles / nbrMean), 2 , (meanFPS / nbrMean), nbrMean);

		      	  	   break;
		      }
		  }
	  }
  } while (1);
  return 0;
}
