#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUFFER_SIZE 100
#define READ_END 0
#define WRITE_END 1
#define NUM_PIPES 5
#define PIPE_DURATION 30

// Global variables
fd_set input, input_fd;
FILE *output;
char buffer[BUFFER_SIZE];
char consoleInput[BUFFER_SIZE - 10]; // To prevent compilation warning
int fd[NUM_PIPES][2];
int timeout = 0;
int messageCount = 1;
struct timeval start_t;
struct itimerval timer;
time_t start;

// Function prototypes
void readFromPipe(int *pipeDescriptors, int pipeEnd, int pipeNumber);
void writeToPipe(int *pipeDescriptors);
void handleInterrupt(int signal);

int main() {
    // Open output file
    output = fopen("output.txt", "w");

    // Initialize timer
    time(&start);
    timer.it_value.tv_sec = PIPE_DURATION;
    timer.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
    gettimeofday(&start_t, NULL);
    signal(SIGALRM, handleInterrupt);
    srand(time(NULL));
    int seed;

    FD_ZERO(&input_fd);

    // Create child processes and associated pipes
    int i, pipeNumber;
    pid_t pid;
    for (i = 0; i < NUM_PIPES; i++) {
        seed = rand();
        if (pipe(fd[i]) == -1) {
            perror("Pipe error");
            exit(1);
        }
        FD_SET(fd[i][READ_END], &input_fd);
        pid = fork();
        if (pid == -1) {
            perror("Fork error");
            exit(1);
        }
        if (pid == 0) {
            fflush(stdout);
            srand(seed);
            break;
        }
    }

    // Main loop
    while (!timeout) {
        if (pid > 0) { // Parent process
            input = input_fd;
            pipeNumber = select(FD_SETSIZE, &input, NULL, NULL, NULL);
            if (pipeNumber < 0) {
                perror("Pipe Number error");
                exit(1);
            } else if (pipeNumber == 0) {
                perror("Nothing to read");
            } else {
                for (i = 0; i < NUM_PIPES; i++) {
                    if (FD_ISSET(fd[i][READ_END], &input)) {
                        readFromPipe(fd[i], fd[i][READ_END], i);
                    }
                }
            }
        } else { // Child process
            input = input_fd;
            if (i == 4) {
                printf("Child 5 => ");
                fgets(consoleInput, BUFFER_SIZE, stdin);
                snprintf(buffer, BUFFER_SIZE, "Child 5: %s", consoleInput);
                writeToPipe(fd[i]);
            } else {
                snprintf(buffer, BUFFER_SIZE, "Child %d Message %d", i + 1, messageCount++);
                writeToPipe(fd[i]);
                sleep(rand() % 3); // Each process sleeps for a random time (0, 1, or 2 seconds)
            }
        }
    }

    // Clean up and exit
    fclose(output);
    exit(0);
}

// Read from pipe and write to output file
void readFromPipe(int *pipeDescriptors, int pipeEnd, int pipeNumber) {
    if (!timeout) {
        close(pipeDescriptors[WRITE_END]);
        struct timeval currentTime;
        gettimeofday(&currentTime, NULL);
        float currentReadTime = (float)((currentTime.tv_sec - start_t.tv_sec) + ((currentTime.tv_usec - start_t.tv_usec) / 1000000.));

        read(pipeEnd, buffer, BUFFER_SIZE);
        if (pipeNumber == 4) {
            fprintf(output, "0:%06.3f: %s", currentReadTime, buffer);
        } else {
            fprintf(output, "0:%06.3f: %s\n", currentReadTime, buffer);
        }
    }
}

// Write to pipe
void writeToPipe(int *pipeDescriptors) {
    if (!timeout) {
        close(pipeDescriptors[READ_END]);
        write(pipeDescriptors[WRITE_END], buffer, BUFFER_SIZE);
    }
}

// Signal handler for interrupt
void handleInterrupt(int signal) {
    assert(signal == SIGALRM);
    timeout = 1;
    exit(0);
}
