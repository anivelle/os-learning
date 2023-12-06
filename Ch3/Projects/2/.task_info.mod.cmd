savedcmd_/home/alex/OS/Ch3/Projects/2/task_info.mod := printf '%s\n'   task_info.o | awk '!x[$$0]++ { print("/home/alex/OS/Ch3/Projects/2/"$$0) }' > /home/alex/OS/Ch3/Projects/2/task_info.mod
