#pragma once
#include "integrator.h"
#include "rng.h"
#include <cstdint>
#include <glm/ext.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_double3.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <optional>
#include <variant>
#include <vector>

const glm::mat4 default_camera = glm::lookAt(
    glm::dvec3(0.0, 0.0, 1.0), glm::dvec3(0.0), glm::dvec3(0.0, 1.0, 0.0));

struct Ray {
  glm::dvec3 origin;
  glm::dvec3 dir;

  glm::dvec3 at(double t) const { return origin + dir * t; }
};

struct Lambertian {
  glm::dvec3 color;
};

struct DiffuseLight {
  glm::dvec3 color;
  double intensity;
};

struct ScatterRecord {
  glm::dvec3 dir;
  glm::dvec3 attenuation;
  bool is_specular{false};
};

struct Material {

  static Material make_lambertian(const glm::dvec3 &color);
  static Material make_diffuse_light(const glm::dvec3 &color, double intensity);

  std::optional<ScatterRecord> sample(const glm::dvec3 &wo, const glm::dvec3 &n,
                                      RNG &rng) const;

  double pdf(const glm::dvec3 &wo, const glm::dvec3 &n,
             const glm::dvec3 &wi) const;

  glm::dvec3 emit() const;

  bool is_light() const;

  std::variant<Lambertian, DiffuseLight> m;
};

struct HitRecord {
  glm::dvec3 position;
  glm::dvec3 normal;
  double t;
  const class Mesh *mesh;
  bool is_inside = false;
};

struct Mesh {
  std::vector<glm::dvec3> positions;
  std::vector<uint16_t> indices;
  Material material;

  double area() const {
    double res = 0.0;
    for (int i = 0; i < indices.size(); i += 3) {
      const auto &v0 = positions[indices[i]];
      const auto &v1 = positions[indices[i + 1]];
      const auto &v2 = positions[indices[i + 2]];
      res += glm::length(glm::cross(v1 - v0, v2 - v0));
    }
    return res / 2.0;
  }

  glm::dvec3 sample_point(const glm::dvec3 &target, RNG &rng) const;
  double pdf(const glm::dvec3 &origin, const glm::dvec3 &dir) const;

  std::optional<double> hit_p(const Ray &ray, double tmin, double tmax) const;

  std::optional<HitRecord> hit(const Ray &ray, double tmin, double tmax) const;
};

struct Transform {
  glm::mat4 model{glm::mat4(1.0)};
  glm::mat4 imodel{glm::mat4(1.0)};

  static Transform from_imodel(const glm::mat4 &imodel);
  static Transform from_model(const glm::mat4 &model);
};

struct Film {
  std::vector<glm::dvec3> buffer;
  uint16_t width;
  uint16_t height;

  void set(int x, int y, const glm::dvec3 &color) {
    buffer[y * width + x] = color;
  }
};

struct Camera {
  Camera(const glm::dvec3 &eye, const glm::dvec3 &target, const glm::dvec3 &up,
         double fov, double aspect);

  Transform transform{Transform::from_imodel(default_camera)};
  double fov{45.0};
  double aspect{1.0};
  // double near{0.001};
  // double far{1000.0};

  Ray get_ray(double s, double t) const;
};

struct Scene {
  std::vector<Mesh> meshes;
  Camera camera;
  Integrator integrator;
  uint16_t width;
  uint16_t height;
  int16_t bounces;
  int16_t samples;

  void add(const Mesh &mesh) { meshes.push_back(mesh); }

  std::optional<double> hit_p(const Ray &ray, double tmin, double tmax) const;

  std::optional<HitRecord> hit(const Ray &ray, double tmin, double tmax) const;
};
