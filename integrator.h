#pragma once
#include <glm/glm.hpp>
#include <variant>
class Ray;
class Scene;
class RNG;

struct NormalIntegrator {
  glm::dvec3 li(const Ray &ray, const Scene &scene, RNG &rng) const;
};

struct PathIntegrator {

  glm::dvec3 trace_path(const Ray &ray, const Scene &scene, RNG &rng,
                        int16_t bounces) const;
  glm::dvec3 li(const Ray &ray, const Scene &scene, RNG &rng) const;
};

struct Integrator {
  static Integrator make_path() { return Integrator{PathIntegrator{}}; }
  static Integrator make_normal() { return Integrator{NormalIntegrator{}}; }

  glm::dvec3 li(const Ray &ray, const Scene &scene, RNG &rng) const;
  std::variant<NormalIntegrator, PathIntegrator> integrator;
};