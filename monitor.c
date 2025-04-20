#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t got_command = 0;

void handler(int sig) {
    got_command = 1;
}

int main() {
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    sigaction(SIGTERM, &sa, NULL);

    while (1) {
        pause(); // așteaptă semnal

        if (got_command) {
            FILE *f = fopen(".command.txt", "r");
            if (!f) continue;

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
            got_command = 0;
        }
    }

    return 0;
}
