#pragma once
#include "rng.h"
#include <cstdint>
#include <glm/ext.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <optional>
#include <variant>
#include <vector>

const glm::mat4 default_camera = glm::lookAt(
    glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

struct Ray {
  glm::vec3 origin;
  glm::vec3 dir;

  glm::vec3 at(float t) const { return origin + dir * t; }
};

struct Lambertian {
  glm::vec3 color;
};

struct DiffuseLight {
  glm::vec3 color;
  float intensity;
};

struct Material {

  std::optional<glm::vec3> sample(const glm::vec3 &wo, const glm::vec3 &n,
                                  RNG &rng) const;

  float pdf(const glm::vec3 &wo, const glm::vec3 &n, const glm::vec3 &wi) const;

  glm::vec3 emit() const;

  bool is_light() const;

  std::variant<Lambertian, DiffuseLight> m;
};

struct HitRecord {
  glm::vec3 position;
  glm::vec3 normal;
  float t;
  const class Mesh *mesh;
};

struct Mesh {
  std::vector<glm::vec3> positions;
  std::vector<uint16_t> indices;

  bool hit_p(const Ray &ray, float tmin, float tmax) const;

  std::optional<HitRecord> hit(const Ray &ray, float tmin, float tmax) const;
};

struct Transform {
  glm::mat4 model{glm::mat4(1.0f)};
  glm::mat4 imodel{glm::mat4(1.0f)};

  static Transform from_imodel(const glm::mat4 &imodel);
  static Transform from_model(const glm::mat4 &model);
};

struct Film {
  std::vector<glm::vec3> buffer;
  uint16_t width;
  uint16_t height;
};

struct Camera {
  Transform transform{Transform::from_imodel(default_camera)};
  float fov{45.0};
  float aspect{1.0};
  // float near{0.001};
  // float far{1000.0};

  Ray get_ray(float s, float t) const;
};

struct Scene {
  std::vector<Mesh> meshes;
  int16_t bounces;
  int16_t samples;

  bool hit_p(const Ray &ray, float tmin, float tmax);

  std::optional<HitRecord> hit(const Ray &ray, float tmin, float tmax);
};
