#pragma once
#include "flow_math.h"

namespace flow {
vec3f sample_hemisphre_uniform(double u1, double u2);
double sample_hemisphere_uniform_pdf(const vec3f &n, const vec3f &d);

vec3f sample_hemisphere_cosine(double u1, double u2);
double sample_hemisphere_cosine_pdf(const vec3f &n, const vec3f &d);
} // namespace flow
