#ifndef __MDA_VISION__MDA_FRAMEDATA__
#define __MDA_VISION__MDA_FRAMEDATA__

#include "mgui.h"
#include "mv.h"

// this class stores data that is remembered between frames for buoy
class MDA_FRAME_DATA {
public:
    // valid if one or more objects found
    bool valid;
    int n_circles;  // 0 or 1
    int n_boxes;    // 0, 1, or 2

    // each buoy frame can have a single circle
    MvCircle m_frame_circle;
    
    // each buoy frame can have 2 boxes. If one box is found it always goes into m_frame_box[0]
    // otherwise the left box goes in [0] and right box goes in [1]
    MvRotatedBox m_frame_box[2];

    MDA_FRAME_DATA () { valid = false; n_circles = n_boxes = 0; }
    
    MDA_FRAME_DATA& operator = (MDA_FRAME_DATA right) {
        this->valid = right.valid;
        this->n_circles = right.n_circles;
        this->n_boxes = right.n_boxes;
        this->m_frame_circle = right.m_frame_circle;
        this->m_frame_box[0] = right.m_frame_box[0];
        this->m_frame_box[1] = right.m_frame_box[1];
        return *this;
    }

    void assign_circle (MvCircle circle) {
        m_frame_circle = circle;
        n_circles = 1;
        valid = true;
    }
    void assign_rbox (MvRotatedBox rbox) {
        if (n_boxes > 0) {
            m_frame_box[1] = rbox;
            if (n_boxes < 2)
                n_boxes = 2;
        }
        else {
            m_frame_box[0] = rbox;
            n_boxes = 1;
        }
        valid = true;
    }
    void clear () {
        valid = false;
        n_circles = n_boxes = 0;
    }

    void drawOntoImage (IplImage* img) {
        if (n_circles > 0)
            m_frame_circle.drawOntoImage(img);
        if (n_boxes > 0)
            m_frame_box[0].drawOntoImage(img);
        if (n_boxes > 1)
            m_frame_box[1].drawOntoImage(img);
    }
    void print () {
        if (valid)
            printf ("FRAME_DATA: %d Circles, %d Boxes\n", n_circles, n_boxes);
        else
            printf ("FRAME_DATA: Invalid\n");
    }
};

inline void shift_frame_data (MDA_FRAME_DATA frame_data_vector[], int &read_index, int num_frames) {
    assert (read_index >= 0);
    assert (read_index < num_frames);
    
    read_index--;
    if (read_index < 0)
        read_index = num_frames-1;
    frame_data_vector[read_index].clear();
}

#endif