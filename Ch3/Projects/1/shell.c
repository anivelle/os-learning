#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 80

int main(void) {

  char *args[MAX_LINE / 2 + 1];
  char *prevArgs[MAX_LINE / 2 + 1];
  int should_run = 1;

  while (should_run) {
    printf("osh> ");
    fflush(stdout);
    char *buf = malloc(sizeof(char));
    size_t n = 1;
    int i;
    getline(&buf, &n, stdin);

    buf[n - 2] = '\0';
    args[0] = strtok(buf, " ");
    for (i = 1; i < MAX_LINE / 2 + 1; i++){
      args[i] = strtok(NULL, " ");
    }
    
    // for (int j = 0; j < MAX_LINE / 2 + 1; j++){
    //   printf("%d: %s\n", j, args[j]);
    //   if(args[j] == NULL)
    //     break;
    // }

    int child = fork();
    if (child) {
      if (args[i - 1][0] != '&')
        wait(NULL);
    } else {
      execvp(args[0], args);
    }
  }

  return 0;
}
