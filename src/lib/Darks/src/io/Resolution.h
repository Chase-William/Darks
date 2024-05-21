#ifndef DARKS_IO_RESOLUTION_H_
#define DARKS_IO_RESOLUTION_H_

#include "nlohmann/json.hpp"

namespace Darks::IO {
    struct Resolution {
    // public:
        //Resolution() { }

        /*Resolution(
            int width,
            int height
        );*/

        int width = 0;
        int height = 0;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(Resolution, width, height)
    };

    /*void to_json(nlohmann::json& json, const Resolution& res);

    void from_json(const nlohmann::json& json, Resolution& res);*/
}

#endif