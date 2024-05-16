#include "data.h"
#include "rng.h"
#include <cstdint>

glm::dvec3 trace_ray(const Ray &ray, const Scene &scene, RNG &rng,
                     int16_t bounces);

void render(const Scene &scene, const Camera &camera, Film &film);