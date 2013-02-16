#include "mda_vision.h"

#define VISION_DEBUG
#ifdef VISION_DEBUG
    #define DEBUG_PRINT(format, ...) printf(format, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(format, ...)
#endif

const char MDA_VISION_MODULE_FRAME::MDA_VISION_FRAME_SETTINGS[] = "vision_frame_settings.csv";

/// #########################################################################
/// MODULE_FRAME methods
/// #########################################################################
MDA_VISION_MODULE_FRAME:: MDA_VISION_MODULE_FRAME () :
    _window (mvWindow("Frame Vision Module")),
    _MeanShift (mvMeanShift(MDA_VISION_FRAME_SETTINGS)),
    _HoughLines (mvHoughLines(MDA_VISION_FRAME_SETTINGS)),
    _lines (mvLines())
{
    _filtered_img = mvGetScratchImage (); // common size
}

MDA_VISION_MODULE_FRAME:: ~MDA_VISION_MODULE_FRAME () {
    mvReleaseScratchImage();
}

void MDA_VISION_MODULE_FRAME:: primary_filter (IplImage* src) {
    _MeanShift.filter (src, _filtered_img);
    _filtered_img->origin = src->origin;
    _lines.clearData ();
    _KMeans.clearData ();
    
    _HoughLines.findLines (_filtered_img, &_lines);
    _KMeans.cluster_auto (1, 3, &_lines, 1);
}

MDA_VISION_RETURN_CODE MDA_VISION_MODULE_FRAME:: calc_vci () {
    MDA_VISION_RETURN_CODE retval = FATAL_ERROR;
    unsigned nClusters = _KMeans.nClusters();
    int imWidth  =  (int)_filtered_img->width*0.5;
    int imHeight =  (int)_filtered_img->height*0.5;

    // for _IsRed, we check any vertical segments we find to see if they are red
    // We look near the centroid of the vert. segment, and check that the pixels 
    // are set to value I * mvMeanShift::GREYSCALE_FACTOR where I is index of red bin
    // will be set to 0 only if we have 2 vertical segments 
    unsigned char value_to_find = (unsigned char) RED_BIN_INDEX * mvMeanShift::GREYSCALE_FACTOR;
    _IsRed = -1;

    if(nClusters == 0) {
        printf ("Frame: No clusters =(\n");
        return NO_TARGET;
    }
    else if(nClusters == 1){
        DEBUG_PRINT("Frame: 1 Cluster...\n");
        int x1 = _KMeans[0][0].x, x2 = _KMeans[0][1].x;
        int y1 = _KMeans[0][0].y, y2 = _KMeans[0][1].y;    

        float denom = (x1==x2) ? 1 : abs(x2-x1);
        double slope = ((double)y1-y2)/denom;

        if (abs(slope) < 0.2) { // single horizontal line
            m_pixel_x = (int)(x1+x2)*0.5 - imWidth;
            m_pixel_y = (int)(y1+y2)*0.5 - imHeight;
            m_range = ((float)(FRAME_REAL_WIDTH) * _filtered_img->width) / ((x2-x1) * TAN_FOV_X);

            // displace returned y coordinate upwards by half the frame height 
            m_pixel_y -= (int) (abs(x2-x1) / FRAME_REAL_WIDTH * FRAME_REAL_HEIGHT * 0.5); 
            
            retval = ONE_SEGMENT;
            goto RETURN_CENTROID;
        }
        else if (abs(slope) > 6) { // single vertical line
            int centroid_x = (int)(x1+x2)*0.5;
            int centroid_y = (int)(y1+y2)*0.5;  

            m_pixel_x = centroid_x - imWidth;
            m_pixel_y = centroid_y - imHeight;
            
            // displace returned y coordinate upwards by half the frame height 
            m_pixel_y -= (int) (abs(x2-x1) / FRAME_REAL_WIDTH * FRAME_REAL_HEIGHT * 0.5);

            retval = ONE_SEGMENT;
            goto RETURN_CENTROID;
        }
        else { // unknown - raise error and return centroid
            retval = UNKNOWN_TARGET;
            goto RETURN_CENTROID;
        }
    }
    else if(nClusters == 2){
        DEBUG_PRINT ("Frame: 2 clusters...\n");
        int x00 = _KMeans[0][0].x,   y00 = _KMeans[0][0].y;
        int x01 = _KMeans[0][1].x,   y01 = _KMeans[0][1].y;
        int x10 = _KMeans[1][0].x,   y10 = _KMeans[1][0].y;
        int x11 = _KMeans[1][1].x,   y11 = _KMeans[1][1].y;

        //float height0 = abs(y01 - y00); // height of first 1ine
        //float height1 = abs(y11 - y10); // height of second line
        float height = -1;
        float width  = -1;

        float denom0 = (x00==x01) ? 1 : abs(x00-x01);
        double slope0 = ((double)y00-y01)/denom0;

        float denom1 = (x10==x11) ? 1 : abs(x10-x11);
        double slope1 = ((double)y10-y11)/denom1;

        if(abs(slope0) < 0.2 && abs(slope1) < 0.2){
            printf("Frame Sanity Failure: 2 Horizontal Lines?????\n");
            m_pixel_x = (int)(x00+x01+x10+x11)*0.25 - imWidth;
            m_pixel_y = (int)(y00+y01+y10+y11)*0.25 - imHeight;
            width = (int)(abs(x00-x01) + abs(x10-x11))*0.25;
            retval = UNKNOWN_TARGET;
        }
        // RZ: for some reason here Y coordinates go from 0(top) to 300(bottom), so we actually want MIN for the vertical segments
        else if(abs(slope0) < 0.2 && abs(slope1) > 6){
            //printf("1 of Each\n");
            m_pixel_x = (int)(x00 + x01)*0.5 - imWidth;
            m_pixel_y = (int)(2*MIN(y10,y11)+y00+y01)/4 - imHeight;
            width  = (int)abs(x00-x01);
            height = (int)abs(y10-y11);

            // check for red segment
            unsigned char* centroid_pixel = (unsigned char*)(_filtered_img->imageData + (y10+y11)/2*_filtered_img->widthStep + (x10+x11)/2);
            if (centroid_pixel[0] == value_to_find || centroid_pixel[-1] == value_to_find || centroid_pixel[1] == value_to_find)
                _IsRed = 1;

            retval = ONE_SEGMENT;  //Need other retval here
        }
        else if(abs(slope0) > 6 && abs(slope1) < 0.2){
            //printf("1 of Each\n");
            m_pixel_x = (int)(x10 + x11)*0.5 - imWidth;
            m_pixel_y = (int)(2*MIN(y00,y01)+y10+y11)/4 - imHeight;
            width  = (int)abs(x10-x11);
            height = (int)abs(y00-y01);
            
            // check for red segment
            unsigned char* centroid_pixel = (unsigned char*)(_filtered_img->imageData + (y00+y01)/2*_filtered_img->widthStep + (x00+x01)/2);
            if (centroid_pixel[0] == value_to_find || centroid_pixel[-1] == value_to_find || centroid_pixel[1] == value_to_find)
                _IsRed = 1;

            retval = ONE_SEGMENT;
        }
        else if(abs(slope0) > 6 && abs(slope1) > 6){
            //printf("2 Vertical Lines\n");
            m_pixel_x = (int)(x00+x01+x10+x11)*0.25 - imWidth;
            m_pixel_y = (int)(y00+y01+y10+y11)*0.25 - imHeight;
            width  = (int)(abs(x00-x11) + abs(x10-x11))*0.5;
            height = (int)(abs(y00-y11) + abs(y10-y11))*0.5;

            // check both segments for red segment
            unsigned char* centroid_pixel = (unsigned char*)(_filtered_img->imageData + (y00+y01)/2*_filtered_img->widthStep + (x00+x01)/2);
            unsigned char* centroid_pixel2 = (unsigned char*)(_filtered_img->imageData + (y10+y11)/2*_filtered_img->widthStep + (x10+x11)/2); 
            if (centroid_pixel[0] == value_to_find || centroid_pixel[-1] == value_to_find || centroid_pixel[1] == value_to_find ||
                centroid_pixel2[0] == value_to_find || centroid_pixel2[-1] == value_to_find || centroid_pixel2[1] == value_to_find)
                _IsRed = 1;
            else
                _IsRed = 0;

            retval = ONE_SEGMENT;
        }
        else{
            DEBUG_PRINT("Frame Sanity Failure: Lines Incorrectly Oriented\n");  
            retval = UNKNOWN_TARGET;   
            goto RETURN_CENTROID;   
        }

        m_range = (float)(FRAME_REAL_WIDTH * _filtered_img->width / width * TAN_FOV_X);
    }

    else if(nClusters == 3){
        DEBUG_PRINT ("Frame: 3 clusters\n");
        int x00 = _KMeans[0][0].x,   y00 = _KMeans[0][0].y;
        int x01 = _KMeans[0][1].x,   y01 = _KMeans[0][1].y;
        int x10 = _KMeans[1][0].x,   y10 = _KMeans[1][0].y;
        int x11 = _KMeans[1][1].x,   y11 = _KMeans[1][1].y;
        int x20 = _KMeans[2][0].x,   y20 = _KMeans[2][0].y;
        int x21 = _KMeans[2][1].x,   y21 = _KMeans[2][1].y;

        float height = -1;
        float width  = -1;

        float denom0 = (x00==x01) ? 1 : abs(x00-x01);
        double slope0 = ((double)y00-y01)/denom0;

        float denom1 = (x10==x11) ? 1 : abs(x10-x11);
        double slope1 = ((double)y10-y11)/denom1;

        float denom2 = (x20==x21) ? 1 : abs(x20-x21);
        double slope2 = ((double)y20-y21)/denom2;

        if(abs(slope0) < 0.2 && abs(slope1) > 6 && abs(slope2) > 6){
            m_pixel_x = (int)(x00+x01+x10+x11+x20+x21)*0.167 - imWidth;
            m_pixel_y = (int)(MIN(y10,y11)+MIN(y20,y21)+y00+y01)*0.25 - imHeight;
            width  = (int)(abs(x00-x01)+abs((x10+x11)-(x20+x21)))*0.333;
            height = (int)(-1*y00-y01+MIN(y10,y11)+MIN(y20,y21))*0.5;

            // check both vert segments for red segment
            unsigned char* centroid_pixel = (unsigned char*)(_filtered_img->imageData + (y10+y11)/2*_filtered_img->widthStep + (x10+x11)/2); 
            unsigned char* centroid_pixel2 = (unsigned char*)(_filtered_img->imageData + (y20+y21)/2*_filtered_img->widthStep + (x20+x21)/2);
            if (centroid_pixel[0] == value_to_find || centroid_pixel[-1] == value_to_find || centroid_pixel[1] == value_to_find ||
                centroid_pixel2[0] == value_to_find || centroid_pixel2[-1] == value_to_find || centroid_pixel2[1] == value_to_find)
                _IsRed = 1;
            else
                _IsRed = 0;

            retval = FULL_DETECT;
        }
        else if(abs(slope0) > 6 && abs(slope1) < 0.2 && abs(slope2) > 6){
            m_pixel_x = (int)(x00+x01+x10+x11+x20+x21)*0.167 - imWidth;
            m_pixel_y = (int)(MIN(y00,y01)+MIN(y20,y21)+y10+y11)*0.25 - imHeight;
            width  = (int)(abs(x10-x11)+abs((x00+x01)-(x20+x21)))*0.333;
            height = (int)(-1*y10-y11+MIN(y00,y01)+MIN(y20,y21))*0.5;

            // check both vert segments for red segment
            unsigned char* centroid_pixel = (unsigned char*)(_filtered_img->imageData + (y00+y01)/2*_filtered_img->widthStep + (x00+x01)/2); 
            unsigned char* centroid_pixel2 = (unsigned char*)(_filtered_img->imageData + (y20+y21)/2*_filtered_img->widthStep + (x20+x21)/2);
            if (centroid_pixel[0] == value_to_find || centroid_pixel[-1] == value_to_find || centroid_pixel[1] == value_to_find ||
                centroid_pixel2[0] == value_to_find || centroid_pixel2[-1] == value_to_find || centroid_pixel2[1] == value_to_find)
                _IsRed = 1;
            else
                _IsRed = 0;

            retval = FULL_DETECT;
        }
        else if(abs(slope0) > 6 && abs(slope1) > 6 && abs(slope2) < 0.2){
            m_pixel_x = (int)(x00+x01+x10+x11+x20+x21)*0.167 - imWidth;
            m_pixel_y = (int)(MIN(y10,y11)+MIN(y00,y01)+y20+y21)*0.25 - imHeight;
            width  = (int)(abs(x20-x21)+abs((x10+x11)-(x00+x01)))*0.333;
            height = (int)(-1*y20-y21+MIN(y10,y11)+MIN(y00,y01))*0.5;
        
            // check both vert segments for red segment
            unsigned char* centroid_pixel = (unsigned char*)(_filtered_img->imageData + (y00+y01)/2*_filtered_img->widthStep + (x00+x01)/2); 
            unsigned char* centroid_pixel2 = (unsigned char*)(_filtered_img->imageData + (y10+y11)/2*_filtered_img->widthStep + (x10+x11)/2);
            if (centroid_pixel[0] == value_to_find || centroid_pixel[-1] == value_to_find || centroid_pixel[1] == value_to_find ||
                centroid_pixel2[0] == value_to_find || centroid_pixel2[-1] == value_to_find || centroid_pixel2[1] == value_to_find)
                _IsRed = 1;
            else
                _IsRed = 0;

            retval = FULL_DETECT;
        }
        else{
            DEBUG_PRINT("Frame Sanity Failure: Incorrect line arrangement\n");
            m_pixel_x = (int)(x00+x01+x10+x11+x20+x21)*0.167 -imWidth;
            m_pixel_y = (int)(y00+y01+y10+y11+y20+y21)*0.167 - imHeight;
            retval = UNKNOWN_TARGET;
            goto RETURN_CENTROID;
        }

        m_range = (float)(FRAME_REAL_WIDTH * _filtered_img->width / width * TAN_FOV_X);
    }

    else{
        printf("nClusters = %d unhandled\n", nClusters);
        return NO_TARGET;
    }

    // this code has to be here since we check _filter_image values inside this function
    _lines.drawOntoImage (_filtered_img);
    _KMeans.drawOntoImage (_filtered_img);
    _window.showImage (_filtered_img);

    /// if we encounter any sort of sanity error, we will return only the centroid
    RETURN_CENTROID:
        m_angular_x = RAD_TO_DEG * atan(TAN_FOV_X * m_pixel_x / _filtered_img->width);
        m_angular_y = RAD_TO_DEG * atan(TAN_FOV_Y * m_pixel_y / _filtered_img->height);
        DEBUG_PRINT ("Frame: (%d,%d) (%5.2f, %5.2f)\n", m_pixel_x, m_pixel_y, 
            m_angular_x, m_angular_y); 

        switch (_IsRed) {
            case -1: DEBUG_PRINT ("Frame red signal: Unsure\n"); break;
            case 0: DEBUG_PRINT ("Frame red signal: No\n"); break;
            case 1: DEBUG_PRINT ("Frame red signal = Yes\n"); break;
            default: printf ("Unhandled value of _IsRed in vision_frame.\n"); exit(1);
        }
        return retval;
}
