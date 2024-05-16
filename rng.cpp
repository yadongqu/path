#include "rng.h"

double RNG::next_1f() { return m_dis(m_gen); }

glm::dvec2 RNG::next_2f() { return glm::vec2(m_dis(m_gen), m_dis(m_gen)); }