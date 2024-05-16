#pragma once
#include <glm/vec2.hpp>
#include <random>
struct RNG {
  std::random_device m_rd;
  std::mt19937 m_gen;
  std::uniform_real_distribution<double> m_dis;

  RNG() : m_gen(m_rd()), m_dis(0.0, 1.0) {}

  double next_1f();
  glm::dvec2 next_2f();
};