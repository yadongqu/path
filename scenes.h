#pragma once

#include "material.h"
#include "math.h"
#include "scene.h"
#include "shape.h"
#include "vec.h"
#include <stdint.h>
#include <stdlib.h>

scene_t *build_cornell() {
  const int WHITE = 0;
  const int RED = 1;
  const int GREEN = 2;
  const int LIGHT = 3;

  material_t *materials = malloc(4 * sizeof(material_t));
  materials[WHITE].color = vec4f_make(1.0f, 1.0f, 1.0f, 1.0f);

  materials[RED].color = vec4f_make(1.0f, 0.0f, 0.0f, 1.0f);

  materials[GREEN].color = vec4f_make(1.0f, 1.0f, 1.0f, 1.0f);
  materials[LIGHT].color = vec4f_make(1.0f, 1.0f, 1.0f, 1.0f);
  materials[LIGHT].emittance = 100.0f;

  const int FLOOR = 0;
  const int CEILING = 1;
  const int LI = 2;
  const int BACK_WALL = 3;
  const int RIGHT_WALL = 4;
  const int LEFT_WALL = 5;

  vec3f_t floor_positions[4] = {{
                                    {0.0f, 0.0f, 0.0f},
                                },
                                {{0.0f, 0.0f, 559.2f}},
                                {{556.0, 0.0, 559.2}},
                                {{556.0, 0.0, 0.0}}};
  vec3f_t ceiling_positions[4] = {{{0.0, 548.9, 0.0}},
                                  {{556.0, 548.9, 0.0}},
                                  {{556.0, 548.9, 559.2}},
                                  {{0.0, 548.9, 559.2}}};
  vec3f_t light_positions[4] = {{{343.0, 548.8, 227.0}},
                                {{343.0, 548.8, 332.0}},
                                {{213.0, 548.8, 332.0}},
                                {{213.0, 548.8, 227.0}}};

  vec3f_t back_wall_positions[4] = {{{0.0f, 0.0f, 559.2}},
                                    {{0.0, 548.9, 559.2}},
                                    {{556.0, 548.9, 559.2}},
                                    {{556.0, 0.0, 559.2}}};

  vec3f_t right_wall_positions[4] = {
      {{0.0, 0.0, 0.0}},
      {{0.0, 548.9, 0.0}},
      {{0.0, 548.9, 559.2}},
      {{0.0, 0.0, 559.2}},
  };
  vec3f_t left_wall_positions[4] = {
      {{556.0, 0.0, 0.0}},
      {{556.0, 0.0, 559.2}},
      {{556.0, 548.9, 559.2}},
      {{556.0, 548.9, 0.0}},
  };

  vec3f_t positions[6][4] = {floor_positions,      ceiling_positions,
                             light_positions,      back_wall_positions,
                             right_wall_positions, left_wall_positions};

  int16_t indices[6] = {0, 1, 2, 1, 2, 3};

  int p_len = 4;
  int i_len = 6;

  mesh_t *meshes = malloc(6 * sizeof(mesh_t));

  for (int i = 0; i < 6; i++) {
    meshes[i].positions =
  }

  meshes[FLOOR].indices = (int16_t *)&indices;
  meshes[FLOOR].positions = (vec3f_t *)&floor_positions;
  meshes[FLOOR].i_len = i_len;
  meshes[FLOOR].p_len = p_len;
}