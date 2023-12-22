#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#define NUM_POINTS 100000000

struct point {
    float x;
    float y;
};

int main(int argc, char *argv[]) {

    srandom(time(NULL));
    int i;
    int points_in = 0;

    float x, y;
// Parallelized point generation
#pragma omp parallel for schedule(static)
    for (i = 0; i < NUM_POINTS; i++) {
        x = (float)random() / RAND_MAX * 2 - 1;
        y = (float)random() / RAND_MAX * 2 - 1;
        if (sqrt(pow(x, 2) + pow(y, 2)) < 1)
            points_in++;
    }

    float pi = 4 * (float)points_in / NUM_POINTS;
    printf("Pi is: %f\n", pi);

    return 0;
}
