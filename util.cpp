#include "util.h"

bool ray_triangle_intersect(const Ray &ray, const glm::vec3 &v0,
                            const glm::vec3 &v1, const glm::vec3 &v2,
                            float &t) {

  auto e1 = v1 - v0;
  auto e2 = v2 - v0;
  auto h = glm::cross(ray.dir, e2);
  auto a = glm::dot(e1, h);
  if (a > -0.00001 && a < 0.00001) {
    return false;
  }
  auto f = 1.0 / a;
  auto s = ray.origin - v0;
  auto u = f * glm::dot(s, h);
  if (u < 0.0 || u > 1.0) {
    return false;
  }
  auto q = glm::cross(s, e1);
  auto v = f * glm::dot(ray.dir, q);
  if (v < 0.0 || u + v > 1.0) {
    return false;
  }
  t = f * glm::dot(e2, q);
  return t;
}
