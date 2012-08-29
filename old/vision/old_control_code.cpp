







char controller_GATE (IplImage* img, char &state, HSV_settings HSV, char* window[]) {
    if (state == 0) // initiation character for controllers
        state = 'F';
    char vcode;
    int gateX, gateY; float range;
    
    vcode = vision_GATE (img, gateX,gateY,range, HSV, window);
        
    /** state machine
     * F: "Gate too far"
     * r: "Gate to the Right" (go fowards)
     * R: "Gate to the Right" (go right)
     * l: "Gate to the left" (go fowards)
     * L: "Gate to the Left" (go left)
     * C: "Gate is centered"
     * S: "Reached front of gate. Stop vision"
     * X: "Error state" 
    */
    
    if (vcode == 2) {
        if (range < 3.0) state = 'S';
        else state = 'C';
    }
    else if (vcode == 3) state = 'S';
    else if (vcode == -1) state = 'X';
    else
    switch (state) {
        case 'F': // gate too far
            if (vcode == 1) { 
                if (gateX > 0) state = 'r';
                else state = 'l';
            }
            break;
            // if vcode == 0 then case == 'F'
        case 'C': // gate centered
            if (vcode == 1) { 
                if (gateX > 0) state = 'r';
                else state = 'l';
            };
            break;
            // if vcode == 2 then case == 'C'
        case 'r':
            if (vcode == 1) { 
                if (gateX > 0) state = 'R';
                else state = 'l';
            } 
            else if (vcode == 0) state = 'R';
            break;
        case 'l':
            if (vcode == 1) { 
                if (gateX > 0) state = 'r';
                else state = 'L';
            }
            else if (vcode == 0) state = 'L';
            break;
        case 'R':
            if (vcode == 1) { 
                if (gateX > 0) state = 'r';
                else state = 'l';
            }
            else if (vcode == 0) state = 'r';
            break;
         case 'L':
            if (vcode == 1) { 
                if (gateX > 0) state = 'r';
                else state = 'l';
            }
            else if (vcode == 0) state = 'l';
            break;
    }
    
    switch (state) {
        case 'F': case 'r': case 'l': return 'w';
        case 'L': return 'a';
        case 'R': return 'd';
        case 'C': 
            if (gateX > 30) return 'd';
            else if (gateX < -30) return 'a';
            else return 'w';
        case 'S':
            printf ("Gate Task Complete!\n");
            return '/';
        case 'X':
            printf ("Gate Error\n");
            return '/';
        default:
            return '\0';
    }
}

char controller_PATH (IplImage* img, char &state, HSV_settings HSV, char* window[]) {
    char vcode;
    int pathX, pathY; float tan_angle, length;
    
    vcode = vision_PATH (img, pathX,pathY,tan_angle,length, HSV, window);
        
    /** state machine
     * F: "Path too far (cant see it)"
     * P: "Partial Detection"  -  go towards the path
     * c: "Fully centered high depth"  -  sink
     * 1: sink 1
     * 2: ..
     * 3: ..
     * 4: ..
     * 5: ..
     * D: "Correct Depth"  -  Recenter
     * C: "Centered at correct depth"  -  Align
     * A: "Aligned"  - stop
     * S: "Reached front of gate. Stop vision"
     * X: "Error state" 
    */
    
    enum ESTATE {NOPATH,PARTIAL,CENTERED, SINK, DPARTIAL,DCENTERED,STOP,ERR};
    ESTATE lookup[8][3] = {
            {NOPATH,  PARTIAL,   CENTERED},  // returns 'w'
            {NOPATH,  PARTIAL,   PARTIAL},  // tries to center, math
            {ERR,     PARTIAL,   SINK},        // sinks
/*SINK*/    {ERR,     DPARTIAL,  DCENTERED},  // sinks, needs math
/*DPARTIAL*/{ERR,     DPARTIAL,  DCENTERED},  // tries to center
/*DCENTER*/ {ERR,     DPARTIAL,  DCENTERED},  // need math
            {STOP,    STOP,      STOP},
            {ERR,     ERR,       ERR}};
    
    ESTATE estate = (ESTATE) state;  
    // the only state that needs additional processing is DCENTERED
    if (estate == PARTIAL && sqrt(pathX*pathX + pathY*pathY) < 30)
        estate = CENTERED;    
    else if (estate == DCENTERED) {
        if (fabs(tan_angle) < 0.05) // 3ish degrees
            estate = STOP;
    }
    else if ((estate == SINK) && (length < 100)) {
        // continue to sink
    }
    else estate = lookup[estate][(int)vcode];
    
    state = estate;
    printf ("%d\n",state);
    
    /** figure out output */
    switch (estate) {
        case NOPATH: return 'w';
        case PARTIAL: case DPARTIAL:
            if (fabs(pathY/(pathX+0.01)) < 11.5) { // if centroid outside of +-5 degrees from vertical
                if (pathX > 0) return 'd'; // turn towards the centroid
                else return 'a';
            }
            else return 'w'; // move towards centroid
        case CENTERED: case SINK:
            return 'k';
        case DCENTERED:
            if (tan_angle > 0) // first quadrant
                return 'd';
            else return 'a'; // second quadrant
        case STOP:
            printf ("PATH Task Complete!\n");
            return '/';
        case ERR:
            printf ("PATH Error\n");
            return '/';
    }
}



