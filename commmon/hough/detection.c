#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "detection.h"

HSV rgb_to_hsv(RGB rgb) {

    HSV hsv;
    unsigned char rgb_min, rgb_max;
    
    rgb_min = MIN3(rgb.red, rgb.green, rgb.blue);
    rgb_max = MAX3(rgb.red, rgb.green, rgb.blue);

    hsv.val = rgb_max;
    if (hsv.val == 0) {
        hsv.hue = hsv.sat = 0;
        return hsv;
    }

    hsv.sat = (unsigned char)(255.0*(1 - rgb_min/(float)hsv.val));
    if (hsv.sat == 0) {
        hsv.hue = 0;
        return hsv;
    }
    
    // Compute hue 
    if (rgb_max == rgb.red) {
        hsv.hue = 0 + 43*(rgb.green - rgb.blue)/(rgb_max - rgb_min);
    } else if (rgb_max == rgb.green) {
        hsv.hue = 85 + 43*(rgb.blue - rgb.red)/(rgb_max - rgb_min);
    } else  {//rgb_max == rgb.b 
        hsv.hue = 171 + 43*(rgb.red - rgb.green)/(rgb_max - rgb_min);
    }
    
    return hsv;
}




unsigned char* image_rgb_to_hsv(unsigned char* image, int num_pixels)
{  
  unsigned char* dest = image;
  

  for(int i = 0 ; i < num_pixels; i++) {
    int px = i * 3;
    RGB c1;

    c1.red = image[px + 0];
    c1.green = image[px + 1];
    c1.blue = image[px + 2];

    HSV c2 = rgb_to_hsv(c1);
    dest[px] = c2.hue;
    dest[px + 1] = c2.sat;
    dest[px + 2] = c2.val;
  }
  return dest;
}


unsigned char* is_RGB_black_or_white(unsigned char* image, 
				     int num_pixels, 
				     int want_black)
{
  unsigned char* dest = (unsigned char*)malloc(num_pixels);

  for(int i = 0 ; i < num_pixels; i++) {
    int px = i * 3;
    int black_thresh = 150;
    int white_thresh = 150;
    dest[i] = 0; 
    int found = 0;

    if ((image[px+0] < black_thresh) &&
	(image[px+1]< black_thresh) &&
	(image[px+2]< black_thresh))    //pixel is black
      {
	if(want_black)
	  {
	    dest[i] = 1; //black
	    found = 1;
	  }
      }
    else if ((image[px+0] > white_thresh) &&
	     (image[px+1] > white_thresh) &&
	     (image[px+2] > white_thresh)) //pixel is white
      {
	if(!want_black)
	  {
	    dest[i] = 1; //white
	    found = 1;
	  }
      }

    if(found) {
      image[px+0] = 255;
      image[px+1] = 255;
      image[px+2] = 255;
    }
    else {
      // set the image to show the result
      image[px+0] = 0;
      image[px+1] = 0;
      image[px+2] = 0;
    }
    
  }
  return dest;
}

#define NO_HUE 5
static inline int is_black_or_white(unsigned char* hsv) // single pixel
{
  if((  hsv[0]<=NO_HUE) && 
     (( hsv[1] <= NO_HUE) &&
      ((hsv[2] <= NO_HUE) || (hsv[2]>200))
      ))
    return 1; else return 0;
}

unsigned char* colorFilter(unsigned char* hsv, int num_pixels,
			   int minHue, int maxHue,
			   int only_black_white,
			   int remove_black_white)
{
  unsigned char* binary =  (unsigned char*)malloc(num_pixels);
  int p = 0;

  printf("filter minhue %d maxhue %d\n", minHue, maxHue);
  
  for(int i = 0; i < 3*num_pixels; i = i + 3)
    {
      if(only_black_white)
	binary[p] = is_black_or_white(&(hsv[i]));
      else
	{
	  if(minHue < maxHue)
	    binary[p] = ((hsv[i] >= minHue) && (hsv[i] <= maxHue));
	  else
	    binary[p] = ((hsv[i] <= maxHue) || (hsv[i] >= minHue));
	  // this adds white filtering
	  //&&(hsv[i + 1] > 50)&&(hsv[i + 2] < 200)
	  
	  if(remove_black_white) // take out black and white
	    {
	      if(is_black_or_white(&(hsv[i])))
		binary[p] = 0;
	    }
	}
      p++;
    }
  
  return binary;
}

unsigned char* sobelFilter(unsigned char* gray, int height, int width, int threshold) 
{
  unsigned char* binary = (unsigned char*)malloc(height*width);
  
  
  for(int i = 1; i < width - 1; i++) {
    for(int j = 1; j < height - 1; j++) {
      int x = 2* (int)gray[(j)*width + i + 1] + (int)gray[(j-1)*width + i + 1] 
        + (int)gray[(j+1)*width + i + 1] - 2*(int)gray[(j)*width + i - 1] 
        - (int)gray[(j+1)*width + i - 1] - (int)gray[(j-1)*width + i - 1];
      int y = 2* (int)gray[(j -1)*width + i] + (int)gray[(j-1)*width + i - 1] 
        + (int)gray[(j-1)*width + i + 1] - 2*(int)gray[(j +1)*width + i] 
        - (int)gray[(j +1)*width + i - 1] - (int)gray[(j+1)*width + i + 1];
      
      if((x*x + y*y) > threshold)
	binary[j*width + i] = 1;
      else
	binary[j*width + i] = 0;
    }
  }
  // clear the first row
  memset(binary, 0, width*sizeof(char));

  // clear the last row
  memset(&(binary[height*width - width]), 0, width*sizeof(char));

  // clear the two side columns
  for(int i = 0; i < height; i++){
    binary[i*width] = 0;
    binary[i*width + width -1] = 0;
  }
  
  return binary;
}

    


