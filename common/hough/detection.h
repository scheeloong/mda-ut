
#define ORANGE_MIN_HUE 15
#define ORANGE_MAX_HUE 50

#define MIN3(x,y,z)  ((y) <= (z) ? \
                         ((x) <= (y) ? (x) : (y)) \
                     : \
                         ((x) <= (z) ? (x) : (z)))

#define MAX3(x,y,z)  ((y) >= (z) ? \
                         ((x) >= (y) ? (x) : (y)) \
                     : \
                         ((x) >= (z) ? (x) : (z)))



typedef struct  {
    unsigned char red, green, blue;    /* Channel intensities between 0 and 255 */
} RGB;

typedef struct  {
    unsigned char hue;        /* Hue degree between 0 and 255 */
    unsigned char sat;        /* Saturation between 0 (gray) and 255 */
    unsigned char val;        /* Value between 0 (black) and 255 */
} HSV;


unsigned char* image_rgb_to_hsv(unsigned char* image, int num_pixels);
unsigned char* colorFilter(unsigned char* hsv, int num_pixels, int minHue,
			   int maxHue,
			   int only_black_white,
			   int remove_black_white);
unsigned char* is_RGB_black_or_white(unsigned char* image, 
				     int num_pixels, 
				     int want_black);

unsigned char* sobelFilter(unsigned char* gray, int height, int width, int threshold);

unsigned char* thinEdges(unsigned char* gray, int height, int imageWidth);


void save_image(char* name, 
		unsigned char* img, int width, int height, int binary,
		unsigned char* overlayed);
