#include "data.h"
#include "sampling.h"
#include "util.h"
#include <glm/common.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/matrix.hpp>
#include <limits>
#include <optional>

#include <variant>
Material Material::make_lambertian(const glm::vec3 &color) {
  return Material{.m = Lambertian{.color = color}};
}
Material Material::make_diffuse_light(const glm::vec3 &color, float intensity) {
  return Material{.m = DiffuseLight{.color = color, .intensity = intensity}};
}

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

std::optional<ScatterRecord>
Material::sample(const glm::vec3 &wo, const glm::vec3 &n, RNG &rng) const {
  std::optional<ScatterRecord> res{std::nullopt};
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
          res = std::make_optional(ScatterRecord{.dir = glm::normalize(dir),
                                                 .attenuation = material.color,
                                                 .is_specular = false});
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
                       glm::vec4(s * half_width, t * half_height, -1.0, 0.0));
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

std::optional<float> Mesh::hit_p(const Ray &ray, float tmin, float tmax) const {
  bool is_hit;
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
  auto res = is_hit ? std::make_optional(closest_so_far) : std::nullopt;
  return res;
}

std::optional<float> Scene::hit_p(const Ray &ray, float tmin,
                                  float tmax) const {
  bool is_hit = false;
  float closest_so_far = tmax;
  for (int i = 0; i < meshes.size(); i++) {
    auto res = meshes[i].hit_p(ray, tmin, closest_so_far);
    if (res.has_value()) {
      closest_so_far = res.value();
      is_hit = true;
    }
  }
  auto res = is_hit ? std::make_optional(closest_so_far) : std::nullopt;
  return res;
}

glm::vec3 Mesh::sample_point(const glm::vec3 &target, RNG &rng) const {
  auto index = (int)glm::floor(rng.next_1f() * (float)indices.size() / 3.0f);
  const auto &v0 = positions[indices[index * 3]];
  const auto &v1 = positions[indices[index * 3 + 1]];
  const auto &v2 = positions[indices[index * 3 + 2]];
  auto v = rng.next_1f();
  auto u = rng.next_1f();
  if (u + v > 1.0f) {
    u = 1.0 - u;
    v = 1.0 - v;
  }
  return v0 + (v1 - v0) * u + (v2 - v0) * v;
}

float Mesh::pdf(const glm::vec3 &origin, const glm::vec3 &dir) const {
  Ray ray{.origin = origin, .dir = dir};
  auto hit = hit_p(ray, 0.0001f, std::numeric_limits<float>::max());
  if (hit.has_value()) {
    return 1.0 / area();
  }
  return 0.0f;
}

std::optional<HitRecord> Scene::hit(const Ray &ray, float tmin,
                                    float tmax) const {
  std::optional<HitRecord> rec{std::nullopt};
  float closest_so_far = tmax;
  for (int i = 0; i < meshes.size(); i++) {
    auto res = meshes[i].hit(ray, tmin, closest_so_far);
    if (res.has_value()) {
      auto current_rec = res.value();
      closest_so_far = current_rec.t;
      rec = current_rec;
    }
  }
  return rec;
}

Camera::Camera(const glm::vec3 &eye, const glm::vec3 &target,
               const glm::vec3 &up, float fov, float aspect) {
  this->fov = fov;
  this->aspect = aspect;
  this->transform = Transform::from_imodel(glm::lookAt(eye, target, up));
}
