#ifndef _MDA_TRIANGULARMATRIX_
#define _MDA_TRIANGULARMATRIX_

/** Triangular Matrix - fast implementation of a triangular matrix */
// Fast because it is all in a contiguous array (takes advantage of cache block)
// and it uses C style access and realloc
// Just use (i,j) to read values and use .set(i,j,value) to set values. 
// The matrix automatically grows, and newly allocated values are always
// initialized to 0.
// Note for a TriangularMatrix *mat, you can do (*mat)(i,j) for access.
class TriangularMatrix {
    #define STARTING_COLUMNS 100
    int * data;
    unsigned N_COLUMNS;
    
    // this gives u the size of the linear array needed to contain the matrix w N columns
    unsigned array_size_of (unsigned N) { return (N*N+N)/2 - 1; } 
    
    public:
    // constructor uses calloc, which initializes bits to zero
    TriangularMatrix () { 
        N_COLUMNS = STARTING_COLUMNS;
        data = (int*)calloc(array_size_of(STARTING_COLUMNS),sizeof(int)); 
    }
    
    ~TriangularMatrix () { 
        free(data); 
        data = NULL;
    }
    
    // accessor operator. It cannot be used to set an element
    int operator () (unsigned i, unsigned j) {
        assert (i > j);
        return data[i*(i-1)/2+j]; 
    }
    
    // use this to set (i,j) to a certain value
    void set (unsigned i, unsigned j, int value) {
        assert (i > j);
        if (i > N_COLUMNS) { // if array too small, allocate i+10 columns
            int new_size = array_size_of(i+10)+1; // len of array in # of ints
            int old_size = array_size_of(N_COLUMNS);
            
            data = (int*)realloc(data, new_size*sizeof(int));
            // the next line is just to give Java programmers stuff to complain about
            memset (data+old_size+1, 0, (new_size-old_size-1)*sizeof(int)); 
            N_COLUMNS = i+10;
        }
        data[i*(i-1)/2+j] = value;
    }
};

#endif