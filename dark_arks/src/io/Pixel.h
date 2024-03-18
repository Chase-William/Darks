#pragma once

#include <Windows.h>
#include <cstddef>
#include <optional>

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