#include <stdio.h>
#include <cv.h>

#include "mgui.h"
#include "mv.h"

int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("Usage: %s <image file>\n", argv[0]);
        return 1;
    }

    const char *img_file = argv[1];

    return 0;
}
