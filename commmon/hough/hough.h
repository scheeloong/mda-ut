

#define N_ANGLE 512
#define MAX_LINES 200 
#define MAXSEG 256

#define PERLINE 5

void init_coord(short* );

int init_hough(int width, int height);
unsigned char* houghTransform(unsigned char* buf, int width, int height);

int peak(unsigned char* buf, int dim);

void init_lines(int width, int height);
unsigned char* findLines(unsigned char* buf, int num, int width, int height);

void draw_line(int Ax, int Ay, int Bx, int By, unsigned char Color, 
	       unsigned char* buf, int WIDTH, int HEIGHT, bool times3);


void make_rif (char* name, 
	       unsigned char* img, int width, int height);

short* process_image(unsigned char* orig, 
		     int width, int height,
		     int hue_min, int hue_max,
		     int only_black_white,
		     int remove_black_white,
		     int want_black,
		     int want_blob,
		     int blob_min_size,
		     int save);
void getCentroid(unsigned char* image,
		 int width, int height,
		 int min_size);
