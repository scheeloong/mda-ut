/**
* @file planks.h
*
* @brief Provides location of planks, boxes and exit octogon
*
* 5 boxes, 4 boards and 1 exit location are given
*/

// planks:
world_vector b_positions[NUM_BOARDS] =
{
   {7.1, 0.000000, -4.6},
   {6.9, 0.000000, -5.9},
   {10.8, 0.000000, -7.1},
   {12.2, 0.000000, -9.4},
   {10.6, 0.000000, -1.5},
   {-3.3, 0.000000, 3},
   {1, 0.000000, -4.2},
   {10.1, 0.000000, -1.9},
};
// boxes:
world_vector bx_positions[NUM_BOXES] =
{
   {10.978841, 0.000000, -8.192278},
   {10.878841, 0.000000, -8.992278},
   {10.778841, 0.000000, -9.792278},
   {10.678841, 0.000000, -10.592278}
};
world_vector Exit_v =
   {15.352598, 4.876800, -11.080584}
   ;
