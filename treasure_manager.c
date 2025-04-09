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

int main(){

    return 0;
}