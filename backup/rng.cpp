#include "rng.h"

namespace flow {
float RNG::next_1f() { return m_dis(m_gen); }

vec2f RNG::next_2f() { return vec2f(m_dis(m_gen), m_dis(m_gen)); }
} // namespace flow
