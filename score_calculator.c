#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define USERNAME_SIZE 32
#define CLUE_SIZE 128

typedef struct {
    int id;
    char username[USERNAME_SIZE];
    double lat, lon;
    char clue[CLUE_SIZE];
    int value;
} Treasure;

typedef struct {
    char username[USERNAME_SIZE];
    int total_value;
} UserScore;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hunt_dir>\n", argv[0]);
        return 1;
    }

    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s/treasures.dat", argv[1]);

    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        perror("open treasures.dat");
        return 1;
    }

    UserScore scores[100];
    int n_scores = 0;

    Treasure t;
    ssize_t r;

    while ((r = read(fd, &t, sizeof(Treasure))) == sizeof(Treasure)) {
        int i;
        for (i = 0; i < n_scores; i++) {
            if (strcmp(scores[i].username, t.username) == 0) {
                scores[i].total_value += t.value;
                break;
            }
        }
        if (i == n_scores && n_scores < 100) {
            strncpy(scores[n_scores].username, t.username, USERNAME_SIZE);
            scores[n_scores].total_value = t.value;
            n_scores++;
        }
    }
    close(fd);

    printf("Scores for hunt '%s':\n", argv[1]);
    for (int i = 0; i < n_scores; i++) {
        printf("User %-20s : %d\n", scores[i].username, scores[i].total_value);
    }
    return 0;
}
