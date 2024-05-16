
#include <cstdint>
#include <glm/common.hpp>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "renderer.h"
#include "scenes.h"
#include <stb/stb_image_write.h>
int main() {
  auto scene = build_triangle_scene();
  // auto [scene, camera, film] = build_cornell_scene();

  auto film = render(scene);

  std::vector<uint8_t> data;

  auto clamp = [](const double v) { return glm::min(glm::max(0.0, v), 1.0); };

  for (auto &color : film.buffer) {
    if (glm::isnan(color.x)) {
      color.x = 0.0;
    }
    if (glm::isnan(color.y)) {
      color.x = 0.0;
    }
    if (glm::isnan(color.y)) {
      color.x = 0.0;
    }
    uint8_t x = glm::floor(clamp(color.x) * 255.9);
    uint8_t y = glm::floor(clamp(color.y) * 255.9);
    uint8_t z = glm::floor(clamp(color.z) * 255.9);
    data.push_back(x);
    data.push_back(y);
    data.push_back(z);
  }
  stbi_write_png("output.png", film.width, film.height, 3, data.data(),
                 film.width * 3);
}