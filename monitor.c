// monitor.c - Reads commands from stdin and outputs results

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_LINE 256
#define TREASURE_FILE "treasures.dat"

struct Treasure {
    char id[16];
    char user[32];
    double lat;
    double lon;
    char clue[64];
    int value;
};

void list_hunts() {
    DIR *d = opendir(".");
    if (!d) return;
    struct dirent *ent;
    while ((ent = readdir(d))) {
        if (ent->d_type == DT_DIR && strncmp(ent->d_name, "Hunt", 4) == 0) {
            char path[128];
            snprintf(path, sizeof(path), "%s/%s", ent->d_name, TREASURE_FILE);
            int fd = open(path, O_RDONLY);
            if (fd < 0) continue;
            struct Treasure t;
            int count = 0;
            while (read(fd, &t, sizeof(t)) == sizeof(t)) count++;
            close(fd);
            printf("%s: %d treasures\n", ent->d_name, count);
        }
    }
    closedir(d);
}

void list_treasures(const char *hunt_id) {
    char path[128];
    snprintf(path, sizeof(path), "%s/%s", hunt_id, TREASURE_FILE);
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("Could not open hunt %s\n", hunt_id);
        return;
    }
    struct Treasure t;
    while (read(fd, &t, sizeof(t)) == sizeof(t)) {
        printf("ID: %s | User: %s | GPS: %.2f,%.2f | Value: %d\n",
               t.id, t.user, t.lat, t.lon, t.value);
    }
    close(fd);
}

void view_treasure(const char *hunt_id, const char *treasure_id) {
    char path[128];
    snprintf(path, sizeof(path), "%s/%s", hunt_id, TREASURE_FILE);
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("Could not open hunt %s\n", hunt_id);
        return;
    }
    struct Treasure t;
    while (read(fd, &t, sizeof(t)) == sizeof(t)) {
        if (strcmp(t.id, treasure_id) == 0) {
            printf("Treasure %s:\n  User: %s\n  GPS: %.2f, %.2f\n  Clue: %s\n  Value: %d\n",
                   t.id, t.user, t.lat, t.lon, t.clue, t.value);
            close(fd);
            return;
        }
    }
    printf("Treasure not found.\n");
    close(fd);
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "list_hunts") == 0) {
            list_hunts();
        } else if (strncmp(line, "list_treasures", 15) == 0) {
            char hunt[64];
            if (sscanf(line, "list_treasures %63s", hunt) == 1)
                list_treasures(hunt);
        } else if (strncmp(line, "view_treasure", 13) == 0) {
            char hunt[64], tid[16];
            if (sscanf(line, "view_treasure %63s %15s", hunt, tid) == 2)
                view_treasure(hunt, tid);
        }
    }
    // simulate delay
    usleep(500000);
    return 0;
}