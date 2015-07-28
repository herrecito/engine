#ifndef _COLOR_
#define _COLOR_

#include <stdint.h>

typedef struct Color {
    uint8_t red, green, blue;
} Color;


#define HEX2COLOR(hex) (C_HexToColor((hex)))
#define COLOR2HEX(color) (C_ColorToHex((color)))


#define BLACK       ((Color){ 0x00, 0x00, 0x00 })
#define BLUE        ((Color){ 0x00, 0x00, 0xFF })
#define GREEN       ((Color){ 0x00, 0xFf, 0x00 })
#define GREY        ((Color){ 0x11, 0x11, 0x11 })
#define LIGHTGREY   ((Color){ 0x22, 0x22, 0x22 })
#define RED         ((Color){ 0xFF, 0x00, 0x00 })
#define WHITE       ((Color){ 0xFF, 0xFF, 0xFF })


Color C_HexToColor(int hex);
int C_ColorToHex(Color c);
Color C_ScaleColor(Color c, double intensity);

#endif
