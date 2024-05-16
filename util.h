#pragma once
#include "data.h"
bool ray_triangle_intersect(const Ray &ray, const glm::dvec3 &v0,
                            const glm::dvec3 &v1, const glm::dvec3 &v2,
                            double &t);
