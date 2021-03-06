#include <stdio.h>

#include <SDL.h>

#include "system.h"
#include "geometry.h"
#include "buffer.h"
#include "draw.h"
#include "defs.h"
#include "map.h"
#include "color.h"


#define SNAP_DISTANCE 8

//------------------------------------------------------------------------------
// WallNodes
//------------------------------------------------------------------------------

typedef struct WallNode {
    Wall *wall;

    struct WallNode *next, *prev;
} WallNode;


typedef struct {
    WallNode *first;
} WallList;


// Create a WallNode from a Wall.
WallNode *CreateWallNode(Wall *wall, WallNode *next, WallNode *prev) {
    WallNode *node = malloc(sizeof(WallNode));

    node->wall = wall;
    node->next = next;
    node->prev = prev;

    return node;
}



// Add a Wall to a WallList.
//
// Returns a pointer to the WallNode used to store the Wall.
WallNode *UnshiftWallNode(WallList *list, Wall *wall) {
    WallNode *node = CreateWallNode(wall, list->first, NULL);

    if (list->first) {
        list->first->prev = node;
    }

    list->first = node;

    return node;
}



// Delete a WallNode from a WallList.
//
// Returns a pointer to the stored Wall.
Wall *DeleteWallNode(WallList *list, WallNode *n) {
    Wall *wall = n->wall;

    if (n->prev) {
        n->prev->next = n->next;
    }

    if (n->next) {
        n->next->prev = n->prev;
    }

    if (list->first == n) {
        list->first = n->next;
    }

    free(n);

    return wall;
}



// Search for a WallNode containing Wall and returns it if found.
// NULL otherwise.
WallNode *GetWallNode(WallList *list, Wall *wall) {
    WallNode *wn = list->first;

    while (wn) {
        if (wn->wall == wall) return wn;

        wn = wn->next;
    }

    return NULL;
}


// Free all Wall and WallNodes in list.
void ClearWallList(WallList *list) {
    while (list->first) {
        Wall *w = DeleteWallNode(list, list->first);
        free(w);
    }
}



//------------------------------------------------------------------------------
// Globals
//------------------------------------------------------------------------------

Buffer *buffer;

WallList level;

Vector loose;  // The start point of a Wall under construction.
int loosef;



//------------------------------------------------------------------------------
// Editor
//------------------------------------------------------------------------------

void Init() {
    S_Init("Editor", 640, 480);
    buffer = B_CreateBuffer(640, 480);
}



void AddWall(Segment seg) {
    Wall *wall = malloc(sizeof(Wall));
    wall->seg = seg;

    UnshiftWallNode(&level, wall);
}



void DeleteWall(Wall *wall) {
    WallNode *wn = GetWallNode(&level, wall);

    if (wn) {
        DeleteWallNode(&level, wn);
        free(wall);
    }
}



Wall *GetWallNearMouse() {
    Vector pos = S_GetMousePos(buffer);

    for (WallNode *wn = level.first; wn; wn = wn->next) {
        if (G_SegmentPointDistance(wn->wall->seg, pos) < SNAP_DISTANCE) {
            return wn->wall;
        }
    }

    return NULL;
}



void SaveLevel(const char *path) {
    FILE *f = fopen(path, "w");

    for (WallNode *wn = level.first; wn; wn = wn->next) {
        fprintf(f, "%f %f %f %f\n",
                wn->wall->seg.start.x,
                wn->wall->seg.start.y,
                wn->wall->seg.end.x,
                wn->wall->seg.end.y);
    }

    fclose(f);
}

void LoadLevel(const char *path) {
    ClearWallList(&level);

    FILE *f = fopen(path, "r");

    Segment seg;
    while (fscanf(f, "%lf %lf %lf %lf",
                &seg.start.x, &seg.start.y, &seg.end.x, &seg.end.y) != EOF ) {
        AddWall(seg);
    }

    fclose(f);
}



void Quit() {
    ClearWallList(&level);
    S_Quit();
    exit(1);
}



void Draw() {
    B_ClearBuffer(buffer, BLACK);

    for (WallNode *wn = level.first; wn; wn = wn->next) {
        uint32_t color;
        if (G_SegmentPointDistance(wn->wall->seg, S_GetMousePos(buffer)) < SNAP_DISTANCE) {
            color = RED;
        } else {
            color = WHITE;
        }

        D_DrawSegment(buffer, wn->wall->seg, color);
    }

    if (loosef) {
        D_DrawSegment(buffer, (Segment){ loose, S_GetMousePos(buffer) }, WHITE);
    }

    S_Blit(buffer);
}



void HandleLeftClick() {
    Vector pos = S_GetMousePos(buffer);

    Wall *wall = GetWallNearMouse();
    if (wall) {
        pos = G_NearestPointOnSegment(wall->seg, pos);
    }

    if (loosef) {
        AddWall((Segment){ loose, pos });
        loosef = 0;
    } else {
        loose = pos;
        loosef = 1;
    }
}



void HandleRightClick() {
    if (loosef) {
        loosef = 0;
    }

    Wall *wall = GetWallNearMouse();
    if (wall) {
        DeleteWall(wall);
    }
}


void Input() {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type)  {
            case SDL_KEYDOWN:
                switch (ev.key.keysym.sym) {
                    case 'q':
                        Quit();
                        break;

                    case 's':
                        SaveLevel("level.map");
                        puts("Saved.");
                        break;

                    case 'l':
                        LoadLevel("level.map");
                        puts("Loaded.");
                        break;

                    case 'c':
                        ClearWallList(&level);
                        puts("Cleared.");
                        break;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                switch (ev.button.button) {
                    case SDL_BUTTON_LEFT:
                        HandleLeftClick();
                        break;

                    case SDL_BUTTON_RIGHT:
                        HandleRightClick();
                        break;
                }
                break;
        }
    }
}

int main() {
    Init();

    while (1) {
        Input();
        Draw();
    }

    Quit();
}
