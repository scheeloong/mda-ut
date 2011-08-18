#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include <stdio.h>
#include <iomanip>

using namespace std;


char *replace_str(char *str, char *orig, char *rep)
{
  static char buffer[4096];
  char *p;

  if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
    return str;

  strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
  buffer[p-str] = '\0';

  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

  return buffer;
}

// returns number of bits per pixel
int packing(char* name)
{
  int pack = 1;
  if(strstr(name, "hough"))
    pack = 8;  
  if(strstr(name, "orig"))
    pack = 24;
  if(strstr(name, "hsv"))
    pack = 24;
  return pack;
}

void make_rif (char* name, 
	       unsigned char* img, int width, int height)
{
  name = replace_str(name, "bmp", "rif");

  printf("saving %s\n", name);
  ofstream file_txtput(name, ios::out);
  
  if (file_txtput.fail())
    { cerr << "Error: Cannot open output file: " << name << endl; 
      exit (-1);}

  int pack = packing(name);

  switch(pack) {
  case 1:
    for (int i = 0; i <= (width/16*height); i++)
      {
	unsigned short accum = 0;
	for(int j=0; j<16; j++)
	  {
	    accum = (accum << 1) | (img[i*16+j] & 1);
	  }
	
	if(!(i%(width/16)))
	  file_txtput << endl << hex << "@" << i << endl;
	file_txtput << hex << setw(4) << setfill('0') << accum   << " ";
      }
    break;
  case 8:
    for (int i = 0; i <= (width/2*height); i++)
      {
	unsigned short accum = 0;
	for(int j=0; j<2; j++)
	  {
	    accum = (accum << 8) | (img[i*2+j] & 0xff);
	  }
	
	if(!(i%(width/2)))
	  file_txtput << endl << hex << "@" << i << endl;
	file_txtput << hex << setw(4) << setfill('0') << accum   << " ";
      }
    break;
  case 24:
    for (int i = 0; i <= (width*height); i++)
      {
	unsigned int accum = 0;
	for(int j=2; j>=0; j--)
	  {
	    accum = (accum << 8) | (img[i*3+j] & 0xff);
	  }
	
	if(!(i%(width)))
	  file_txtput << endl << hex << "@" << i << endl;
	file_txtput << hex << setw(6) << setfill('0') << accum   << " ";
      }
    break;
  default:
    cerr << "Error: unknown packing " << pack << endl; 
    exit (-1);    
  }

  file_txtput.close();
}
