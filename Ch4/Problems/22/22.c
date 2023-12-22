#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include <string.h>

struct args {
    int count;
    char **elements;
};

int mean = 0;
int max = INT_MIN;
int min = INT_MAX;
void *calc_mean(void *param) {
    struct args *pass = (struct args *)param;
    for (int i = 0; i < pass->count; i++) {
        mean += atoi(pass->elements[i]);
    }
    mean /= pass->count;
    pthread_exit(0);
}

void *calc_min(void *param) {
    struct args *pass = (struct args *)param;
    for (int i = 0; i < pass->count; i++) {
        int cur = atoi(pass->elements[i]);
        if (cur < min)
            min = cur;
    }

    pthread_exit(0);
}

void *calc_max(void *param) {
    struct args *pass = (struct args *)param;
    for (int i = 0; i < pass->count; i++) {
        int cur = atoi(pass->elements[i]);
        if (cur > max)
            max = cur;
    }
    pthread_exit(0);
}

int main(int argc, char *argv[]) {

    pthread_t thread_id[3];
    pthread_attr_t thread_attr[3];

    if (argc < 2) {
        printf("Please input some numbers\n");
        return 1;
    }
    void *(*functions[])(void *param) = {calc_mean, calc_min, calc_max};
    struct args pass = {.count = argc - 1, .elements = &argv[1]};
    for (int i = 0; i < 3; i++) {
        pthread_attr_init(&thread_attr[i]);
        pthread_create(&thread_id[i], &thread_attr[i], functions[i], &pass);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(thread_id[i], NULL);
    }

    printf("The mean is %d\n"
           "The min is %d\n"
           "The max is %d\n",
           mean, min, max);
    return 0;
}
