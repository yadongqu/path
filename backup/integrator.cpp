#include "integrator.h"

#include <ranges>

namespace flow {
vec3f NormalIntegrator::li(const Ray &ray, const Scene &scene, RNG &rng) const {
  auto res = scene.hit(ray, 0.001, std::numeric_limits<double>::max());
  if (!res.has_value()) {
    return vec3f(0.0);
  }
  auto rec = res.value();
  return rec.normal;
}

vec3f PathIntegrator::trace_path(const Ray &ray, const Scene &scene, RNG &rng,
                                 int16_t bounces) const {
  if (bounces <= 0) {
    return vec3f(0.0);
  }
  auto res = scene.hit(ray, 0.001, std::numeric_limits<double>::max());
  if (!res.has_value()) {
    return vec3f(0.0);
  }
  // printf("hit\n");
  auto rec = res.value();

  return rec.normal;
  // return rec.normal;
  auto mesh = rec.mesh;
  auto material = mesh->material;

  auto emitted = material.emit();

  if (res->is_inside) {
    return emitted;
  }

  auto scatter = material.sample(-ray.dir, rec.normal, rng);
  if (!scatter.has_value()) {
    return emitted;
  }

  auto scatter_record = scatter.value();
  double prob = 0.5;
  vec3f next_dir = scatter_record.dir;
  // printf("%f\n", rng.next_1f());
  if (rng.next_1f() < prob) {
    auto is_light = [](const Mesh &m) { return m.material.is_light(); };
    auto filtered = scene.meshes | std::views::filter(is_light);
    auto mesh = filtered.base().data();
    vec3f p = mesh->sample_point(rec.position, rng);
    next_dir = glm::normalize(p - rec.position);
  }

  auto pdf = 0.5 * material.pdf(-ray.dir, rec.normal, next_dir) +
             0.5 * mesh->pdf(rec.position, next_dir);

  if (pdf < 0.0001) {
    return emitted;
  }

  auto next_ray =
      Ray{.origin = rec.position + rec.normal * 0.0001, .dir = next_dir};
  auto scatter_pdf = material.pdf(-ray.dir, rec.normal, next_dir);
  // printf("scatter pdf %f\n", scatter_pdf);
  auto scattered_color = scatter_record.attenuation * scatter_pdf *
                         trace_path(next_ray, scene, rng, bounces - 1) / pdf;

  return emitted + scattered_color;
}

vec3f PathIntegrator::li(const Ray &ray, const Scene &scene, RNG &rng) const {
  return trace_path(ray, scene, rng, scene.bounces);
}

vec3f Integrator::li(const Ray &ray, const Scene &scene, RNG &rng) const {

  return std::visit([&](auto &&i) { return i.li(ray, scene, rng); },
                    integrator);
}
} // namespace flow
