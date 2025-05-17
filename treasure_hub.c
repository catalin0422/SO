#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <dirent.h>

#define MAX_CMD 128
#define MAX_ARGS 10

pid_t monitor_pid = 0;
int pipefd[2];

void handle_sigchld(int sig) {
    waitpid(monitor_pid, NULL, 0);
    monitor_pid = 0;
    printf("\nMonitor terminated.\n");
}

void start_monitor() {
    if (monitor_pid != 0) {
        printf("Monitor already running.\n");
        return;
    }

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child: monitor
        close(pipefd[1]); // close write end
        dup2(pipefd[0], STDIN_FILENO);
        execl("./monitor", "./monitor", NULL);
        perror("execl");
        exit(1);
    } else if (pid > 0) {
        // Parent
        close(pipefd[0]); // close read end
        monitor_pid = pid;
        printf("Monitor started with PID %d\n", monitor_pid);
    } else {
        perror("fork");
    }
}

void stop_monitor() {
    if (monitor_pid == 0) {
        printf("Monitor not running.\n");
        return;
    }
    kill(monitor_pid, SIGTERM);
}

void send_command(const char *cmd) {
    if (monitor_pid == 0) {
        printf("Monitor not running.\n");
        return;
    }
    write(pipefd[1], cmd, strlen(cmd));
    write(pipefd[1], "\n", 1);
}

void calculate_scores() {
    DIR *d = opendir(".");
    if (!d) {
        perror("opendir");
        return;
    }
    struct dirent *ent;
    while ((ent = readdir(d))) {
        if (ent->d_type == DT_DIR && strncmp(ent->d_name, "Hunt", 4) == 0) {
            pid_t pid = fork();
            if (pid == 0) {
                execl("./score_calc", "./score_calc", ent->d_name, NULL);
                perror("execl score_calc");
                exit(1);
            }
        }
    }
    closedir(d);
    while (wait(NULL) > 0);
}

void calculate_scores_for_hunts() {
    DIR *dir = opendir(".");
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (entry->d_type != DT_DIR) continue;
        if (entry->d_name[0] == '.') continue; // Skip . and ..

        pid_t pid;
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("pipe");
            continue;
        }

        pid = fork();
        if (pid == 0) {
            // Child process
            close(pipefd[0]); // Close read end
            dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
            close(pipefd[1]);

            execl("./calculate_score", "calculate_score", entry->d_name, NULL);
            perror("execl failed");
            exit(1);
        } else {
            // Parent process
            close(pipefd[1]); // Close write end

            printf("Scores for %s:\n", entry->d_name);
            char buffer[256];
            ssize_t n;
            while ((n = read(pipefd[0], buffer, sizeof(buffer)-1)) > 0) {
                buffer[n] = '\0';
                printf("%s", buffer);
            }
            close(pipefd[0]);

            waitpid(pid, NULL, 0);
        }
    }

    closedir(dir);
}

int main() {
    char input[128];
    struct sigaction sa = {0};
    sa.sa_handler = handle_sigchld;
    sigaction(SIGCHLD, &sa, NULL);

    while (1) {
        printf("treasure_hub> ");
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = 0;

        char *args[MAX_ARGS];
        int argc = 0;
        char *token = strtok(input, " ");
        while (token && argc < MAX_ARGS) {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        if (argc == 0) continue;

        if (strcmp(args[0], "start_monitor") == 0) {
            start_monitor();
        } else if (strcmp(args[0], "stop_monitor") == 0) {
            stop_monitor();
        } else if (strcmp(args[0], "list_hunts") == 0) {
            send_command("list_hunts");
        } else if (strcmp(args[0], "list_treasures") == 0 && argc == 2) {
            char cmd[MAX_CMD];
            snprintf(cmd, sizeof(cmd), "list_treasures %s", args[1]);
            send_command(cmd);
        } else if (strcmp(args[0], "view_treasure") == 0 && argc == 3) {
            char cmd[MAX_CMD];
            snprintf(cmd, sizeof(cmd), "view_treasure %s %s", args[1], args[2]);
            send_command(cmd);
        } else if (strcmp(args[0], "calculate_score") == 0) {
            calculate_scores_for_hunts();
        } else if (strcmp(args[0], "exit") == 0) {
            if (monitor_pid) {
                printf("Monitor still running. Stop it first.\n");
            } else {
                break;
            }
        } else {
            printf("Unknown command\n");
        }
    }
    return 0;
}
