#pragma once
#include "flow_math.h"
namespace flow {
struct camera_data {
    frame3f frame        = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {0, 0, 0}};
    bool    orthographic = false;
    float   lens         = 0.050f;
    float   film         = 0.036f;
    float   aspect       = 1.500f;
    float   focus        = 10000;
    float   aperture     = 0;
};

enum struct material_type {
    // clang-format off
    matte, glossy, reflective, transparent, refractive, subsurface, volumetric,
    gltfpbr
    // clang-format on
};

inline const auto material_type_names = std::vector<std::string>{"matte",
                                                                 "glossy", "reflective", "transparent", "refractive", "subsurface",
                                                                 "volumetric", "gltfpbr"};


}
