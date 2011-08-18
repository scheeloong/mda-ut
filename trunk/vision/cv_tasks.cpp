#include <cstdio>

#include "cv_tasks.h"

// this is for the gate task
void vision_GATE (IplImage* img, 
                  int &gateX, int &gateY,
                  char flags, char* window[])
{   
// HS filter to extract gate object
    IplImage* img_1;
    HueSat_Filter1 (img, img_1, 
                    GATE_HMIN, GATE_HMAX, GATE_SMIN, GATE_SMAX,
                    5);
// copy image a few times to use for display   
    IplImage* img_2 = cvCreateImage ( 
        cvGetSize(img_1),
        img_1->depth,
        img_1->nChannels);
    cvCopy (img_1, img_2);
    img_2->origin = 1;
    img_1->origin = 1;
    
   // int cnt = cvCountNonZero (img_Sat);
    CvMemStorage* storage = cvCreateMemStorage(0); // create memstorage for line finidng
    CvSeq* lines = 0;
    
// probabilistic Hough line finder
    int minlen=30, mindist=30;
    lines = cvHoughLines2(img_2, storage,
        CV_HOUGH_PROBABILISTIC,
        1, CV_PI/360.0,
        50, minlen, mindist);

// check if there is lines
    int nlines=lines->total;
    printf ("%d\n", nlines);
    if (nlines == 0) {
        printf ("WHY NO LINE D=\n");
        cvShowImage(window[2], img_2);
        return;
    }
    
// draw the line finder output and display it. Also rearrange the two endpoints of each line
// so the lower y value comes first
    CvPoint* temp; int swap;
    for (int i = 0; i < nlines; i++) {
        temp = (CvPoint*)cvGetSeqElem(lines, i);  
 
        if (temp[0].y > temp[1].y) { // arrange by Y value
            swap=temp[1].y; temp[1].y=temp[0].y; temp[0].y=swap;
            swap=temp[1].x; temp[1].x=temp[0].x; temp[0].x=swap;
        }
    //printf ("%d %d      %d %d\n", temp[0].x,temp[0].y, temp[1].x,temp[1].y);
        cvLine (img_2, temp[0],temp[1], CV_RGB(100,200,100), 1);
    }
    cvShowImage(window[1], img_2);
    
    int nseeds=0;
    CvPoint** cseed=0;

    addClusterSeed (cseed, nseeds, lines, nlines);
    addClusterSeed (cseed, nseeds, lines, nlines);    
    addClusterSeed (cseed, nseeds, lines, nlines);   
    KMcluster (cseed, nseeds, lines, nlines, 2);
    
    for (int i = 0; i < 3; i++) {
        //printf ("%d %d      %d %d\n", cseed[i][0].x,cseed[i][0].y, cseed[i][1].x,cseed[i][1].y);
        cvLine (img_2, cseed[i][0],cseed[i][1], CV_RGB(100,200,100), 2);
    }
    
    cvShowImage(window[2], img_2);
    cvReleaseMemStorage(&storage);

    
// LINES_BINNING
/*
    CvMat* horiz;    
    int rbounds[2] = {0, img_Pipe->width*1.5};
    float abounds[2] = {-CV_PI/3, CV_PI/3};
    lines_binning (lines, N_LINES, horiz, 2, // input line matrix, nlines, output line mat 
                       rbounds, abounds, // min/max bound for rad and ang
                       6, 6);         // bin resolution in rad and ang

    CvMat* vert;    
    float vert_abounds[2] = {CV_PI/4, CV_PI*3/4};
    lines_binning (lines, N_LINES, vert, 1, // input line matrix, nlines, output line mat 
                       rbounds, vert_abounds, // min/max bound for rad and ang
                       4, 5);         // bin resolution in rad and ang
    
    if (flags & 1) {
        IplImage* img_Lines = cvCreateImage ( // temp img for display
        cvGetSize(img_Pipe), img_Pipe->depth, 1);
        
        cvCopy (img_Pipe, img_Lines);       // copy img     
        drawHoughLines (img_Lines, horiz);  // draw lines on img
        drawHoughLines (img_Lines, vert);  // draw lines on img
        img_Lines->origin = 1;
        cvShowImage (window[1], img_Lines);
        cvReleaseImage(&img_Lines);
    }
    */
}
/*
void vision_PATH (IplImage* img,
                  int &x, int &y, float &ang
                  char flags=0, char* window[]=NULL)
{*/
