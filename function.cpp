#include "function.h"
#include <algorithm>
#include <cstdio>
#include <execution>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <limits>
#include <ranges>
#include <vector>
glm::vec3 trace_ray(const Ray &ray, const Scene &scene, RNG &rng,
                    int16_t bounces) {
  if (bounces < 0) {
    return glm::vec3(0.0);
  }
  auto res = scene.hit(ray, 0.0001, std::numeric_limits<float>::max());
  if (!res.has_value()) {
    return glm::vec3(0.0);
  }
  auto rec = res.value();
  auto mesh = rec.mesh;
  auto material = mesh->material;
  auto emitted = material.emit();
  auto scatter = material.sample(-ray.dir, rec.normal, rng);
  if (!scatter.has_value()) {
    return emitted;
  }
  auto scatter_record = scatter.value();
  float prob = 0.5f;
  glm::vec3 next_dir = scatter_record.dir;
  if (rng.next_1f() < prob) {
    auto is_light = [](const Mesh &m) { return m.material.is_light(); };
    auto filtered = scene.meshes | std::views::filter(is_light);
    auto mesh = filtered.base().data();
    glm::vec3 p = mesh->sample_point(rec.position, rng);
    next_dir = glm::normalize(p - rec.position);
  }

  auto pdf = 0.5f * material.pdf(-ray.dir, rec.normal, next_dir) +
             0.5f * mesh->pdf(rec.position, next_dir);

  if (pdf < 0.001) {
    return emitted;
  }

  auto next_ray =
      Ray{.origin = rec.position + rec.normal * 0.0001f, .dir = next_dir};
  auto scatter_pdf = material.pdf(-ray.dir, rec.normal, next_dir);
  auto scattered_color = scatter_record.attenuation * scatter_pdf *
                         trace_ray(next_ray, scene, rng, bounces - 1) / pdf;
  return emitted + scattered_color;
}

struct Tile {
  int x;
  int y;
  int width;
  int height;
  std::vector<glm::vec3> buffer;
};

void render(const Scene &scene, const Camera &camera, Film &film) {
  auto height = film.height;
  auto width = film.width;
  auto samples = scene.samples;

  std::vector<Tile> tiles;
  int tile_size = 16;
  for (int x = 0; x < width; x += tile_size) {
    for (int y = 0; y < height; y += tile_size) {
      tiles.push_back(Tile{.x = x,
                           .y = y,
                           .width = glm::min(width - x, tile_size),
                           .height = glm::min(height - y, tile_size)});
    }
  }

  std::for_each(std::execution::par, tiles.begin(), tiles.end(),
                [film, samples, camera, scene](auto &tile) {
                  RNG rng{};
                  tile.buffer.resize(tile.width * tile.height);
                  for (int y = 0; y < tile.height; y++) {
                    for (int x = 0; x < tile.width; x++) {
                      int cur_x = x + tile.x;
                      int cur_y = y + tile.y;
                      glm::vec3 color{0.0};
                      for (int s = 0; s < samples; s++) {
                        float u =
                            (cur_x + rng.next_1f()) / film.width * 2.0 - 1.0;
                        float v =
                            1.0 - (cur_y + rng.next_1f()) / film.height * 2.0;
                        auto ray = camera.get_ray(u, v);
                        color += trace_ray(ray, scene, rng, scene.bounces);
                      }
                      color /= samples;
                      tile.buffer[y * tile.width + x] = color;
                    }
                  }
                });
  film.buffer.resize(width * height);

  for (auto &tile : tiles) {
    for (int x = tile.x; x < tile.width; x += 1) {
      for (int y = tile.y; y < tile.height; y += 1) {
        film.buffer[y * film.width + x] = tile.buffer[y * tile.width + x];
      }
    }
  }
}