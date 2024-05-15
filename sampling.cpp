#include "sampling.h"
#include <glm/ext.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/scalar_constants.hpp>
glm::vec3 sample_hemisphre_uniform(float u1, float u2) {
  float phi = 2 * glm::pi<float>() * u1;
  float cos_theta = u2;
  float sin_theta = sqrt(1 - cos_theta * cos_theta);
  return glm::normalize(
      glm::vec3(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta));
}
float sample_hemisphere_uniform_pdf(const glm::vec3 &n, const glm::vec3 &d) {
  return glm::dot(n, d) > 0.0 ? 0.5 / glm::pi<float>() : 0;
}

glm::vec3 sample_hemisphere_cosine(float u1, float u2) {
  float phi = 2 * glm::pi<float>() * u1;
  float cos_theta = sqrt(u2);
  float sin_theta = sqrt(1 - cos_theta * cos_theta);
  return glm::normalize(
      glm::vec3(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta));
}
float sample_hemisphere_cosine_pdf(const glm::vec3 &n, const glm::vec3 &d) {
  auto cosine = glm::max(0.0f, glm::dot(n, d));
  return cosine / glm::pi<float>();
}
