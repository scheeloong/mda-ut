#ifndef __PROFILE_BIN__
#define __PROFILE_BIN__
#include <time.h>
#include <string.h>

class PROFILE_BIN {
    
    char binName[30];
    unsigned startTime, nCalls;
    float totalTime;
    
    public:
    
    PROFILE_BIN (const char* Name) {
      
	assert(strlen(Name) < 30);
      
        startTime = 0;
        totalTime = 0;
        nCalls = 0;
	strcpy(binName, Name);
        
    }
    ~PROFILE_BIN () {
        printf ("Bin: %s\n", binName);
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
