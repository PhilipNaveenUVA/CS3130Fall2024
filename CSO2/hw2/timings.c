// File: gettimings.c
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>

#define NANOSECONDS_IN_SEC 1000000000LL

long long get_nsecs() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * NANOSECONDS_IN_SEC + t.tv_nsec;
}

// Scenario 1: Empty function call
__attribute__((noinline)) void empty_function() {
    __asm__("");
}

// Scenario 4: Signal handler for timing
volatile sig_atomic_t signal_received = 0;

void signal_handler(int signo) {
    signal_received = 1;
}

// Scenario 5: Signal handler for process-to-process communication
volatile sig_atomic_t signal_from_other_process = 0;

void signal_exchange_handler(int signo) {
    signal_from_other_process = 1;
}

void measure_scenario(int scenario) {
    long long start, end;
    switch (scenario) {
        case 1:  // Empty function
            start = get_nsecs();
            empty_function();
            end = get_nsecs();
            printf("Time for empty function: %lld ns\n", end - start);
            break;

        case 2:  // getppid
            start = get_nsecs();
            getppid();
            end = get_nsecs();
            printf("Time for getppid: %lld ns\n", end - start);
            break;

        case 3:  // system("/bin/true")
            start = get_nsecs();
            if (system("/bin/true") != 0) {
                fprintf(stderr, "system() failed\n");
            }
            end = get_nsecs();
            printf("Time for system('/bin/true'): %lld ns\n", end - start);
            break;

        case 4:  // Signal to the current process
            signal(SIGUSR1, signal_handler);
            start = get_nsecs();
            raise(SIGUSR1);
            while (!signal_received);  // Busy wait until signal handler executes
            end = get_nsecs();
            printf("Time to handle signal: %lld ns\n", end - start);
            break;

        case 5: {  // Signal exchange between two processes
            pid_t other_pid;
            printf("Enter PID of other process: ");
            if (scanf("%d", &other_pid) != 1) {
                fprintf(stderr, "Failed to read PID\n");
                exit(EXIT_FAILURE);
            }

            signal(SIGUSR1, signal_exchange_handler);
            start = get_nsecs();
            kill(other_pid, SIGUSR1);  // Send signal to other process

            while (!signal_from_other_process);  // Wait for signal back
            end = get_nsecs();
            printf("Time for signal exchange: %lld ns\n", end - start);
            break;
        }

        default:
            fprintf(stderr, "Invalid scenario.\n");
            exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <scenario_number>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int scenario = atoi(argv[1]);

    if (scenario == -1) {  // Scenario for the second process (waiting for signal)
        pid_t other_pid;
        printf("My PID: %d\n", getpid());
        printf("Enter PID of process to send signal to: ");
        if (scanf("%d", &other_pid) != 1) {
            fprintf(stderr, "Failed to read PID\n");
            return EXIT_FAILURE;
        }

        signal(SIGUSR1, signal_exchange_handler);

        while (!signal_from_other_process);  // Wait for signal from the other process
        kill(other_pid, SIGUSR1);  // Signal back
        printf("Signal sent back to PID %d\n", other_pid);
        return 0;
    }

    // Measure the specified scenario
    measure_scenario(scenario);

    return 0;
}

