#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define USERNAME_SIZE 32
#define CLUE_SIZE 128
#define FILENAME_SIZE 256

typedef struct {
    int treasure_id;
    char username[USERNAME_SIZE];
    double latitude;
    double longitude;
    char clue[CLUE_SIZE];
    int value;
} Treasure;

void log_action(const char *hunt_dir, const char *action) {
    char log_path[FILENAME_SIZE];
    snprintf(log_path, sizeof(log_path), "%s/logged_hunt", hunt_dir);
    int log_fd = open(log_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd < 0) {
        perror("log open");
        return;
    }
    dprintf(log_fd, "%s\n", action);
    close(log_fd);
}

void create_symlink(const char *hunt_id) {
    char link_name[FILENAME_SIZE], target_path[FILENAME_SIZE];
    snprintf(link_name, sizeof(link_name), "logged_hunt-%s", hunt_id);
    snprintf(target_path, sizeof(target_path), "%s/logged_hunt", hunt_id);
    symlink(target_path, link_name);
}

void add_treasure(const char *hunt_id) {
    char dir_path[FILENAME_SIZE];
    snprintf(dir_path, sizeof(dir_path), "%s", hunt_id);
    mkdir(dir_path, 0755);

    char file_path[FILENAME_SIZE];
    snprintf(file_path, sizeof(file_path), "%s/treasures.dat", hunt_id);
    int fd = open(file_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) { perror("open"); return; }

    Treasure t;
    printf("Enter Treasure ID: "); scanf("%d", &t.treasure_id);
    printf("Enter Username: "); scanf("%s", t.username);
    printf("Enter Latitude: "); scanf("%lf", &t.latitude);
    printf("Enter Longitude: "); scanf("%lf", &t.longitude);
    printf("Enter Clue: "); getchar(); fgets(t.clue, CLUE_SIZE, stdin);
    t.clue[strcspn(t.clue, "\n")] = 0;
    printf("Enter Value: "); scanf("%d", &t.value);

    write(fd, &t, sizeof(Treasure));
    close(fd);

    log_action(hunt_id, "ADD");
    create_symlink(hunt_id);
}

void list_treasures(const char *hunt_id) {
    char file_path[FILENAME_SIZE];
    snprintf(file_path, sizeof(file_path), "%s/treasures.dat", hunt_id);
    int fd = open(file_path, O_RDONLY);
    if (fd < 0) { perror("open"); return; }

    struct stat st;
    if (stat(file_path, &st) == 0) {
        printf("Hunt: %s\nSize: %lld bytes\nLast Modified: %ld\n", hunt_id, st.st_size, st.st_mtime);
    }

    Treasure t;
    printf("\n-- Treasures --\n");
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        printf("ID: %d | User: %s | Coords: %.4f, %.4f | Value: %d\n",
            t.treasure_id, t.username, t.latitude, t.longitude, t.value);
    }
    close(fd);
    log_action(hunt_id, "LIST");
}

void view_treasure(const char *hunt_id, int target_id) {
    char file_path[FILENAME_SIZE];
    snprintf(file_path, sizeof(file_path), "%s/treasures.dat", hunt_id);
    int fd = open(file_path, O_RDONLY);
    if (fd < 0) { perror("open"); return; }

    Treasure t;
    while (read(fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        if (t.treasure_id == target_id) {
            printf("ID: %d\nUser: %s\nCoords: %.4f, %.4f\nClue: %s\nValue: %d\n",
                   t.treasure_id, t.username, t.latitude, t.longitude, t.clue, t.value);
            close(fd);
            log_action(hunt_id, "VIEW");
            return;
        }
    }
    printf("Treasure ID %d not found.\n", target_id);
    close(fd);
}

void remove_treasure(const char *hunt_id, int target_id) {
    char file_path[FILENAME_SIZE], temp_path[FILENAME_SIZE];
    snprintf(file_path, sizeof(file_path), "%s/treasures.dat", hunt_id);
    snprintf(temp_path, sizeof(temp_path), "%s/temp.dat", hunt_id);

    int in_fd = open(file_path, O_RDONLY);
    int out_fd = open(temp_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (in_fd < 0 || out_fd < 0) { perror("open"); return; }

    Treasure t;
    while (read(in_fd, &t, sizeof(Treasure)) == sizeof(Treasure)) {
        if (t.treasure_id != target_id)
            write(out_fd, &t, sizeof(Treasure));
    }
    close(in_fd);
    close(out_fd);
    rename(temp_path, file_path);
    log_action(hunt_id, "REMOVE_TREASURE");
}

void remove_hunt(const char *hunt_id) {
    char file_path[FILENAME_SIZE];
    snprintf(file_path, sizeof(file_path), "%s/treasures.dat", hunt_id);
    unlink(file_path);

    snprintf(file_path, sizeof(file_path), "%s/logged_hunt", hunt_id);
    unlink(file_path);

    rmdir(hunt_id);

    char link_path[FILENAME_SIZE];
    snprintf(link_path, sizeof(link_path), "logged_hunt-%s", hunt_id);
    unlink(link_path);

    printf("Hunt %s removed.\n", hunt_id);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: treasure_manager --command hunt_id [id]\n");
        return 1;
    }

    const char *cmd = argv[1];
    const char *hunt_id = argv[2];

    if (strcmp(cmd, "--add") == 0) add_treasure(hunt_id);
    else if (strcmp(cmd, "--list") == 0) list_treasures(hunt_id);
    else if (strcmp(cmd, "--view") == 0 && argc == 4) view_treasure(hunt_id, atoi(argv[3]));
    else if (strcmp(cmd, "--remove_treasure") == 0 && argc == 4) remove_treasure(hunt_id, atoi(argv[3]));
    else if (strcmp(cmd, "--remove_hunt") == 0) remove_hunt(hunt_id);
    else printf("Invalid command or arguments.\n");

    return 0;
}
