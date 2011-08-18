/**
* @file planks.c
*
* @brief Debugging program for planks
*
* Run this using make plancks in the sim directory
* planks == path marker
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// plank length
#define PL 1.2192
// box length
#define BL 0.6096

#define NUM_BOARDS 5
#define NUM_BOXES 2

typedef struct
{
   float x;
   float y;
   float z;
} world_vector;

#define FIRST_POS 1

// plank position
world_vector positions[NUM_BOARDS] =
{
   {0, 0.0, -1},
   {4.404640, 0.000000, -3.650495},
   {2, 0.0, -2},
   {3, 0.0, -2},
   {4, 0.0, -3}
};

#define FACT (3.1416/180)
// plank rotation
float rotations[NUM_BOARDS] = {30*FACT, 70*FACT, 45*FACT, 75*FACT, 90*FACT};

world_vector planks[NUM_BOARDS], boxes[NUM_BOXES], X[1];

void print_vect(world_vector* addr, int num)
{
   for (int k=0; k<num; k++)
   {
      printf("{%f, %f, %f}", addr[k].x, addr[k].y, addr[k].z);
      if (k != (num-1)) printf(",");
      printf("\n");
   }
}

void set_pos(world_vector* addr, int i, float x, float y, float z)
{
   addr[i].x = x;
   addr[i].y = y;
   addr[i].z = z;
}

int next_b[3] = {0,2,100};

int main(void)
{
   int i;
   int box_cnt=0;
   float x,y,z;
   x = positions[FIRST_POS].x;
   y = positions[FIRST_POS].y;
   z = positions[FIRST_POS].z;

   srand(10);

   for (i = 0; i<NUM_BOARDS; i++)
   {
      float ext =   (2.0 * (rand() / (RAND_MAX + 1.0)));

      x += PL/2*sin(rotations[i]);
      z -= PL/2*cos(rotations[i]);

      set_pos(planks, i, x, y, z);

      x += PL/4*sin(rotations[i]);
      z -= PL/4*cos(rotations[i]);

      x += ext*sin(rotations[i]);
      z -= ext*cos(rotations[i]);

      if (i == next_b[box_cnt])
      {
         x += BL/2*sin(rotations[i]);
         z -= BL/2*cos(rotations[i]);

         set_pos(boxes, box_cnt, x, y, z);
         box_cnt++;
      }

      x += ext*sin(rotations[i]);
      z -= ext*cos(rotations[i]);
   }

   printf("//planks:\n");
   printf("world_vector b_positions[NUM_BOARDS] = {\n");
   print_vect(planks, NUM_BOARDS);
   printf("};\n");
   printf("//boxes:\n");
   printf("world_vector bx_positions[NUM_BOXES] = {\n");
   print_vect(boxes, NUM_BOXES);
   printf("};\n");

   float ext =   (3.0 * (rand() / (RAND_MAX + 1.0)));

   x += ext*sin(rotations[i]);
   z -= ext*cos(rotations[i]);
   set_pos(X, 0, x, .9, z);

   printf("world_vector X_v =\n");
   print_vect(X,1);
   printf(";\n");

   return 0;
}
