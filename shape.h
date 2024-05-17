#pragma once
#include <glm/glm.hpp>
#include <vector>
using glm::fvec3;
using glm::fvec2;
using std::vector;
struct shape_data {
    vector<fvec3> positions;
    vector<fvec3> normals;
    vector<fvec2> texcoords;
};


shape_data make_cube(float scale = 1.0);
shape_data make_quad(float scale = 1.0);
