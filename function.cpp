#include "function.h"

glm::vec3 trace_ray(const Ray &ray, const Scene &scene, RNG &rng,
                    int16_t bounces) {}

void render(const Scene &scene, const Camera &camera, Film &film) {
  auto height = film.height;
  auto width = film.width;
  auto samples = scene.samples;
  RNG rng{};
  film.buffer.resize(width * height);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      glm::vec3 color{0.0};
      for (int s = 0; s < samples; s++) {
        float u = (x + rng.next_1f()) / width * 2.0 - 1.0;
        float v = 1.0 - (y + rng.next_1f()) / height * 2.0;
        auto ray = camera.get_ray(u, v);
        color += trace_ray(ray, scene, rng, scene.bounces);
      }
      color /= samples;
      film.buffer[y * width + x] = color;
    }
  }
}