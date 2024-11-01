// Andrew Jacobson 10/29/24

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    int fd1[2], fd2[2]; // file descriptors for the two pipes
    pid_t pid;

    // create first pipe fd1
    if (pipe(fd1) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // fork first child
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) { // first child process to run 'sort'
        printf("The child process running sort is %d\n", getpid());

        // tie write end of pipe fd1 to standard output (file descriptor 1)
        dup2(fd1[1], STDOUT_FILENO);
        // close unnecessary pipe ends
        close(fd1[0]);
        close(fd1[1]);
        
        // execute 'sort'
        execlp("sort", "sort", NULL);

        // if execlp fails
        printf("Should not be here after execlp to sort\n");
        perror("execlp sort");
        exit(EXIT_FAILURE);
    }

    // create second pipe fd2
    if (pipe(fd2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // fork second child
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) { // second child process to run 'uniq'
        printf("The child process running uniq is %d\n", getpid());

        // tie read end of fd1 to standard input (file descriptor 0)
        dup2(fd1[0], STDIN_FILENO);
        // tie write end of fd2 to standard output (file descriptor 1)
        dup2(fd2[1], STDOUT_FILENO);
        
        // close unnecessary pipe ends
        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);

        // execute 'uniq'
        execlp("uniq", "uniq", NULL);

        // if execlp fails
        printf("Should not be here after execlp to uniq\n");
        perror("execlp uniq");
        exit(EXIT_FAILURE);
    }

    // fork third child
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) { // third child process to run 'wc -l'
        printf("The child process running wc -l is %d\n", getpid());

        // tie read end of fd2 to standard input (file descriptor 0)
        dup2(fd2[0], STDIN_FILENO);

        // close all unnecessary pipe ends
        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);

        // execute 'wc -l'
        execlp("wc", "wc", "-l", NULL);

        // if execlp fails
        printf("Should not be here after execlp to wc -l\n");
        perror("execlp wc -l");
        exit(EXIT_FAILURE);
    }

    // parent process code
    // close both ends of pipes fd1 and fd2
    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);

    // wait for all child processes to finish
    wait(NULL);
    wait(NULL);
    wait(NULL);

    printf("All child processes have finished.\n");

    return 0;
}
