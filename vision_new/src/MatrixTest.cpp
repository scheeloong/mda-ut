/** sanity test for TriangularMatrix class */
#include <stdlib.h>
#include "mvLines.h"    
#include "TriangularMatrix.h"

int main () {
    int N = 121; // num of columns
    int N2 = 192;
    
    TriangularMatrix mat;
    mat.set(N,N-1, 0);    
    
    
    for (int i = 1; i <= N; i++) {
        for (int j = 0; j < i; j++) {
            printf ("Init: mat(%d,%d) = 0?\n", i,j);
            assert (mat(i,j) == 0); // matrix init correctly?
        }
    }
    
    N = N2;
    
    // write values to each element
    for (int i = 1; i <= N; i++) {
        for (int j = 0; j < i; j++) {
            mat.set(i,j, i*j);
            printf ("Write: (%d,%d) = %d\n", i,j,i*j);
        }
    }
    
    printf ("Randomly writing values\n");
    // write values at random. should not overwrite with wrong values
    for (int i = 1; i <= N*100; i++) {
        int x = 1 + rand()%N; // from 1 to N inclusive
        int y =     rand()%x;  // from 0 to i-1
        mat.set(x,y, x*y);
    }
    
    for (int i = 1; i <= N; i++) {
        for (int j = 0; j < i; j++) {
            printf ("Read: mat(%d,%d) = %d\n", i,j, mat(i,j));
            assert (mat(i,j) == i*j); // matrix 
        }
    }
    
    mat.set(N+12,N+11, 0);
    
    for (int i = 1; i <= N; i++) {
        for (int j = 0; j < i; j++) {
            printf ("Realloc'd: mat(%d,%d) = %d\n", i,j, mat(i,j));
            assert (mat(i,j) == i*j); // matrix 
        }
    }
    
    printf ("TriangularMatrix Test Passed!\n");
    return 0;
}