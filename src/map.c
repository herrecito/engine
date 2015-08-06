#include <stdio.h>
#include <stdlib.h>

#include "map.h"
#include "dbg.h"
#include "geometry.h"

Map *M_Load(const char *path) {
    Map *map = malloc(sizeof(struct Map));

    FILE *f = fopen(path, "r");

    Segment seg;

    int numwalls = 0;
    Wall *walls = NULL;

    int i = 0;
    while (fscanf(f, "%lf %lf %lf %lf",
                &seg.start.x, &seg.start.y, &seg.end.x, &seg.end.y) != EOF)
    {
        walls = realloc(walls, ++numwalls * sizeof(Wall));
        walls[i++] = (Wall){ .seg = seg, .seen = 0 };
    }

    fclose(f);

    map->walls = walls;
    map->numwalls = numwalls;

    return map;
}
