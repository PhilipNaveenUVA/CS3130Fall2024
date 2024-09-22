#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#define BOX_SIZE 4096

pid_t other_pid = 0;
char *my_inbox;
char *other_inbox;
char my_inbox_shm_open_name[NAME_MAX];
char other_inbox_shm_open_name[NAME_MAX];

static void handle_signal(int signum);

// Set up shared memory for the inbox of the specified PID
char *setup_inbox_for(pid_t pid, char *filename) {
    snprintf(filename, NAME_MAX, "/%d-chat", pid);
    int fd = shm_open(filename, O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror("shm_open");
        abort();
    }
    if (ftruncate(fd, BOX_SIZE) != 0) {
        perror("ftruncate");
        abort();
    }
    char *ptr = mmap(NULL, BOX_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == (char *)MAP_FAILED) {
        perror("mmap");
        abort();
    }
    return ptr;
}

// Set up inboxes for the current process and the other process
void setup_inboxes() {
    my_inbox = setup_inbox_for(getpid(), my_inbox_shm_open_name);
    other_inbox = setup_inbox_for(other_pid, other_inbox_shm_open_name);
}

// Clean up shared memory resources
void cleanup_inboxes() {
    munmap(my_inbox, BOX_SIZE);
    munmap(other_inbox, BOX_SIZE);
    shm_unlink(my_inbox_shm_open_name);
}

// Signal handler for SIGINT, SIGTERM, and SIGUSR1
static void handle_signal(int signum) {
    if (signum == SIGINT) {
        cleanup_inboxes();
        kill(other_pid, SIGTERM); // Notify other process to terminate
        exit(EXIT_SUCCESS);
    } else if (signum == SIGTERM) {
        cleanup_inboxes();
        exit(EXIT_SUCCESS);
    } else if (signum == SIGUSR1) {
        printf("Message received: %s\n", my_inbox);
        fflush(stdout);
        my_inbox[0] = '\0'; // Acknowledge message receipt by clearing inbox
    }
}

int main(void) {
    printf("This process's ID: %ld\n", (long)getpid());
    char *line = NULL;
    size_t line_length = 0;

    // Setup signal handling
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);

    // Get the PID of the other process
    do {
        printf("Enter other process ID: ");
        if (getline(&line, &line_length, stdin) == -1) {
            perror("getline");
            abort();
        }
    } while ((other_pid = strtol(line, NULL, 10)) == 0);
    free(line);

    setup_inboxes();

    // Main loop: read and send messages until EOF
    while (1) {
        printf("Enter message: ");
        if (fgets(other_inbox, BOX_SIZE, stdin) == NULL) {
            // End of input, terminate the other process
            kill(other_pid, SIGTERM);
            break;
        }

        // Send signal to the other process that a message is available
        kill(other_pid, SIGUSR1);

        // Wait for the other process to read and clear the inbox
        while (other_inbox[0] != '\0') {
            struct timespec ts = { .tv_sec = 0, .tv_nsec = 10000000 }; // 10ms sleep
            nanosleep(&ts, NULL);
        }
    }

    cleanup_inboxes();
    return EXIT_SUCCESS;
}

