#include "data.h"
#include "sampling.h"
#include "util.h"
#include <cassert>
#include <cstdio>
#include <glm/common.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/matrix.hpp>
#include <limits>
#include <optional>

#include <variant>
Material Material::make_lambertian(const glm::dvec3 &color) {
  return Material{.m = Lambertian{.color = color}};
}
Material Material::make_diffuse_light(const glm::dvec3 &color,
                                      double intensity) {
  return Material{.m = DiffuseLight{.color = color, .intensity = intensity}};
}

glm::dvec3 Material::emit() const {
  return std::visit(
      [](auto &&material) {
        using T = std::decay_t<decltype(material)>;
        if constexpr (std::is_same_v<T, DiffuseLight>) {
          return material.color * material.intensity;
        } else {
          return glm::dvec3(0.0);
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
Material::sample(const glm::dvec3 &wo, const glm::dvec3 &n, RNG &rng) const {
  std::optional<ScatterRecord> res{std::nullopt};
  return std::visit(
      [&](auto &&material) {
        using T = std::decay_t<decltype(material)>;
        if constexpr (std::is_same_v<T, Lambertian>) {
          // if (glm::dot(wo, n) <= 0.0) {
          //   return res;
          // }
          auto sample = sample_hemisphere_cosine(rng.next_1f(), rng.next_1f());
          glm::dvec3 a = glm::abs(n.x) > 0.9 ? glm::dvec3(0.0, 1.0, 0.0)
                                             : glm::dvec3(1.0, 0.0, 0.0);
          glm::dvec3 v = glm::normalize(glm::cross(n, a));
          glm::dvec3 u = glm::normalize(glm::cross(n, v));
          glm::dvec3 dir = u * sample.x + v * sample.y + n * sample.z;
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

double Material::pdf(const glm::dvec3 &wo, const glm::dvec3 &n,
                     const glm::dvec3 &wi) const {
  return std::visit(
      [&](auto &&material) {
        using T = std::decay_t<decltype(material)>;
        if constexpr (std::is_same_v<T, Lambertian>) {
          return sample_hemisphere_cosine_pdf(n, wi);
        } else {
          return 0.0;
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

Ray Camera::get_ray(double s, double t) const {
  auto theta = glm::radians(fov);
  auto half_height = glm::tan(theta / 2.0);
  auto half_width = aspect * half_height;

  auto origin = glm::dvec3(transform.model[3]);
  // printf("%f %f %f\n", origin.x, origin.y, origin.z);
  auto dir = glm::dvec3(transform.model *
                        glm::dvec4(s * half_width, t * half_height, -1.0, 0.0));
  return Ray{.origin = origin, .dir = glm::normalize(dir)};
}

std::optional<HitRecord> Mesh::hit(const Ray &ray, double tmin,
                                   double tmax) const {
  std::optional<HitRecord> res{std::nullopt};
  double closest_so_far = tmax;
  for (int i = 0; i < indices.size(); i += 3) {
    const auto &v0 = positions[indices[i]];
    const auto &v1 = positions[indices[i + 1]];
    const auto &v2 = positions[indices[i + 2]];
    double t;
    if (ray_triangle_intersect(ray, v0, v1, v2, t) && t > tmin &&
        t < closest_so_far) {
      // printf("hit\n");
      if (res.has_value()) {
        res->position = ray.at(t);
        res->t = t;
        res->normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        res->is_inside = false;
        if (glm::dot(ray.dir, res->normal) > 0.0) {
          res->normal = -res->normal;
          res->is_inside = true;
        }
      } else {
        res = std::make_optional(HitRecord{
            .position = ray.at(t),
            .normal = glm::normalize(glm::cross(v1 - v0, v2 - v0)),
            .t = t,
            .mesh = this,
            .is_inside = false,
        });
        if (glm::dot(ray.dir, res->normal) > 0.0) {
          res->normal = -res->normal;
          res->is_inside = true;
        }
      }
      closest_so_far = t;
    }
  }
  return res;
}

std::optional<double> Mesh::hit_p(const Ray &ray, double tmin,
                                  double tmax) const {
  bool is_hit;
  double closest_so_far = tmax;
  for (int i = 0; i < indices.size(); i += 3) {
    const auto &v0 = positions[indices[i]];
    const auto &v1 = positions[indices[i + 1]];
    const auto &v2 = positions[indices[i + 2]];
    double t;
    if (ray_triangle_intersect(ray, v0, v1, v2, t) && t > tmin &&
        t < closest_so_far) {
      closest_so_far = t;
      is_hit = true;
    }
  }
  auto res = is_hit ? std::make_optional(closest_so_far) : std::nullopt;
  return res;
}

std::optional<double> Scene::hit_p(const Ray &ray, double tmin,
                                   double tmax) const {
  bool is_hit = false;
  double closest_so_far = tmax;
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

glm::dvec3 Mesh::sample_point(const glm::dvec3 &target, RNG &rng) const {
  auto index = (int)glm::floor(rng.next_1f() * (double)indices.size() / 3.0f);
  const auto &v0 = positions[indices[index * 3]];
  const auto &v1 = positions[indices[index * 3 + 1]];
  const auto &v2 = positions[indices[index * 3 + 2]];
  auto v = rng.next_1f();
  auto u = rng.next_1f();
  if (u + v > 1.0) {
    u = 1.0 - u;
    v = 1.0 - v;
  }
  return v0 + (v1 - v0) * u + (v2 - v0) * v;
}

double Mesh::pdf(const glm::dvec3 &origin, const glm::dvec3 &dir) const {
  Ray ray{.origin = origin, .dir = dir};
  auto h = hit(ray, 0.0001, std::numeric_limits<double>::max());
  if (h.has_value()) {
    auto &rec = h.value();
    auto distance_squard = rec.t * rec.t;
    auto cosine = glm::max(glm::dot(-ray.dir, rec.normal), 0.0);
    if (cosine < 0.0001) {
      return 0.0;
    }
    return distance_squard / (cosine * area());
  }
  return 0.0;
}

std::optional<HitRecord> Scene::hit(const Ray &ray, double tmin,
                                    double tmax) const {
  std::optional<HitRecord> rec{std::nullopt};
  double closest_so_far = tmax;
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

Camera::Camera(const glm::dvec3 &eye, const glm::dvec3 &target,
               const glm::dvec3 &up, double fov, double aspect) {
  this->fov = fov;
  this->aspect = aspect;
  this->transform = Transform::from_imodel(glm::lookAt(eye, target, up));
}
