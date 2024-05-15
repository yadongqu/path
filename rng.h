#pragma once
#include <glm/vec2.hpp>
#include <random>
struct RNG {
  std::random_device m_rd;
  std::mt19937 m_gen;
  std::uniform_real_distribution<float> m_dis;

  RNG() : m_gen(m_rd()), m_dis(0.0f, 1.0f) {}

  float next_1f();
  glm::vec2 next_2f();
};