#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 80

int main(void) {

    char *args[MAX_LINE / 2 + 1];
    char *prevArgs[MAX_LINE / 2 + 1];

    char prev_length, should_run, should_wait, dup_in, dup_out, use_pipe;
    prev_length = 0;
    should_run = 1;

    char pipe_index;
    int pipes[2];

    while (should_run) {

        char *buf = malloc(sizeof(char));
        size_t n = 1;
        int i;
        dup_in = dup_out = use_pipe = 0;
        should_wait = 1;

        printf("osh> ");
        fflush(stdout);

        // Get the command
        // May need to change this because it doesn't get the EOF properly
        if (getline(&buf, &n, stdin) == 0)
            break;

        // Remove the newline from the end of the command
        if (n >= 2)
            buf[n - 2] = '\0';

        // Detect EOF (Ctrl+D). Placing it here will fix extra printing of the
        // shell prompt, which is nice.
        if (feof(stdin))
            should_run = 0;

        // Start tokenizing the input
        args[0] = strtok(buf, " ");

        // If someone just hits Enter this will skip everything and just print
        // the shell prompt again
        if (args[0] == NULL)
            continue;

        // Continue tokenizing the input. This is where the checks for piping,
        // waiting, etc will go
        for (i = 1; i < MAX_LINE / 2 + 1; i++) {
            args[i] = strtok(NULL, " ");
            if (args[i] == NULL)
                break;
            // Check for special characters
            switch (args[i][0]) {
            case '&':
                should_wait = 0;
                args[i] = NULL;
                break;
            case '>':
                dup_out = 1;
                pipe_index = i + 1;
                args[i] = NULL;
                break;
            case '<':
                dup_in = 1;
                pipe_index = i + 1;
                args[i] = NULL;
                break;
            case '|':
                use_pipe = 1;
                pipe_index = i + 1;
                args[i] = NULL;
                break;
            default:
                break;
            }
        }

        /* Check if we should use the previously run command, and if we aren't,
         * then we will update prevArgs to contain those arguments. Basically
         * stores values, then copies them back (this works better than trying
         * to use the prevArgs array for some reason). This also fixed the
         * issue of the child process not closing properly
         */
        if (!strcmp("!!", args[0])) {
            if (prev_length == 0) {
                printf("No commands in history\n");
                continue;
            }

            for (int j = 0; j < prev_length; j++) {
                args[j] = prevArgs[j];
            }
        } else {
            for (int j = 0; j < i; j++) {
                prevArgs[j] = args[j];
            }
            prev_length = (char)i;
        }

        // for (int j = 0; j < MAX_LINE / 2 + 1; j++){
        //   printf("%d: %s\n", j, args[j]);
        //   if(args[j] == NULL)
        //     break;
        // }

        // Check to exit
        if (!strcmp("exit", args[0])) {
            should_run = 0;
            break;
        }

        // Do the actual execution of the command
        int child = fork();
        if (child) {
            if (should_wait)
                wait(NULL);

        } else {
            // This is... done?
            if (dup_in) {
                int fd = open(args[pipe_index], O_RDONLY);
                char buf[128];
                while (1) {
                    if (read(fd, buf, 128) == 0)
                        break;
                    printf("%s", buf);
                }
                dup2(fd, STDIN_FILENO); // stdout/in are always the second fd
            }
            // This is done
            if (dup_out) {
                int fd = open(args[pipe_index], O_RDWR | O_CREAT,
                              S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                dup2(fd, STDOUT_FILENO);
            }

            if (use_pipe) {
                pipe(pipes);
                if (!fork()) {
                    dup2(pipes[0], STDIN_FILENO);
                    close(pipes[1]);
                    char **child_prog = &args[pipe_index];
                    execvp(child_prog[0], child_prog);
                } else {
                    dup2(pipes[1], STDOUT_FILENO);
                    close(pipes[0]);
                }
            }
            // Execvp doesn't appear to enjoy being in a nested conditional,
            // *or* I simply cannot have it twice in one child program without
            // forking
            execvp(args[0], args);
        }
    }

    return 0;
}
