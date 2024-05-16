#include "function.h"
#include "data.h"
#include "rng.h"
#include <algorithm>
#include <cstdio>
#include <future>
#include <glm/common.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <limits>
#include <ranges>
#include <vector>
glm::dvec3 trace_ray(const Ray &ray, const Scene &scene, RNG &rng,
                     int16_t bounces) {
  if (bounces <= 0) {
    return glm::dvec3(0.0);
  }
  auto res = scene.hit(ray, 0.001, std::numeric_limits<double>::max());
  if (!res.has_value()) {
    return glm::dvec3(0.0);
  }
  // printf("hit\n");
  auto rec = res.value();

  // return rec.normal;
  // return rec.normal;
  auto mesh = rec.mesh;
  auto material = mesh->material;

  auto emitted = material.emit();

  if (res->is_inside) {
    return emitted;
  }

  auto scatter = material.sample(-ray.dir, rec.normal, rng);
  if (!scatter.has_value()) {
    return emitted;
  }

  auto scatter_record = scatter.value();
  double prob = 0.5;
  glm::dvec3 next_dir = scatter_record.dir;
  // printf("%f\n", rng.next_1f());
  if (rng.next_1f() < prob) {
    auto is_light = [](const Mesh &m) { return m.material.is_light(); };
    auto filtered = scene.meshes | std::views::filter(is_light);
    auto mesh = filtered.base().data();
    glm::dvec3 p = mesh->sample_point(rec.position, rng);
    next_dir = glm::normalize(p - rec.position);
  }

  auto pdf = 0.5 * material.pdf(-ray.dir, rec.normal, next_dir) +
             0.5 * mesh->pdf(rec.position, next_dir);

  if (pdf < 0.0001) {
    return emitted;
  }

  auto next_ray =
      Ray{.origin = rec.position + rec.normal * 0.0001, .dir = next_dir};
  auto scatter_pdf = material.pdf(-ray.dir, rec.normal, next_dir);
  // printf("scatter pdf %f\n", scatter_pdf);
  auto scattered_color = scatter_record.attenuation * scatter_pdf *
                         trace_ray(next_ray, scene, rng, bounces - 1) / pdf;

  return emitted + scattered_color;
}

struct Tile {
  int x;
  int y;
  int width;
  int height;
  std::vector<glm::dvec3> buffer;
};

void render(const Scene &scene, const Camera &camera, Film &film) {
  auto height = film.height;
  auto width = film.width;
  auto samples = scene.samples;

  std::vector<Tile> tiles;
  int tile_size = 64;
  for (int x = 0; x < width; x += tile_size) {
    for (int y = 0; y < height; y += tile_size) {
      tiles.push_back(Tile{.x = x,
                           .y = y,
                           .width = glm::min(width - x, tile_size),
                           .height = glm::min(height - y, tile_size)});
    }
  }
  film.buffer.resize(width * height);
  std::vector<std::future<bool>> tile_render;

  // for (int i = 0; i < tiles.size(); i++) {
  //   auto &tile = tiles[i];
  //   tile.buffer.resize(tile.width * tile.height);
  //   RNG rng{};
  //   for (int y = 0; y < tile.height; y++) {
  //     for (int x = 0; x < tile.width; x++) {
  //       int cur_x = x + tile.x;
  //       int cur_y = y + tile.y;
  //       glm::dvec3 color{0.0};
  //       for (int s = 0; s < samples; s++) {
  //         double u = (cur_x + rng.next_1f()) / film.width * 2.0 - 1.0;
  //         double v = 1.0 - (cur_y + rng.next_1f()) / film.height * 2.0;
  //         auto ray = camera.get_ray(u, v);
  //         color += glm::abs(ray.dir);
  //         // printf("%f %f %f\n", color.x, color.y, color.z);
  //         // color += trace_ray(ray, scene, rng, scene.bounces);
  //       }
  //       color /= samples;
  //       tile.buffer[y * tile.width + x] = color;
  //     }
  //   }
  // }

  auto do_render = [](const Scene &scene, const Camera &camera,
                      const Film &film, Tile &tile) {
    RNG rng{};
    tile.buffer.resize(tile.width * tile.height);
    for (int y = 0; y < tile.height; y++) {
      for (int x = 0; x < tile.width; x++) {
        int cur_x = x + tile.x;
        int cur_y = y + tile.y;
        glm::dvec3 color{0.0};
        for (int s = 0; s < scene.samples; s++) {
          double u = (cur_x + rng.next_1f()) / film.width * 2.0 - 1.0;
          double v = 1.0 - (cur_y + rng.next_1f()) / film.height * 2.0;

          auto ray = camera.get_ray(u, v);

          // color += glm::abs(ray.dir);

          color += trace_ray(ray, scene, rng, scene.bounces);
          // printf("color %f %f %f\n", color.x, color.y, color.z);
          // assert(!glm::isnan(color.x) && !glm::isnan(color.y) &&
          //        !glm::isnan(color.z));
        }
        color /= scene.samples;
        tile.buffer[y * tile.width + x] = color;
      }
    }
    printf("finished tile \n");
    return true;
  };

  for (int i = 0; i < tiles.size(); i++) {
    auto &tile = tiles[i];
    tile_render.push_back(std::async(std::launch::async, do_render,
                                     std::ref(scene), std::ref(camera),
                                     std::ref(film), std::ref(tile)));
  }
  for (int i = 0; i < tile_render.size(); i++) {
    auto done = tile_render[i].get();
    auto tile = tiles[i];
    for (int y = 0; y < tile.height; y++) {
      for (int x = 0; x < tile.width; x++) {
        int cur_y = y + tile.y;
        int cur_x = x + tile.x;
        film.buffer[cur_y * film.width + cur_x] =
            tile.buffer[y * tile.width + x];
      }
    }
  }
}
