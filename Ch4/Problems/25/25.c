#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NUM_POINTS 100000000

int main(int argc, char *argv[]) {

  srand(time(NULL));
  int i;
  int points_in = 0;

  // Parallelized point generation
  #pragma omp parallel for
  for (i = 0; i < NUM_POINTS; i++){
    float x = (float)rand() / RAND_MAX * 2 - 1;
    float y = (float)rand() / RAND_MAX * 2 - 1;
    if (sqrt(pow(x, 2) + pow(y, 2)) < 1) 
      points_in++;
  }

  float pi = 4 * (float)points_in / NUM_POINTS;
  printf("Pi is: %f\n", pi);

  return 0;
}
