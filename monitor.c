#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

volatile sig_atomic_t got_command = 0;
volatile sig_atomic_t stop = 0;
pid_t manager_pid = -1;

void usr1_handler(int sig) {
    got_command = 1;
}

void term_handler(int sig) {
    stop = 1;
}

time_t get_file_mtime(const char *path) {
    struct stat attr;
    if (stat(path, &attr) == -1) return 0;
    return attr.st_mtime;
}

int main() {
    struct sigaction sa_usr1, sa_term;
    sa_usr1.sa_handler = usr1_handler;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    sigaction(SIGUSR1, &sa_usr1, NULL);

    sa_term.sa_handler = term_handler;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    sigaction(SIGTERM, &sa_term, NULL);

    // Citește PID-ul treasure_manager
    FILE *pid_file = fopen(".manager_pid", "r");
    if (!pid_file) {
        perror("fopen .manager_pid");
        return 1;
    }
    fscanf(pid_file, "%d", &manager_pid);
    fclose(pid_file);
    printf("Monitor: got treasure_manager PID = %d\n", manager_pid);

    time_t last_mtime = get_file_mtime("hunt1/treasures.dat");

    while (!stop) {
        pause(); // Așteaptă comenzi sau semnal extern

        if (got_command) {
            FILE *f = fopen(".command.txt", "r");
            if (f) {
                char cmd[100];
                fgets(cmd, sizeof(cmd), f);
                cmd[strcspn(cmd, "\n")] = 0;
                fclose(f);

                if (strcmp(cmd, "list_hunts") == 0) {
                    printf("Hunt1: 3 treasures\nHunt2: 5 treasures\n");
                } else if (strcmp(cmd, "list_treasures") == 0) {
                    printf("Treasure1: loc, descriere, etc.\n");
                } else if (strcmp(cmd, "view_treasure") == 0) {
                    printf("Viewing treasure details...\n");
                }
            }
            got_command = 0;
        }

        // Verifică modificarea fisierului treasures.dat 
        time_t current_mtime = get_file_mtime("hunt1/treasures.dat");
        if (current_mtime != 0 && current_mtime != last_mtime) {
            printf("Monitor: treasures.dat modified! Sending SIGUSR1 to manager.\n");
            kill(manager_pid, SIGUSR1);
            last_mtime = current_mtime;
        }

        
        usleep(500000); 
    }

    printf("Monitor exiting.\n");
    return 0;
}
