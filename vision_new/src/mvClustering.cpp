#include "mvLines.h"
#include "TriangularMatrix.h"
#include <cv.h>

//#define CLUSTERING_DEBUG

#define ABS(X) (((X) > 0) ? (X) : (-(X)))
/*
inline
int Cross_Product (int Ax, int Ay, int Bx, int By) {
    return Ax*By - Ay-Bx;
}
*/
inline
int Line_Difference_Metric (int x1,int y1, int x2,int y2, int x3,int y3, int x4,int y4) {
// this is 8x the area of the quadrilateral
    int a123 = (x2-x1)*(y3-y1) - (x3-x1)*(y2-y1);
    int a134 = (x3-x1)*(y4-y1) - (x4-x1)*(y3-y1);
    int a124 = (x2-x1)*(y4-y1) - (x4-x1)*(y2-y1);
    int a234 = (x3-x2)*(y4-y2) - (x4-x2)*(y3-y2);
    
    return ABS(a123) + ABS(a134) + ABS(a124) + ABS(a234);
}

