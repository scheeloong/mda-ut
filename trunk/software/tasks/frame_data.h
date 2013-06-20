#ifndef __MDA_VISION__MDA_FRAMEDATA__
#define __MDA_VISION__MDA_FRAMEDATA__

#include "mgui.h"
#include "mv.h"

// this class stores data that is remembered between frames for buoy
class MDA_FRAME_DATA {
public:
    // valid if one or more objects found
    bool m_valid;
    int n_circles;  // 0 or 1
    int n_boxes;    // 0, 1, or 2

    // each buoy frame can have a single circle
    MvCircle m_frame_circle;
    
    // each buoy frame can have 2 boxes. If one box is found it always goes into m_frame_box[0]
    // otherwise the left box goes in [0] and right box goes in [1]
    MvRotatedBox m_frame_box[2];

    MDA_FRAME_DATA () { m_valid = false; n_circles = n_boxes = 0; }
};

#endif