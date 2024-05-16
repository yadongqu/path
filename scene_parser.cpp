#include "scene_parser.h"
#include <array>
#include <cassert>
#include <cstring>

#include <glm/gtc/type_ptr.hpp>
#include <optional>
#include <pugixml.hpp>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
namespace Mitsuba {

static std::unordered_map<std::string, Sampler> samplers;
static std::unordered_map<std::string, int> integers;
static std::unordered_map<std::string, Integrator> integrators;
static std::unordered_map<std::string, BSDF> bsdfs;
std::vector<std::string> tokenize(const std::string &string,
                                  const std::string &delim = ", ",
                                  bool includeEmpty = false) {
  std::string::size_type lastPos = 0,
                         pos = string.find_first_of(delim, lastPos);
  std::vector<std::string> tokens;

  while (lastPos != std::string::npos) {
    if (pos != lastPos || includeEmpty)
      tokens.push_back(string.substr(lastPos, pos - lastPos));
    lastPos = pos;
    if (lastPos != std::string::npos) {
      lastPos += 1;
      pos = string.find_first_of(delim, lastPos);
    }
  }

  return tokens;
}

float to_float(const std::string &str) {
  char *end_ptr = nullptr;
  float result = (float)strtof(str.c_str(), &end_ptr);
  if (*end_ptr != '\0')
    printf("something is wrong\n");
  // throw std::runtime_error("wrong format");
  return result;
}

float parse_float(pugi::xml_node &node) {
  assert(strcmp(node.name(), "float") == 0);
  return node.attribute("value").as_float();
}

std::optional<std::string> parse_reference_key(pugi::xml_node &node) {
  auto value = std::string(node.attribute("value").as_string());
  if (value.length() > 0 && value.at(0) == '$') {
    auto key = value.substr(1);
    return key;
  }
  return std::nullopt;
}

int parse_integer(pugi::xml_node &node) {
  assert(strcmp(node.name(), "integer") == 0);
  auto value = std::string(node.attribute("value").as_string());
  auto res = parse_reference_key(node);
  if (res.has_value()) {
    auto key = res.value();
    if (integers.contains(key)) {
      return integers.at(key);
    }
    throw std::runtime_error("reference " + key + " is not present");
  }
  return node.attribute("value").as_int();
}

std::array<float, 16> parse_transform(pugi::xml_node &node) {
  assert(strcmp(node.name(), "transform") == 0);
  auto matrix_node = node.child("matrix");
  auto value = matrix_node.attribute("value").value();
  std::array<float, 16> values;
  auto tokens = tokenize(value);

  if (tokens.size() != 16) {
    printf("wrong size of tokens\n");
  }
  for (int i = 0; i < 16; i++) {
    values[i] = to_float(tokens[i]);
  }
  return values;
}

Sampler parse_sampler(pugi::xml_node &node) {
  assert(strcmp(node.name(), "sampler") == 0);
  auto type = node.attribute("type").value();
  Sampler sampler;
  if (strcmp(type, "independent") == 0) {
    sampler.type = IDEPENDENT;
  }
  auto sample_count_node =
      node.find_child_by_attribute("integer", "sample_count");
  sampler.spp = parse_integer(sample_count_node);
}

FileFormat parse_file_format(pugi::xml_node &node) {
  assert(strcmp(node.name(), "string") == 0);
  auto value = node.attribute("value").as_string();
  if (strcmp(value, "openexr") == 0) {
    return OPENEXR;
  }
  throw std::runtime_error("unsupported file format");
}

PixelFormat parse_pixel_format(pugi::xml_node &node) {
  assert(strcmp(node.name(), "string") == 0);
  auto value = node.attribute("value").as_string();
  if (strcmp(value, "rgb") == 0) {
    return RGB;
  }
  throw std::runtime_error("unsupported pixel format");
}

FilterType parse_filter_type(pugi::xml_node &node) {
  auto type = node.attribute("type").value();

  if (strcmp(type, "tent") == 0) {
    return TENT;
  }
  throw std::runtime_error("unsupported filter type");
}

Film parse_film(pugi::xml_node &node) {
  assert(strcmp(node.name(), "film") == 0);
  Film film;
  for (auto child = node.first_child(); child; child = child.next_sibling()) {
    if (strcmp(child.attribute("name").value(), "width") == 0) {
      film.width = parse_integer(child);
    } else if (strcmp(child.attribute("name").value(), "height") == 0) {
      film.height = parse_integer(child);
    } else if (strcmp(child.attribute("name").value(), "file_format") == 0) {
      film.file_format = parse_file_format(child);
    } else if (strcmp(child.attribute("name").value(), "pixel_format") == 0) {
      film.pixel_format = parse_pixel_format(child);
    } else if (strcmp(child.name(), "rfilter") == 0) {
      film.filter_type = parse_filter_type(child);
    }
  }
  return film;
}

Integrator parse_integrator(pugi::xml_node &node) {
  assert(strcmp(node.name(), "integrator") == 0);
  auto res = parse_reference_key(node);
  if (res.has_value()) {
    auto key = res.value();
    if (integrators.contains(key)) {
      auto integrator = integrators.at(key);
      auto depth_node = node.find_child_by_attribute("name", "max_depth");
      integrator.max_depth = parse_integer(depth_node);
      return integrators.at(key);
    }
    throw std::runtime_error("Integrator " + key + " is not presented");
  } else {
    throw std::runtime_error("unsupported format for integrator");
  }
}

Camera parse_camera(pugi::xml_node &node) {
  assert(strcmp(node.name(), "sensor") == 0);
  auto type = node.attribute("type").value();
  Camera camera;
  if (strcmp(type, "perspective") == 0) {
    camera.type = PERSPECTIVE;
    for (auto child = node.first_child(); child; child = child.next_sibling()) {
      if (strcmp(child.attribute("type").value(), "fov") == 0) {
        camera.fov = parse_float(child);
      } else if (strcmp(child.name(), "transform") == 0) {
        auto t = child.attribute("name").value();
        if (t && strcmp(t, "to_world") == 0) {
          camera.transform = parse_transform(child);
        }
      } else if (strcmp(child.name(), "sampler") == 0) {
      }
    }
  }
}

std::array<float, 3> parse_rgb(pugi::xml_node &node) {
  assert(strcmp(node.name(), "rgb") == 0);
  auto value = node.attribute("value").value();
  auto tokens = tokenize(value);
  std::array<float, 3> values;
  for (int i = 0; i < 3; i++) {
    values[i] = to_float(tokens[i]);
  }
  return values;
}

BSDF parse_bsdf(pugi::xml_node &node) {
  assert(strcmp(node.name(), "bsdf") == 0);
  BSDF bsdf;
  auto id = node.attribute("id").value();
  auto type = node.attribute("type").value();
  if (strcmp(type, "twosided") == 0) {
    bsdf.two_sided = true;
  }
  auto real_bsdf_node = node.first_child();
  auto real_type = real_bsdf_node.attribute("type").value();
  if (strcmp(real_type, "diffuse") == 0) {
    bsdf.type = DIFFUSE;
  }
  auto rgb_node = real_bsdf_node.first_child();
  if (strcmp(rgb_node.attribute("name").value(), "reflectance") == 0) {
    auto rgb = parse_rgb(rgb_node);
    bsdf.reflect = rgb;
  }
  bsdfs.insert(std::make_pair(id, bsdf));
  return bsdf;
}

} // namespace Mitsuba

glm::dvec3 to_vector3(const std::string &str) {}

bool parse_camera(pugi::xml_node &node, Camera &camera) {
  if (strcmp(node.attribute("type").value(), "perspective") == 0) {
    for (auto child = node.first_child(); child; child = child.next_sibling()) {
      if (strcmp(child.attribute("name").value(), "fov") == 0) {
        camera.fov = child.attribute("value").as_float();
      } else if (strcmp(child.name(), "transform") == 0) {
        float values[16]{};
        auto tokens =
            tokenize(child.child("matrix").attribute("value").value());

        if (tokens.size() != 16) {
          printf("wrong tokens\n");
        }
        for (int i = 0; i < 16; i++) {
          values[i] = to_float(tokens[i]);
        }

        glm::mat4 model;
        memcpy(glm::value_ptr(model), values, sizeof(values));
        camera.transform = Transform::from_model(model);
      }

      else if (strcmp(child.name(), "sampler") == 0) {
        // ignore for now
      } else if (strcmp(child.name(), "film") == 0) {
        // ignore fow now
      } else {
        printf("unknown %s\n", child.name());
      }
    }
  } else {
    // printf("unsupported camera type\n");
  }
}

bool load_scene(const std::string &path, Scene &scene) {
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(path.c_str());
  if (!result) {
    printf("failed to load scene %s\n", path.c_str());
  }
  pugi::xml_node scene_node = doc.child("scene");
  Integrator integrator;
  for (auto child = scene_node.first_child(); child;
       child = child.next_sibling()) {
    if (strcmp(child.name(), "default") == 0) {
      auto name = child.attribute("name").value();
      if (strcmp(name, "integrator") == 0) {
        if (strcmp(child.attribute("value").value(), "path")) {
          scene.integrator = Integrator::make_path();
        }
      } else if (strcmp(name, "spp") == 0) {
        scene.samples = child.attribute("value").as_int();
      } else if (strcmp(name, "resx") == 0) {
        scene.width = child.attribute("value").as_int();
      } else if (strcmp(name, "resy") == 0) {
        scene.height = child.attribute("value").as_int();
      } else if (strcmp(name, "max_depth") == 0) {
        scene.bounces = child.attribute("value").as_int();
      }
    } else if (strcmp(child.name(), "sensor") == 0) {
      parse_camera(child, scene.camera);
    }
  }
  return true;
}