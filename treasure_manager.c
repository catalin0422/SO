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

int main(){

    return 0;
}