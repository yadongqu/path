#include "data.h"
#include "integrator.h"
#include "rng.h"
#include <cstdint>

glm::dvec3 trace_ray(const Ray &ray, const Scene &scene, RNG &rng,
                     int16_t bounces);

void render(const Scene &scene, const Camera &camera,
            const Integrator &integrator, Film &film);