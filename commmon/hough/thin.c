#include <string.h>
#include <stdio.h>

bool patternMatch(unsigned char* image, int i, int imageWidth)
{
  if ((image[i-imageWidth-1] > 0) && 
      (image[i-imageWidth+1] > 0) && 
      (image[i-imageWidth] > 0))
    return true;

  if((image[i+imageWidth-1] > 0) && 
     (image[i+imageWidth+1] > 0) && 
     (image[i+imageWidth] > 0))
    return true;

                
  if ((image[i-imageWidth-1] > 0) && 
      (image[i-1] > 0) && 
      (image[i+imageWidth-1] > 0))
    return true;
	  
  if((image[i-imageWidth+1] > 0) && 
     (image[i+1] > 0) && 
     (image[i+imageWidth+1] > 0))
    return true;
                                
  if ((image[i-imageWidth-1] > 0)
      && (image[i-imageWidth] > 0)
      && (image[i-1] > 0)
      && (image[i-imageWidth+1] == 0)
      && (image[i+imageWidth-1] == 0)
      && (image[i+imageWidth] == 0)
      && (image[i+1] == 0)
      && (image[i+imageWidth+1] == 0))
    return true;
                                
  if ((image[i+imageWidth-1] > 0)
      && (image[i+imageWidth] > 0)
      && (image[i-1] > 0)
      && (image[i-imageWidth-1] == 0)
      && (image[i+imageWidth+1] == 0)
      && (image[i-imageWidth] == 0)
      && (image[i+1] == 0)
      && (image[i-imageWidth+1] == 0))
    return true;
                                
  if ((image[i-imageWidth+1] == 0)
      && (image[i+1] > 0)
      && (image[i+imageWidth+1] > 0)
      && (image[i+imageWidth] > 0)
      && (image[i+imageWidth-1] == 0)
      && (image[i-imageWidth] == 0)
      && (image[i-1] == 0)
      && (image[i-imageWidth-1] == 0))
    return true;
                
  if ((image[i-imageWidth-1] == 0)
      && (image[i-imageWidth] > 0)
      && (image[i-imageWidth+1] > 0)
      && (image[i+1] > 0)
      && (image[i+imageWidth+1] == 0)
      && (image[i+imageWidth] == 0)
      && (image[i-1] == 0)
      && (image[i+imageWidth-1] == 0))
    return true;
  
  return false;
}       

unsigned char* thinEdges(unsigned char* gray, int height, int imageWidth) 
{
  unsigned char* image = new unsigned char[height*imageWidth];
  memset(image, 0, height*imageWidth);
  unsigned char* upd = NULL;
  int i, remain = 1;
  int removed = 0;
  int total = 0;
  do
    {
      remain = 0;
      for(int k=0; k<4; k++)
	{
	  for (i = imageWidth+1; i < height*imageWidth-imageWidth-2; i++)
	    {
	      // needs 4 passes in the core, must do 4 directions
	      // i.e. can't repeat the same direction consecutively
	      bool test = ((gray[i]>0) &&
			   (((gray[i + 1] == 0) && k==0)||
			    ((gray[i - 1] == 0) && k==1)||
			    ((gray[i + imageWidth] == 0) && k==2) ||
			    ((gray[i - imageWidth] == 0) && k==3) ));
	      
	      if (test && patternMatch(gray, i, imageWidth))
		{ image[i] = 0; 
		  removed++;
		  remain = true; }
	      else
		image[i] = (gray[i]>0);
	      
	      if(gray[i])
		total++;
	    }

	  // substitute the update to the original, 
	  // without changing the original
	  if(upd) { delete [] upd; upd = NULL; }

	  
	  upd = new unsigned char[height*imageWidth];
	  memcpy(upd, image, height*imageWidth);
	  gray = upd;
	}
    }
  while (0);  // 1 pass only

  printf("removed %d pixels out of %d (%f%%)\n", removed, total, (float)removed/total*100);
  if(upd)  delete []upd;
  return image;
}


#if 0
int main(void)
{
  unsigned char test[] = {0,0,0,0,0,0,0,0,
			  0,0,0,0,0,0,0,0,
			  0,0,1,1,1,1,0,0,
			  0,0,1,1,1,1,0,0,
			  0,0,0,0,0,0,0,0,
			  0,0,0,0,0,0,0,0};
  int w = 8;
  int h = 6;
  unsigned char* nouv = thinEdges(test, h, w);
  for(int i=0; i<h; i++)
    {
      for(int j=0; j<w; j++)
	printf("%d ", nouv[i*w+j]);
      printf("\n");
    }
  return 0;
}
#endif
