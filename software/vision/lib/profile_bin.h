#ifndef __PROFILE_BIN__
#define __PROFILE_BIN__
#include <time.h>
class PROFILE_BIN {
    
    unsigned startTime, nCalls;
    float totalTime;
    
    public:
    
    PROFILE_BIN () {
        
        startTime = 0;
        totalTime = 0;
        nCalls = 0;
        
    }
    ~PROFILE_BIN () {
        
        printf ("Times called: %d\n", nCalls);
        printf ("Average time: %6.2f secs\n", totalTime/nCalls);
        printf ("Total time: %6.2f secs\n", totalTime);
        
    }
    
    void start() {
        startTime = clock();
        nCalls++;
    }
    
    void stop() {
        
        totalTime += float(clock() - startTime)/CLOCKS_PER_SEC;
    }
    
};

#endif
