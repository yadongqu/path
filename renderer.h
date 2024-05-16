#include "integrator.h"
#include "rng.h"
#include "scene_data.h"
#include <cstdint>

// glm::dvec3 trace_ray(const Ray &ray, const Scene &scene, RNG &rng,
//                      int16_t bounces);

Film render(const Scene &scene);