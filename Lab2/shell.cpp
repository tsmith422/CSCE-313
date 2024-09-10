/****************
LE2: Introduction to Unnamed Pipes
****************/
#include <unistd.h> // pipe, fork, dup2, execvp, close
using namespace std;
/*
    pipe:   construct that allows two processes to communicate
    fork:   creates a new process by duplicating the calling process
    dup2:   duplicates a file descriptor (used to redirect input/output to read/write)
    execvp: can execute commands with the arguments (file, array of args for the command)
    close:  closes a file descriptor (read or write)
*/

int main()
{
    // command 1 (to be executed): lists all the files in the root directory in the long format
    char *cmd1[] = {(char *)"ls", (char *)"-al", (char *)"/", nullptr};
    // command 2 (to be executed): translates all input from lowercase to uppercase
    char *cmd2[] = {(char *)"tr", (char *)"a-z", (char *)"A-Z", nullptr};

    // TODO: add functionality
    // Create pipe
    int fd[2];
    // creates pipe with two file descriptors: fd[0] reading, fd[1] writing
    pipe(fd);

    // Create child to run first command
    // In child, redirect output to write end of pipe
    // Close the read end of the pipe on the child side.
    // In child, execute the command
    // creates child and stores it in child1
    pid_t child1 = fork();
    // if child1 = 0, the fork was a success
    if (child1 == 0)
    {
        // redirects standard output (STDOUT_FILENO) to fd[1] (write end)
        dup2(fd[1], STDOUT_FILENO);
        // close fd[0] (read end)
        close(fd[0]);
        // executes the first command
        execvp(cmd1[0], cmd1);
    }

    // Create another child to run second command
    // In child, redirect input to the read end of the pipe
    // Close the write end of the pipe on the child side.
    // Execute the second command.
    // creates child and stores it in child2
    pid_t child2 = fork();
    // if child2 = 0, the fork was a success
    if (child2 == 0)
    {
        // redirects standard input (STDIN_FILENO) to read end (fd[0])
        dup2(fd[0], STDIN_FILENO);
        // close write end (fd[1])
        close(fd[1]);
        // executes the second command
        execvp(cmd2[0], cmd2);
    }

    // Reset the input and output file descriptors of the parent.
    // after redirecting, we close the original parent file descriptors to ensure were only reading/writing to standard input/output
    close(fd[0]);
    close(fd[1]);
}
