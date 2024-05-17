#include "shape.h"
namespace flow {
shape_data make_cube(float scale) {
  static const auto cube_positions = vector<vec3f>{
      {-1, -1, +1}, {+1, -1, +1}, {+1, +1, +1}, {-1, +1, +1}, {+1, -1, -1},
      {-1, -1, -1}, {-1, +1, -1}, {+1, +1, -1}, {+1, -1, +1}, {+1, -1, -1},
      {+1, +1, -1}, {+1, +1, +1}, {-1, -1, -1}, {-1, -1, +1}, {-1, +1, +1},
      {-1, +1, -1}, {-1, +1, +1}, {+1, +1, +1}, {+1, +1, -1}, {-1, +1, -1},
      {+1, -1, +1}, {-1, -1, +1}, {-1, -1, -1}, {+1, -1, -1}};
  static const auto cube_normals = vector<vec3f>{
      {0, 0, +1}, {0, 0, +1}, {0, 0, +1}, {0, 0, +1}, {0, 0, -1}, {0, 0, -1},
      {0, 0, -1}, {0, 0, -1}, {+1, 0, 0}, {+1, 0, 0}, {+1, 0, 0}, {+1, 0, 0},
      {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {-1, 0, 0}, {0, +1, 0}, {0, +1, 0},
      {0, +1, 0}, {0, +1, 0}, {0, -1, 0}, {0, -1, 0}, {0, -1, 0}, {0, -1, 0}};
  static const auto cube_texcoords = vector<vec2f>{
      {0, 1}, {1, 1}, {1, 0}, {0, 0}, {0, 1}, {1, 1}, {1, 0}, {0, 0},
      {0, 1}, {1, 1}, {1, 0}, {0, 0}, {0, 1}, {1, 1}, {1, 0}, {0, 0},
      {0, 1}, {1, 1}, {1, 0}, {0, 0}, {0, 1}, {1, 1}, {1, 0}, {0, 0}};

  return {.positions = cube_positions,
          .normals = cube_normals,
          .texcoords = cube_texcoords};
}

shape_data make_quad(float scale) {
  static const auto quad_positions =
      vector<vec3f>{{-1, -1, 0}, {+1, -1, 0}, {+1, +1, 0}, {-1, +1, 0}};
  static const auto quad_normals =
      vector<vec3f>{{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}};
  static const auto quad_texcoords =
      vector<vec2f>{{0, 1}, {1, 1}, {1, 0}, {0, 0}};

  return {.positions = quad_positions,
          .normals = quad_normals,
          .texcoords = quad_texcoords};
}
} // namespace flow
