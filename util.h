#pragma once
#include "data.h"
bool ray_triangle_intersect(const Ray &ray, const glm::vec3 &v0,
                            const glm::vec3 &v1, const glm::vec3 &v2, float &t);
