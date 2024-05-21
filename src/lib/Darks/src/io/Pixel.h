#ifndef DARKS_IO_PIXEL_H_
#define DARKS_IO_PIXEL_H_

#include "../Framework.h"

#include "nlohmann/json.hpp"

#include <cstddef>
#include <optional>

namespace Darks::IO {
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

    // Conversion impls 
    // Refer to examples: https://github.com/nlohmann/json/blob/develop/tests/src/unit-udt.cpp    
    
    //
    //
    //  -- NOTE, BEWARE!!! Ordering of the following functions does matter and if incorrect will cause weird complication errors!
    //
    //

    // to_json functions (Serialize)

    static void to_json(nlohmann::json& json, const Point& p) {
        json = nlohmann::json({
            { "x", p.x },
            { "y", p.y }
        });
    }

    static void to_json(nlohmann::json& json, const Color& c) {
        json = nlohmann::json({
            { "red", c.red },
            { "green", c.green },
            { "blue", c.blue }
        });
    }

    static void to_json(nlohmann::json& json, const Rect& r) {
        json = nlohmann::json({
            { "left", r.left },
            { "top", r.top },
            { "right", r.right },
            { "bottom", r.bottom }
        });
    }

    static void to_json(nlohmann::json& json, const Pixel& p) {
        json = nlohmann::json({
            { "pos", p.pos },
            { "color", p.color }
        });
    }

    // from_json functions (Deserialize)

    static void from_json(const nlohmann::json& json, Point& p) {
        json.at("x").get_to(p.x);
        json.at("y").get_to(p.y);
    }

    static void from_json(const nlohmann::json& json, Color& p) {
        json.at("red").get_to(p.red);
        json.at("green").get_to(p.green);
        json.at("blue").get_to(p.blue);
    }

    static void from_json(const nlohmann::json& json, Rect& p) {
        json.at("left").get_to(p.left);
        json.at("top").get_to(p.top);
        json.at("right").get_to(p.right);
        json.at("bottom").get_to(p.bottom);
    }

    static void from_json(const nlohmann::json& json, Pixel& p) {
        json.at("pos").get_to(p.pos);
        json.at("color").get_to(p.color);
    }
}

#endif