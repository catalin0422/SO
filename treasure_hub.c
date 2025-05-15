#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t monitor_pid = -1;
int monitor_running = 0;

// handler pentru terminarea monitorului
void sigchld_handler(int sig) {
    int status;
    waitpid(monitor_pid, &status, 0);
    printf("Monitor ended. Status: %d\n", WEXITSTATUS(status));
    monitor_running = 0;
}

// verificam dacă monitorul rulează
int is_monitor_alive() {
    return (monitor_pid > 0 && monitor_running);
}

void start_monitor() {
    if (is_monitor_alive()) {
        printf("Monitor is already running!\n");
        return;
    }

    // Scrie PID-ul treasure_manager parinte într-un fișier
    FILE *pid_file = fopen(".manager_pid", "w");
    if (pid_file) {
        fprintf(pid_file, "%d\n", getppid());
        fclose(pid_file);
    }

    monitor_pid = fork();
    if (monitor_pid == 0) {
        execl("./monitor", "monitor", NULL);
        perror("execl");
        exit(1);
    } else {
        printf("Monitor started with PID %d\n", monitor_pid);
        monitor_running = 1;
    }
}

void send_command_to_monitor(const char* command) {
    if (!is_monitor_alive()) {
        printf("Monitor is not running.\n");
        return;
    }

    FILE *f = fopen(".command.txt", "w");
    if (!f) {
        perror("fopen");
        return;
    }
    fprintf(f, "%s\n", command);
    fclose(f);

    kill(monitor_pid, SIGUSR1);  // trimite semnal spre monitor
}

void stop_monitor() {
    if (!is_monitor_alive()) {
        printf("Monitor not running.\n");
        return;
    }

    kill(monitor_pid, SIGTERM);  // semnal de oprire
    usleep(3000000); // simulare delay
    // confirmare vine prin SIGCHLD
}

int main() {
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGCHLD, &sa, NULL);

    char command[100];
    while (1) {
        printf("> ");
        fflush(stdout);

        if (!fgets(command, sizeof(command), stdin)) break;
        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "start_monitor") == 0) {
            start_monitor();
        } else if (strcmp(command, "list_hunts") == 0 ||
                   strcmp(command, "list_treasures") == 0 ||
                   strcmp(command, "view_treasure") == 0) {
            send_command_to_monitor(command);
        } else if (strcmp(command, "stop_monitor") == 0) {
            stop_monitor();
        } else if (strcmp(command, "exit") == 0) {
            if (is_monitor_alive()) {
                printf("Error: monitor still running!\n");
            } else {
                break;
            }
        } else {
            printf("Unknown command.\n");
        }
    }

    return 0;
}
