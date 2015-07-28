#include "color.h"

Color C_HexToColor(int hex) {
    Color c;

    c.blue = hex & 0x0000FF;
    c.green = (hex & 0x00FF00) >> 8;
    c.red = (hex & 0xFF0000) >> 16;

    return c;
}


int C_ColorToHex(Color c) {
    return c.red << 16 | c.green << 8 | c.blue;
}

Color C_ScaleColor(Color c, double intensity) {
    c.red *= intensity;
    c.green *= intensity;
    c.blue *= intensity;

    return c;
}
