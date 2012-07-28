#ifndef _MATRIX_
#define _MATRIX_

/** Matrix - simple implementation of a matrix with bounds checking.
 *  Most of this is copied from a C++ faq. 
 */
template <class Type>
class Matrix {
    Type* _data;
    unsigned _rows, _cols;
    
    public:
    Matrix (unsigned rows, unsigned cols);
    ~Matrix () { delete [] _data; }
    
    Type& operator() (unsigned r, unsigned c); // accessor operator
    Type operator() (unsigned r, unsigned c) const;
    
    unsigned getRows () { return _rows; }
    unsigned getCols () { return _cols; }
    void set (unsigned r, unsigned c, Type value);   // Assignment operator
    void setAll (Type value);
};

template <class Type>
inline
Matrix<Type>:: Matrix (unsigned rows, unsigned cols) : _rows(rows), _cols(cols) { 
    unsigned size = _rows*_cols;
    assert (size != 0);
    _data = new Type[size]; 
}

template <class Type>
inline
Type& Matrix<Type>:: operator() (unsigned r, unsigned c) {
    assert (r >= 0 && r < _rows);
    assert (c >= 0 && c < _cols);
    return _data[r*_cols + c];
}

template <class Type>
inline
Type Matrix<Type>:: operator() (unsigned r, unsigned c) const {
    assert (r >= 0 && r < _rows);
    assert (c >= 0 && c < _cols);
    return _data[r*_cols + c];
}

template <class Type>
inline
void Matrix<Type>:: set (unsigned r, unsigned c, Type value) {
    assert (r >= 0 && r < _rows);
    assert (c >= 0 && c < _cols);
    _data[r*_cols + c] = value;
}

template <class Type>
inline
void Matrix<Type>:: setAll (Type value) {
    for (unsigned r = 0; r < _rows; r++)
        for (unsigned c = 0; c < _cols; c++)
            _data[r*_cols + c] = value;
}

#endif