#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int my_system(const char *command) {
    // Return 1 if command is NULL, as required by the `system()` function specification
    if (command == NULL) {
        return 1;
    }

    // Fork the current process to create a child process
    pid_t pid = fork();
    
    if (pid == -1) {
        // So if fork() fails, print an error message and return -1
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        // This block is executed in the child process
        
        // Replace the current process image with the shell process to run the command
        execl("/bin/sh", "sh", "-c", command, (char *)NULL);
        
        // If execl() returns, it means an error occurred while executing the command
        perror("execl");
        // Exit with status 127 if the exec fails (conventionally used in shells)
        exit(127);
    } else {
        // Thhis block is executed in the parent process
        
        int status;  // To store the child's termination status
        
        // Wait for the child process to finish execution
        if (waitpid(pid, &status, 0) == -1) {
            // If waitpid() fails, print an error message and return -1
            perror("waitpid");
            return -1;
        }

        // Check if the child process exited normally
        if (WIFEXITED(status)) {
            // Return the exit status of the child process
            return WEXITSTATUS(status);
        } 
        // Check if the child process was terminated by a signal
        else if (WIFSIGNALED(status)) {
            // Return 128 + the signal number (used to distinguish signal termination)
            return 128 + WTERMSIG(status);
        }
    }

    // Return -1 as a fallback (this point should never be reached)
    return -1;
}

