#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>


int *primes;
// Checks for prime numbers and prints them
void *print_thread(void *params) {
  int *max = (int *)params;
  int root;
  char not_prime;
  int num_primes = 1;
  // 2 is a guaranteed prime I guess? Technically this fails if you put in 1 but
  // why would you be doing that
  printf("2 ");
  for (int i = 3; i < *max; i++) {
    not_prime = 0;

    // Only need to check for divisibility by primes up to the root of a number
    root = (int)sqrt((double)i);

    // Does the actual checking
    for (int j = 0; j < num_primes; j++){
        if (primes[j] > root)
          break;
        // If any prime divides the number then it's not prime and we continue
        if (i % primes[j] == 0) {
          not_prime = 1;
          break;
      }
    }

    if (not_prime)
      continue;
    /* 
     * If we find a prime, track it for use in the next loop 
     * Could probably include logic to ignore it if we're not likely to hit it
     * (because it's larger than the root of the max or something), but I don't
     * care that much 
     */
    num_primes++;
    primes = realloc(primes, num_primes * sizeof(int));
    primes[num_primes - 1] = i;
    printf("%d ", i);

  }
  printf("\b\n");
  pthread_exit(0);
}

int main(int argc, char *argv[]) {
  // Keep track of any primes we find, starting with 2
  primes = malloc(sizeof(int));
  primes[0] = 2;
  if (argc < 2) {
    printf("Please enter a maximum number\n");
    return 1;
  }

  int max = atoi(argv[1]);
  
  pthread_t tid;
  pthread_attr_t attr;
  pthread_attr_init(&attr);

  pthread_create(&tid, &attr, print_thread, (void *)&max);

  pthread_join(tid, NULL);
  return 0;
}
