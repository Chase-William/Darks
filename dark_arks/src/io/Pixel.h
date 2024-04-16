#ifndef DARKS_IO_PIXEL_H_
#define DARKS_IO_PIXEL_H_

#ifndef DPP 
#define DPP 
#include "dpp/dpp.h"
#endif	
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <cstddef>
#include <optional>

struct Rect {
public:
    int left;
    int top;
    int right;
    int bottom;

    Rect(
        int left, int top, int right, int bottom
    ) :
        left(left),
        top(top),
        right(right),
        bottom(bottom)
    { }

    int Width() const { return right - left; }
    int Height() const { return bottom - top; }
};

/// <summary>
/// Represents a RGB color value.
/// </summary>
struct Color {
public:
    Color() : red(0), green(0), blue(0) { }
    Color(BYTE red, BYTE green, BYTE blue) {
        this->red = red;
        this->green = green;
        this->blue = blue;
    }

    bool operator==(Color other) {
        return this->red == other.red && this->green == other.green && this->blue == other.blue;
    }

    BYTE red;
    BYTE green;
    BYTE blue;
};

/// <summary>
/// Represents a 2d vector having a x and y coordinate value.
/// </summary>
struct Point {
public:
    int x;
    int y;
};

/// <summary>
/// Represents a pixel via its (x ,y) position and its color.
/// </summary>
struct Pixel {
public:
    Pixel(Point pos, Color color) {
        this->pos = pos;
        this->color = color;
    }

    Point pos;
    Color color;
};

#endif