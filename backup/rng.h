#pragma once
#include "flow_math.h"
#include <random>
namespace flow {
struct RNG {
  std::random_device m_rd;
  std::mt19937 m_gen;
  std::uniform_real_distribution<float> m_dis;

  RNG() : m_gen(m_rd()), m_dis(0.0, 1.0) {}

  float next_1f();
  vec2f next_2f();
};
} // namespace flow
