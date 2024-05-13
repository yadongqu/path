#pragma once

#include "material.h"
#include "shape.h"
#include <time.h>
typedef struct {
  mesh_t *meshes;
  material_t *materials;
  int mesh_len;
  int material_len;
} scene_t;

scene_t scene_make() {
  scene_t scene = {
      .meshes = NULL, .materials = NULL, .mesh_len = 0, .material_len = 0};

  return scene;
}
