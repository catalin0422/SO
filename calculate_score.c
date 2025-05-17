#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USERNAME_SIZE 32
#define CLUE_SIZE 128

typedef struct {
    int treasure_id;
    char username[USERNAME_SIZE];
    double latitude;
    double longitude;
    char clue[CLUE_SIZE];
    int value;
} Treasure;

typedef struct UserScore {
    char username[USERNAME_SIZE];
    int score;
    struct UserScore *next;
} UserScore;

UserScore* find_or_create_user(UserScore **head, const char *username) {
    UserScore *cur = *head;
    while (cur) {
        if (strcmp(cur->username, username) == 0)
            return cur;
        cur = cur->next;
    }
    // Not found, create new
    UserScore *new_user = malloc(sizeof(UserScore));
    strcpy(new_user->username, username);
    new_user->score = 0;
    new_user->next = *head;
    *head = new_user;
    return new_user;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s hunt_id\n", argv[0]);
        return 1;
    }

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/treasures.dat", argv[1]);

    FILE *f = fopen(filepath, "rb");
    if (!f) {
        perror("fopen");
        return 1;
    }

    Treasure t;
    UserScore *users = NULL;

    while (fread(&t, sizeof(Treasure), 1, f) == 1) {
        UserScore *user = find_or_create_user(&users, t.username);
        user->score += t.value;
    }
    fclose(f);

    // Print scores
    for (UserScore *cur = users; cur != NULL; ) {
        printf("%s %d\n", cur->username, cur->score);
        UserScore *tmp = cur;
        cur = cur->next;
        free(tmp);
    }

    return 0;
}
