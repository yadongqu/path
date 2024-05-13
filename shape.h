#pragma once
#include "vec.h"
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
  vec3f_t *positions;
  int16_t *indices;
  int p_len;
  int i_len;
} mesh_t;

mesh_t *mesh_new(int p_len, int i_len) {
  mesh_t *mesh = malloc(sizeof(mesh_t));
  mesh->positions = malloc(p_len * sizeof(vec3f_t));
  mesh->indices = malloc(i_len * sizeof(int16_t));
  mesh->i_len = i_len;
  mesh->p_len = p_len;
  return mesh;
}

void mesh_free(mesh_t *mesh) {
  free(mesh->positions);
  free(mesh->indices);
  free(mesh);
}

mesh_t mesh_make(vec3f_t *positions, int16_t *indices, int p_len, int i_len) {
  mesh_t mesh = {{NULL, NULL, 0, 0}};
  mesh.positions = positions;
  mesh.indices = indices;
  mesh.i_len = i_len;
  mesh.p_len = p_len;
  return mesh;
}