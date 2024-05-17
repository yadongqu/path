#pragma once
#include "flow_math.h"
#include <vector>
namespace flow {
using std::vector;
struct shape_data {
  vector<vec3f> positions;
  vector<vec3f> normals;
  vector<vec2f> texcoords;
};

shape_data make_cube(float scale = 1.0);
shape_data make_quad(float scale = 1.0);
} // namespace flow
