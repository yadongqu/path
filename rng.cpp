#include "rng.h"

float RNG::next_1f() { return m_dis(m_gen); }

glm::vec2 RNG::next_2f() { return glm::vec2(m_dis(m_gen), m_dis(m_gen)); }