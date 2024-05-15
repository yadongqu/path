
#include <cstdint>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "scenes.h"
#include <stb/stb_image_write.h>

#include "function.h"
int main() {
  auto [scene, camera, film] = build_cornell_scene();
  scene.samples = 100;
  scene.bounces = 3;
  render(scene, camera, film);

  std::vector<uint8_t> data;

  auto clamp = [](const float v) { return glm::min(glm::max(0.0f, v), 1.0f); };

  for (auto &color : film.buffer) {
    uint8_t x = clamp(color.x) * 255.9f;
    uint8_t y = clamp(color.y) * 255.9f;
    uint8_t z = clamp(color.z) * 255.9f;
    data.push_back(x);
    data.push_back(y);
    data.push_back(z);
  }
  stbi_write_png("output.png", film.width, film.height, 3, data.data(),
                 film.width * 3);
}