#pragma once
#include "integrator.h"
#include <cstdint>
#include <optional>
#include <variant>
#include <vector>

#include "flow_math.h"

namespace flow {
const mat4f default_camera =
    glm::lookAt(vec3f(0.0, 0.0, 1.0), vec3f(0.0), vec3f(0.0, 1.0, 0.0));

struct Ray {
  vec3f origin;
  vec3f dir;

  vec3f at(double t) const { return origin + dir * t; }
};

struct Lambertian {
  vec3f color;
};

struct DiffuseLight {
  vec3f color;
  double intensity;
};

struct ScatterRecord {
  vec3f dir;
  vec3f attenuation;
  bool is_specular{false};
};

struct Material {

  static Material make_lambertian(const vec3f &color);
  static Material make_diffuse_light(const vec3f &color, double intensity);

  std::optional<ScatterRecord> sample(const vec3f &wo, const vec3f &n,
                                      RNG &rng) const;

  double pdf(const vec3f &wo, const vec3f &n, const vec3f &wi) const;

  vec3f emit() const;

  bool is_light() const;

  std::variant<Lambertian, DiffuseLight> m;
};

struct HitRecord {
  vec3f position;
  vec3f normal;
  double t;
  const class Mesh *mesh;
  bool is_inside = false;
};

struct Mesh {
  std::vector<vec3f> positions;
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

  vec3f sample_point(const vec3f &target, RNG &rng) const;
  double pdf(const vec3f &origin, const vec3f &dir) const;

  std::optional<double> hit_p(const Ray &ray, double tmin, double tmax) const;

  std::optional<HitRecord> hit(const Ray &ray, double tmin, double tmax) const;
};

struct Transform {
  mat4f model{mat4f(1.0)};
  mat4f imodel{mat4f(1.0)};

  static Transform from_imodel(const mat4f &imodel);
  static Transform from_model(const mat4f &model);
};

struct Film {
  std::vector<vec3f> buffer;
  uint16_t width;
  uint16_t height;

  void set(int x, int y, const vec3f &color) { buffer[y * width + x] = color; }
};

struct Camera {
  Camera(const vec3f &eye, const vec3f &target, const vec3f &up, double fov,
         double aspect);

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

} // namespace flow
