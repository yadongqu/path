#pragma once
#include "flow_math.h"
#include <variant>
class Ray;
class Scene;
class RNG;
namespace flow {
struct NormalIntegrator {
  vec3f li(const Ray &ray, const Scene &scene, RNG &rng) const;
};

struct PathIntegrator {
  vec3f trace_path(const Ray &ray, const Scene &scene, RNG &rng,
                   int16_t bounces) const;
  vec3f li(const Ray &ray, const Scene &scene, RNG &rng) const;
};

struct Integrator {
  static Integrator make_path() { return Integrator{PathIntegrator{}}; }
  static Integrator make_normal() { return Integrator{NormalIntegrator{}}; }

  vec3f li(const Ray &ray, const Scene &scene, RNG &rng) const;
  std::variant<NormalIntegrator, PathIntegrator> integrator;
};
} // namespace flow
