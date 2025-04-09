#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#define MAX_TREASURES 100

typedef struct {
    float x;
    float y;
} Coordinates;

typedef struct {
    int treasureID;
    char username[100];
    Coordinates c;
    char clue[100];
    int value;
} Treasure;

void add(const char *hunt_id) {
    mkdir(hunt_id, 0777);
    char path[256];
    snprintf(path, sizeof(path), "%s/treasures.dat", hunt_id);

    FILE *f = fopen(path, "ab");
    if (!f) { 
        perror("fopen"); 
        return; 
    }

    Treasure t;
    printf("Username: ");
    scanf("%s", t.username);
    printf("Latitude: "); 
    scanf("%f", &t.c.x);
    printf("Longitude: "); 
    scanf("%f", &t.c.y);
    getchar();
    printf("Clue: "); 
    fgets(t.clue, sizeof(t.clue), stdin);
    t.clue[strcspn(t.clue, "\n")] = 0;
    printf("Value: "); 
    scanf("%d", &t.value);

    FILE *rf = fopen(path, "rb");
    int count = 0; 
    Treasure tmp;
    if (rf) {
        while (fread(&tmp, sizeof(Treasure), 1, rf) == 1) 
        count++;
        fclose(rf);
    }

    t.treasureID = count + 1;
    fwrite(&t, sizeof(Treasure), 1, f);
    fclose(f);
    printf("Am adaugat comoara cu id-ul %d\n", t.treasureID);
}

void list(const char *hunt_id){
    char path[256];
    snprintf(path, sizeof(path), "%s/treasures.dat", hunt_id);

    struct stat st;
    if (stat(path, &st) == -1) {
        perror("stat"); return;
    }

    printf("Hunt name: %s\n", hunt_id);
    printf("File size: %lld bytes\n", st.st_size);
    printf("Last modified: %s", ctime(&st.st_mtime));

    FILE *f = fopen(path, "rb");
    if (!f) { 
        perror("fopen"); 
        return; }

    Treasure t;
    while (fread(&t, sizeof(Treasure), 1, f) == 1) {
        printf("\nID: %d\nUser: %s\nCoords: (%.2f, %.2f)\nClue: %s\nValue: %d\n",
               t.treasureID, t.username, t.c.x, t.c.y, t.clue, t.value);
    }
    fclose(f);
}

void view(const char *hunt_id, int id) {
    char path[256];
    snprintf(path, sizeof(path), "%s/treasures.dat", hunt_id);

    FILE *f = fopen(path, "rb");
    if (!f) { 
        perror("fopen"); 
        return; 
    }

    Treasure t;
    int found = 0;
    while (fread(&t, sizeof(Treasure), 1, f) == 1) {
        if (t.treasureID == id) {
            printf("ID: %d\nUser: %s\nCoords: (%.2f, %.2f)\nClue: %s\nValue: %d\n",
                   t.treasureID, t.username, t.c.x, t.c.y, t.clue, t.value);
            found = 1;
            break;
        }
    }
    
    if (!found) printf("Treasure with ID %d not found.\n", id);
    fclose(f);
}

void removeTreasure(const char *hunt_id, int id) {
    char path[256];
    snprintf(path, sizeof(path), "%s/treasures.dat", hunt_id);

    FILE *f = fopen(path, "rb");
    if (!f) { perror("fopen"); return; }

    Treasure list[MAX_TREASURES];
    int count = 0;
    while (fread(&list[count], sizeof(Treasure), 1, f) == 1)
        if (list[count].treasureID != id) count++;
    fclose(f);

    f = fopen(path, "wb");
    if (!f) { perror("fopen write"); return; }
    for (int i = 0; i < count; i++)
        fwrite(&list[i], sizeof(Treasure), 1, f);
    fclose(f);
    printf("Removed treasure with ID %d if it existed.\n", id);
}

void removeHunt(const char *hunt_id) {
    char path[256];
    snprintf(path, sizeof(path), "%s/treasures.dat", hunt_id);
    remove(path);
    snprintf(path, sizeof(path), "%s", hunt_id);
    if (rmdir(path) == 0)
        printf("Removed hunt %s\n", hunt_id);
    else
        perror("rmdir");
}

int main(){

    return 0;
}