
int blob_size;
int blob_x;
int blob_y;
int blob_seen;

void getCentroid(unsigned char* image,
		 int width, int height,
		 int min_size)
{
  int x_weight = 0;
  int y_weight = 0;

  int y_values[height];
  blob_size = 0;

  for(int j = 1; j < height; j++)
    y_values[j] = 0;

  for(int i = 0; i < width; i++){
    int linesum = 0;
    for(int j = 0; j < height; j++){
      linesum = linesum + image[j*width + i];
      y_values[j] = y_values[j] + image[j*width + i]; 
    }
    blob_size += linesum;
    x_weight = x_weight + i * linesum;
  }

  if(blob_size < min_size)
    {
      blob_seen = 0;
      return;
    }

  blob_seen = 1;

  for(int j = 1; j < height; j++)
    y_weight = y_weight + y_values[j]*j;

  if(blob_size){
    blob_x =(int)(x_weight/blob_size);
    blob_y =(int)(y_weight/blob_size);
  }
  else
    {
      blob_x = 0;
      blob_y = 0;
    }
}
