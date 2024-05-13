#pragma once

#include <assert.h>
#include <math.h>

#define FLOAT_EPSILON 1e-4f

#define R 0
#define G 1
#define B 2
#define A 3

#define X 0
#define Y 1
#define Z 2
#define W 3

#define U 0
#define V 1

typedef struct vec2f_t {
  float c[2];
} vec2f_t;

typedef struct {
  float c[3];
} vec3f_t;

typedef struct {
  float c[4];
} vec4f_t;

vec2f_t vec2f_make(float x, float y) {
  vec2f_t res = {{x, y}};
  return res;
}

// TODO: MAYBE PASS BY POINTER?
vec2f_t vec2f_add(vec2f_t v0, vec2f_t v1) {
  vec2f_t res = vec2f_make(0, 0);
  for (int i = 0; i < 2; i++) {
    res.c[i] = v0.c[i] + v1.c[i];
  }
  return res;
}

vec2f_t vec2f_sub(vec2f_t v0, vec2f_t v1) {
  vec2f_t res = vec2f_make(0, 0);
  for (int i = 0; i < 2; i++) {
    res.c[i] = v0.c[i] - v1.c[i];
  }
  return res;
}

vec2f_t vec2f_scale(vec2f_t v, float s) {
  vec2f_t res = vec2f_make(0, 0);
  for (int i = 0; i < 2; i++) {
    res.c[i] = v.c[i] * s;
  }
  return res;
}

float vec2f_dot(vec2f_t v0, vec2f_t v1) {
  float res = 0;
  for (int i = 0; i < 2; i++) {
    res += v0.c[i] * v1.c[i];
  }
  return res;
}

float vec2f_length2(vec2f_t v) { return vec2f_dot(v, v); }

float vec2f_length(vec2f_t v) { return sqrtf(vec2f_length2(v)); }

vec2f_t vec2f_normalize(vec2f_t v) {
  float l = vec2f_length(v);
  assert(l > FLOAT_EPSILON);
  return vec2f_scale(v, 1.0f / l);
}

vec2f_t vec2f_min(vec2f_t v0, vec2f_t v1) {
  vec2f_t res = vec2f_make(0, 0);
  for (int i = 0; i < 2; i++) {
    if (v0.c[i] > v1.c[i]) {
      res.c[i] = v1.c[i];
    } else {
      res.c[i] = v0.c[i];
    }
  }
  return res;
}

vec2f_t vec2f_max(vec2f_t v0, vec2f_t v1) {
  vec2f_t res = vec2f_make(0, 0);
  for (int i = 0; i < 2; i++) {
    if (v0.c[i] < v1.c[i]) {
      res.c[i] = v1.c[i];
    } else {
      res.c[i] = v0.c[i];
    }
  }
  return res;
}

vec3f_t vec3f_make(float x, float y, float z) {
  vec3f_t res = {{x, y, z}};
  return res;
}

vec3f_t vec3f_add(vec3f_t v0, vec3f_t v1) {
  vec3f_t res = vec3f_make(0, 0, 0);
  for (int i = 0; i < 3; i++) {
    res.c[i] = v0.c[i] + v1.c[i];
  }
  return res;
}

vec3f_t vec3f_sub(vec3f_t v0, vec3f_t v1) {
  vec3f_t res = vec3f_make(0, 0, 0);
  for (int i = 0; i < 3; i++) {
    res.c[i] = v0.c[i] - v1.c[i];
  }
  return res;
}

vec3f_t vec3f_scale(vec3f_t v, float s) {
  vec3f_t res = vec3f_make(0, 0, 0);
  for (int i = 0; i < 3; i++) {
    res.c[i] = v.c[i] * s;
  }
  return res;
}

float vec3f_dot(vec3f_t v0, vec3f_t v1) {
  float res = 0;
  for (int i = 0; i < 3; i++) {
    res += v0.c[i] * v1.c[i];
  }
  return res;
}

vec3f_t vec3f_cross(vec3f_t v0, vec3f_t v1) {
  vec3f_t res = vec3f_make(0, 0, 0);
  res.c[0] = v0.c[1] * v1.c[2] - v0.c[2] * v1.c[1];
  res.c[1] = v0.c[2] * v1.c[0] - v0.c[0] * v1.c[2];
  res.c[2] = v0.c[0] * v1.c[1] - v0.c[1] * v1.c[0];
  return res;
}

float vec3f_length2(vec3f_t v) { return vec3f_dot(v, v); }

float vec3f_length(vec3f_t v) { return sqrt(vec3f_length2(v)); }

vec3f_t vec3f_normalize(vec3f_t v) {
  float l = vec3f_length(v);
  assert(l > FLOAT_EPSILON);
  return vec3f_scale(v, 1.0f / l);
}

vec3f_t vec3f_max(vec3f_t v0, vec3f_t v1) {
  vec3f_t res = vec3f_make(0, 0, 0);
  for (int i = 0; i < 3; i++) {
    if (v0.c[i] < v1.c[i]) {
      res.c[i] = v1.c[i];
    } else {
      res.c[i] = v0.c[i];
    }
  }
  return res;
}

vec3f_t vec3f_min(vec3f_t v0, vec3f_t v1) {
  vec3f_t res = vec3f_make(0, 0, 0);
  for (int i = 0; i < 3; i++) {
    if (v0.c[i] > v1.c[i]) {
      res.c[i] = v1.c[i];
    } else {
      res.c[i] = v0.c[i];
    }
  }
  return res;
}

vec4f_t vec4f_make(float x, float y, float z, float w) {
  vec4f_t res = {{x, y, z, w}};
  return res;
}
