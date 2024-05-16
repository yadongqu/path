#pragma once
#include "data.h"
#include <variant>
struct NormalIntegrator {
  glm::dvec3 li(const Ray &ray, const Scene &scene, RNG &rng,
                int16_t bounces) const;
};

struct PathIntegrator {
  glm::dvec3 li(const Ray &ray, const Scene &scene, RNG &rng,
                int16_t bounces) const;
};

struct Integrator {
  static Integrator make_path() { return Integrator{PathIntegrator{}}; }
  static Integrator make_normal() { return Integrator{NormalIntegrator{}}; }

  glm::dvec3 li(const Ray &ray, const Scene &scene, RNG &rng,
                int16_t bounces) const;
  std::variant<NormalIntegrator, PathIntegrator> integrator;
};