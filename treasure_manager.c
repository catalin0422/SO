#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

void handle_signal(int sig) {
    if (sig == SIGUSR1) {
        printf("Comori actualizate pentru vânătoarea monitorizată!\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s --add <hunt_id>\n", argv[0]);
        return 1;
    }

    signal(SIGUSR1, handle_signal); // Setează handler-ul pentru semnal

    if (strcmp(argv[1], "--add") == 0 && argc == 3) {
        char *hunt_id = argv[2];
        char path[256];

        // Creează directorul pentru vânătoare
        snprintf(path, sizeof(path), "%s", hunt_id);
        if (mkdir(path, 0777) == -1 && errno != EEXIST) {
            perror("mkdir");
            return 1;
        }

        // Creează fișierul treasures.dat
        snprintf(path, sizeof(path), "%s/treasures.dat", hunt_id);
        int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd == -1) {
            perror("open");
            return 1;
        }
        close(fd);

        // Creează un pipe pentru a transmite hunt_id copilului
        int pipe_fd[2];
        if (pipe(pipe_fd) == -1) {
            perror("pipe");
            return 1;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        } else if (pid == 0) {
            // Copil
            close(pipe_fd[1]); // Închide capătul de scriere
            dup2(pipe_fd[0], STDIN_FILENO); // Redirecționează stdin către pipe
            close(pipe_fd[0]);
            execlp("./treasure_hub", "./treasure_hub", NULL);
            perror("execlp");
            exit(1);
        } else {
            // Părinte
            close(pipe_fd[0]); // Închide capătul de citire
            write(pipe_fd[1], hunt_id, strlen(hunt_id));
            write(pipe_fd[1], "\n", 1);
            close(pipe_fd[1]);

            // Așteaptă semnal de la copil (se poate folosi și un loop în proiect complet)
            pause();
        }
    } else {
        fprintf(stderr, "Comandă necunoscută sau argumente insuficiente.\n");
        return 1;
    }

    return 0;
}
