#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#define NUM_POINTS 100000000
int points_in;

void *generate_points(void *params) {
    float x, y;
    points_in = 0;
    for (int i = 0; i < NUM_POINTS; i++) {
        // Should generate between -1 and 1
        x = (float)random() / RAND_MAX * 2 - 1;
        y = (float)random() / RAND_MAX * 2 - 1;
        // Give a little leeway for points that are just about on 1.0
        if (sqrt(x * x + y * y) < 1.0001)
            points_in++;
    }
    pthread_exit(0);
}
int main(int argc, char *argv[]) {
    pthread_t tid;
    pthread_attr_t attr;
    srandom(time(NULL));
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, generate_points, NULL);

    pthread_join(tid, NULL);

    // Do the calculation
    float pi = 4 * (float)points_in / NUM_POINTS;
    printf("Pi is: %f\n", pi);
    return 0;
}
