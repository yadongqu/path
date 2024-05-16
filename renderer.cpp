#include "renderer.h"
#include "integrator.h"
#include "rng.h"
#include "scene_data.h"
#include <cstdio>
#include <future>
#include <glm/common.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>

#include <vector>

struct Tile {
  int x;
  int y;
  int width;
  int height;
  std::vector<glm::dvec3> buffer;
};

Film render(const Scene &scene) {
  uint16_t width = scene.width;
  uint16_t height = scene.height;

  Film film{
      .width = scene.width,
      .height = scene.height,
  };
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

  auto do_render = [](const Scene &scene, Tile &tile) {
    RNG rng{};
    auto &integrator = scene.integrator;
    auto &camera = scene.camera;
    tile.buffer.resize(tile.width * tile.height);

    for (int y = 0; y < tile.height; y++) {
      for (int x = 0; x < tile.width; x++) {
        int cur_x = x + tile.x;
        int cur_y = y + tile.y;
        glm::dvec3 color{0.0};
        for (int s = 0; s < scene.samples; s++) {
          double u = (cur_x + rng.next_1f()) / scene.width * 2.0 - 1.0;
          double v = 1.0 - (cur_y + rng.next_1f()) / scene.height * 2.0;

          auto ray = camera.get_ray(u, v);

          // color += glm::abs(ray.dir);

          color += integrator.li(ray, scene, rng);
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
                                     std::ref(scene), std::ref(tile)));
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
  return film;
}
