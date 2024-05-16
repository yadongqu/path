#pragma once
#include <glm/glm.hpp>

glm::dvec3 sample_hemisphre_uniform(double u1, double u2);
double sample_hemisphere_uniform_pdf(const glm::dvec3 &n, const glm::dvec3 &d);

glm::dvec3 sample_hemisphere_cosine(double u1, double u2);
double sample_hemisphere_cosine_pdf(const glm::dvec3 &n, const glm::dvec3 &d);
