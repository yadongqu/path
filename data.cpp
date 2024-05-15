#include "data.h"
#include "sampling.h"
#include "util.h"
#include <glm/common.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/matrix.hpp>
#include <optional>
#include <variant>
glm::vec3 Material::emit() const {
  return std::visit(
      [](auto &&material) {
        using T = std::decay_t<decltype(material)>;
        if constexpr (std::is_same_v<T, DiffuseLight>) {
          return material.color * material.intensity;
        } else {
          return glm::vec3(0.0);
        }
      },
      m);
}

bool Material::is_light() const {
  return std::visit(
      [](auto &&material) {
        using T = std::decay_t<decltype(material)>;
        if constexpr (std::is_same_v<T, DiffuseLight>) {
          return true;
        } else {
          return false;
        }
      },
      m);
}

std::optional<glm::vec3> Material::sample(const glm::vec3 &wo,
                                          const glm::vec3 &n, RNG &rng) const {
  std::optional<glm::vec3> res{std::nullopt};
  return std::visit(
      [&](auto &&material) {
        using T = std::decay_t<decltype(material)>;
        if constexpr (std::is_same_v<T, Lambertian>) {
          if (glm::dot(wo, n) < 0.0) {
            return res;
          }
          auto sample = sample_hemisphere_cosine(rng.next_1f(), rng.next_1f());
          glm::vec3 a = glm::abs(n.x) > 0.9 ? glm::vec3(0.0, 1.0, 0.0)
                                            : glm::vec3(1.0, 0.0, 0.0);
          glm::vec3 v = glm::cross(n, a);
          glm::vec3 u = glm::cross(n, v);
          glm::vec3 dir = u * sample.x + v * sample.y + n * sample.z;
          res = std::make_optional(glm::normalize(dir));
          return res;
        } else {
          return res;
        }
      },
      m);
}

float Material::pdf(const glm::vec3 &wo, const glm::vec3 &n,
                    const glm::vec3 &wi) const {
  return std::visit(
      [&](auto &&material) {
        using T = std::decay_t<decltype(material)>;
        if constexpr (std::is_same_v<T, Lambertian>) {
          return sample_hemisphere_cosine_pdf(n, wi);
        } else {
          return 0.0f;
        }
      },
      m);
}

Transform Transform::from_imodel(const glm::mat4 &imodel) {
  auto model = glm::inverse(imodel);
  return Transform{.model = model, .imodel = imodel};
}

Transform Transform::from_model(const glm::mat4 &model) {
  auto imodel = glm::inverse(model);
  return Transform{.model = model, .imodel = imodel};
}

Ray Camera::get_ray(float s, float t) const {
  auto theta = glm::radians(fov);
  auto half_height = glm::tan(theta / 2.0);
  auto half_width = aspect * half_height;
  auto origin = glm::vec3(transform.model[3]);
  auto dir = glm::vec3(transform.model *
                       glm::vec4(s * half_width, t * half_height, 1.0, 0.0));
  return Ray{.origin = origin, .dir = glm::normalize(dir)};
}

std::optional<HitRecord> Mesh::hit(const Ray &ray, float tmin,
                                   float tmax) const {
  std::optional<HitRecord> res{std::nullopt};
  float closest_so_far = tmax;
  for (int i = 0; i < indices.size(); i += 3) {
    const auto &v0 = positions[indices[i]];
    const auto &v1 = positions[indices[i + 1]];
    const auto &v2 = positions[indices[i + 2]];
    float t;
    if (ray_triangle_intersect(ray, v0, v1, v2, t) && t > tmin &&
        t < closest_so_far) {
      if (res.has_value()) {
        res->position = ray.at(t);
        res->t = t;
        res->normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
      } else {
        res = std::make_optional(HitRecord{
            .position = ray.at(t),
            .normal = glm::normalize(glm::cross(v1 - v0, v2 - v0)),
            .t = t,
            .mesh = this,
        });
      }
      closest_so_far = t;
    }
  }
  return res;
}

bool Mesh::hit_p(const Ray &ray, float tmin, float tmax) const {
  bool is_hit = false;
  float closest_so_far = tmax;

  for (int i = 0; i < indices.size(); i += 3) {
    const auto &v0 = positions[indices[i]];
    const auto &v1 = positions[indices[i + 1]];
    const auto &v2 = positions[indices[i + 2]];
    float t;
    if (ray_triangle_intersect(ray, v0, v1, v2, t) && t > tmin &&
        t < closest_so_far) {
      closest_so_far = t;
      is_hit = true;
    }
  }

  return is_hit;
}
