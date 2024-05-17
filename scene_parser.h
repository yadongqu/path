#pragma once
#include "scene_data.h"
#include <array>
namespace Mitsuba {
enum FileFormat {
  OPENEXR,
};

enum PixelFormat {
  RGB,
};

enum FilterType {
  TENT,
};

enum SamplerType { INDEPENDENT };

enum BSDFType { DIFFUSE };

enum ShapeType {
  RECTANGLE,
  CUBE,
};

enum EmitterType { AREA };

enum CameraType {
  PERSPECTIVE,
};

enum IntegratorType {
  PATH,
};

struct Integrator {
  IntegratorType type;
  int max_depth;
};

struct Film {
  int width;
  int height;
  PixelFormat pixel_format;
  FileFormat file_format;
  FilterType filter_type;
};

struct Sampler {
  SamplerType type;
  int spp;
};

struct Camera {
  CameraType type;
  float fov;
  std::array<float, 16> transform;
  Sampler sampler;
  Film film;
};
struct BSDF {
  BSDFType type;
  bool two_sided;
  std::array<float, 3> reflect;
};

struct Emitter {
  EmitterType type;
  std::array<float, 3> radiance;
};

struct Shape {
  ShapeType type;
  std::array<float, 16> transform;
  BSDF bsdf;
};

struct Scene {
    Integrator integrator;
    Camera camera;
    std::vector<Shape> shapes;
};

bool load_scene(const char *path);
} // namespace Mitsuba
