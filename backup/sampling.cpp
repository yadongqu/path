#include "sampling.h"

namespace flow {
vec3f sample_hemisphre_uniform(double u1, double u2) {
  double phi = 2 * pif * u1;
  double cos_theta = u2;
  double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
  return normalize(
      vec3f(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta));
}
double sample_hemisphere_uniform_pdf(const vec3f &n, const vec3f &d) {
  return dot(n, d) > 0.0 ? 0.5 / pif : 0.0;
}

vec3f sample_hemisphere_cosine(double u1, double u2) {
  double phi = 2 * pif * u1;
  double cos_theta = sqrt(u2);
  double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
  return normalize(
      vec3f(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta));
}
double sample_hemisphere_cosine_pdf(const vec3f &n, const vec3f &d) {
  auto cosine = max(0.0, dot(n, d));
  return cosine / pif;
}
} // namespace flow
