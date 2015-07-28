#include <stdio.h>
#include <stdlib.h>

#include "map.h"
#include "geometry.h"



Map *M_Load(const char *path) {
    Map *map = malloc(sizeof(struct Map));

    FILE *f = fopen(path, "r");

    Vector start, end;
    int height;

    int numwalls = 0;
    Wall *walls = NULL;

    int i = 0;
    while (fscanf(f, "%lf %lf %lf %lf %d",
                &start.x, &start.y, &end.x, &end.y, &height) != EOF) {
        walls = realloc(walls, ++numwalls * sizeof(Wall));
        walls[i++] = (Wall){
            .seg = (Segment){ start, end },
            .height = height
        };
    }

    fclose(f);

    map->walls = walls;
    map->numwalls = numwalls;

    return map;
}



struct MapIterator {
    Map *map;
    int pos;
};


MapIterator *M_GetIterator(Map *map) {
    MapIterator *it = malloc(sizeof(struct MapIterator));

    it->map = map;
    it->pos = -1;

    return it;
}



Wall *M_GetNext(MapIterator *it) {
    if (it->pos < it->map->numwalls) {
        return &it->map->walls[it->pos++];
    } else {
        return NULL;
    }
}

void M_GoBeforeFirst(MapIterator *it) {
    it->pos = -1;
}



void M_DeleteIterator(MapIterator *it) {
    free(it);
}

