#pragma once
#include <glm/glm.hpp>

glm::vec3 sample_hemisphre_uniform(float u1, float u2);
float sample_hemisphere_uniform_pdf(const glm::vec3 &n, const glm::vec3 &d);

glm::vec3 sample_hemisphere_cosine(float u1, float u2);
float sample_hemisphere_cosine_pdf(const glm::vec3 &n, const glm::vec3 &d);
