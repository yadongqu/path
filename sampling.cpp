#include "sampling.h"
#include <glm/ext.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/scalar_constants.hpp>
glm::dvec3 sample_hemisphre_uniform(double u1, double u2) {
  double phi = 2 * glm::pi<double>() * u1;
  double cos_theta = u2;
  double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
  return glm::normalize(
      glm::dvec3(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta));
}
double sample_hemisphere_uniform_pdf(const glm::dvec3 &n, const glm::dvec3 &d) {
  return glm::dot(n, d) > 0.0 ? 0.5 / glm::pi<double>() : 0.0;
}

glm::dvec3 sample_hemisphere_cosine(double u1, double u2) {
  double phi = 2 * glm::pi<double>() * u1;
  double cos_theta = sqrt(u2);
  double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
  return glm::normalize(
      glm::dvec3(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta));
}
double sample_hemisphere_cosine_pdf(const glm::dvec3 &n, const glm::dvec3 &d) {
  auto cosine = glm::max(0.0, glm::dot(n, d));
  return cosine / glm::pi<double>();
}
