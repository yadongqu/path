#include <array>
#include <cassert>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

enum class TokenType {
  Number = 0,
  StringLiteral,
  ListStart,
  ListEnd,
  Identifier,
  NewLine,
  Indent,
  Dedent,
  Undefined,
};

static std::string token_names[9] = {"Number",  "StringLiteral", "ListStart",
                                     "ListEnd", "Identifier",    "NewLine",
                                     "Indent",  "Dedent",        "Undefined"};

struct Token {
  TokenType type;
  std::string_view value;
};

inline std::ostream &operator<<(std::ostream &stream, const Token &token) {
  stream << "type: " << token_names[(int)token.type]
         << " value: " << token.value;
  return stream;
}

struct Tokenizer {
  std::string_view source;
  size_t position;
  size_t row;
  size_t column;

  explicit Tokenizer(std::string_view source)
      : source(source), position(0), row(0), column(0) {}

  std::vector<Token> tokenize() {
    std::vector<Token> tokens;
    size_t indent_level = 0;
    size_t indent_length = 4;
    bool in_array = false;
    bool new_line = false;
    while (position < source.length()) {
      char b = source[position];
      if (column == 0 && !in_array) {
        size_t white_space_count =
            peek_while([&](char ch) { return ch == ' '; }, 0);
        position += white_space_count;
        column += white_space_count;
        size_t count = white_space_count / indent_length;
        if (count < indent_level) {
          size_t n = indent_level - count;
          for (size_t i = 0; i < n; ++i) {
            tokens.push_back(Token{.type = TokenType::Dedent});
          }
          indent_level -= n;
        } else if (count > indent_level) {
          size_t n = count - indent_level;
          for (size_t i = 0; i < n; ++i) {
            tokens.push_back(Token{.type = TokenType::Indent});
          }
          indent_level += n;
        }
      }

      if (b == '\n') {
        if (!in_array) {
          tokens.push_back(Token{
              .type = TokenType::NewLine,
          });
        }
        position += 1;
        column = 0;
        row += 1;
      } else if (b == ' ') {
        size_t count = peek_while([&](char ch) { return ch == ' '; }, 0);
        position += count;
        column += count;
      } else if (isalpha(b)) {
        size_t count = peek_while([&](char ch) { return isalnum(ch); }, 1);
        tokens.push_back(Token{.type = TokenType::Identifier,
                               .value = source.substr(position, count)});
        position += count;
        column += count;
      } else if (b == '[') {
        tokens.push_back(Token{.type = TokenType::ListStart});
        position += 1;
        column += 1;
        in_array = true;
      } else if (b == ']') {
        tokens.push_back(Token{.type = TokenType::ListEnd});
        position += 1;
        column += 1;
        in_array = false;
      } else if (b == '"') {
        size_t count = peek_while([&](char ch) { return ch != '"'; }, 1);
        tokens.push_back(
            Token{.type = TokenType::StringLiteral,
                  .value = source.substr(position + 1, count - 1)});
        position += count + 1;
        column += count + 1;
      } else if (b == '-' || isdigit(b)) {
        size_t count = peek_while(
            [&](char ch) {
              return isdigit(ch) || ch == '.' || ch == '-' || ch == 'e';
            },
            1);
        tokens.push_back(Token{.type = TokenType::Number,
                               .value = source.substr(position, count)});
        position += count;
        column += count;
      } else {
        std::cerr << "Unexpected character: " << position << " " << b
                  << std::endl;
        exit(1);
      }
    }
    while (indent_level > 0) {
      tokens.push_back(Token{.type = TokenType::Dedent});
      indent_level -= 1;
    }
    return tokens;
  }

  size_t peek_while(const std::function<bool(char)> &p, size_t offset) const {
    size_t consumed = offset;
    while (position + consumed < source.length() &&
           p(source[position + consumed])) {
      consumed += 1;
    }

    return consumed;
  }
};

struct IntegratorData {
  std::string kind;
  size_t max_depth;

  IntegratorData() : kind("path"), max_depth(1) {}
};

struct CameraData {
  std::string kind;
  float fov;

  CameraData() : kind("perspective"), fov(90.0f) {}
};

struct SamplerData {
  std::string_view kind;
  size_t samples;

  SamplerData() : kind("independent"), samples(1) {}
};

struct FilmData {
  std::string kind;
  size_t x_resolution;
  size_t y_resolution;
  std::string_view filename;

  FilmData()
      : kind("hdr"), x_resolution(512), y_resolution(512),
        filename("image.exr") {}
};

struct PixelFilterData {
  std::string_view kind;
  float x_radius;
  float y_radius;

  PixelFilterData() : kind("triangle"), x_radius(1.0f), y_radius(1.0f) {}
};

struct DiffuseMaterialData {
  std::array<float, 3> reflectance;
};

struct MaterialData {

  static MaterialData make_diffuse(std::array<float, 3> r) {
    return MaterialData{.data = DiffuseMaterialData{.reflectance = r}};
  }

  std::variant<DiffuseMaterialData> data;
};

struct TriangleMeshShapeData {
  std::vector<float> uvs;
  std::vector<float> normals;
  std::vector<float> positions;
  std::vector<int> indices;
};

struct LightData {
  std::array<float, 3> l;
  std::string_view kind;
};

struct ShapeData {

  static ShapeData make_triangle_mesh() {
    return ShapeData{.data = TriangleMeshShapeData{}};
  }

  std::optional<LightData> light;
  std::string_view material;
  std::variant<TriangleMeshShapeData> data;
};

struct SceneData {
  IntegratorData integrator;
  CameraData camera;
  SamplerData sampler;
  FilmData film;
  PixelFilterData pixel_filter;
  std::array<float, 16> transform;
  std::unordered_map<std::string_view, MaterialData> materials;
  std::vector<ShapeData> shapes;
  SceneData() : integrator(), camera(), sampler(), film(), pixel_filter() {}
};

struct Parser {
  const std::vector<Token> &tokens;
  size_t position;

  Parser(const std::vector<Token> &tokens) : tokens(tokens), position(0) {}

  SceneData parse() {
    SceneData scene_data;
    while (position < tokens.size()) {
      const auto &token = tokens.at(position);
      switch (token.type) {
      case TokenType::Identifier: {
        if (token.value == "Integrator") {
          scene_data.integrator = parse_integrator();
        } else if (token.value == "Transform") {
          scene_data.transform = parse_transform();
        } else if (token.value == "Sampler") {
          scene_data.sampler = parse_sampler();
        } else if (token.value == "PixelFilter") {
          scene_data.pixel_filter = parse_pixel_filter();
        } else if (token.value == "Film") {
          scene_data.film = parse_film();
        } else if (token.value == "WorldBegin") {
          parse_world(scene_data);
        } else if (token.value == "AttributeBegin") {
          parse_attribute(scene_data);
        } else if (token.value == "Camera") {
          scene_data.camera = parse_camera();
        }
        break;
      }

      case TokenType::NewLine:
      case TokenType::Dedent:
        position += 1;
        break;

      default:
        std::cout << token << std::endl;
        std::cout << "not implemented" << std::endl;
        exit(1);
      }
    }
  }

  void parse_world(SceneData &scene_data) {
    position += 1;
    position += 1;
    while (tokens.at(position).type == TokenType::Identifier &&
           tokens.at(position).value == "MakeNamedMaterial") {
      scene_data.materials.insert(parse_material());
    }

    while (tokens.at(position).type == TokenType::Identifier &&
           tokens.at(position).value == "NamedMaterial") {
      scene_data.shapes.push_back(parse_shape());
    }
  }

  void parse_attribute(SceneData &scene_data) {
    position += 1;
    position += 1;
    ShapeData shape;
    if (tokens.at(position).type == TokenType::Indent) {
      position += 1;
      while (tokens.at(position).type != TokenType::Dedent) {
        if (tokens.at(position).value == "AreaLightSource") {
          shape.light = parse_light();
        } else if (tokens.at(position).value == "NamedMaterial") {
          auto res = parse_shape();
          shape.material = res.material;
          shape.data = res.data;
        }
      }
      position += 1;
    }
    position += 1;
  }

  LightData parse_light() {
    position += 1;
    LightData light;
    light.kind = tokens.at(position).value;
    position += 1;
    if (tokens.at(position).type == TokenType::Indent) {
      position += 1;
      while (tokens.at(position).type != TokenType::Dedent) {
        auto [type, key] = parse_type_and_key();
        if (type == "rgb" && key == "L") {
          light.l = parse_values<float, 3>();
        } else {
          std::cerr << "unknown type and key for light " << key << ": " << type
                    << std::endl;
        }
        position += 1;
      }
      position += 1;
    }
    return light;
  }

  ShapeData parse_shape() {
    ShapeData shape_data;
    position += 1;

    auto material_key = tokens.at(position).value;
    shape_data.material = material_key;
    position += 1;
    if (tokens.at(position).value == "Shape") {
      position += 1;
      auto shape_type = tokens.at(position).value;
      if (shape_type == "trianglemesh") {
        TriangleMeshShapeData shape;
        position += 1;
        position += 1;
        if (tokens.at(position).type == TokenType::Indent) {
          position += 1;
          while (tokens.at(position).type != TokenType::Dedent) {
            auto [type, key] = parse_type_and_key();
            if (type == "point2" && key == "uv") {
              shape.uvs = parse_unknown_values<float>();
            } else if (type == "normal" && key == "N") {
              shape.normals = parse_unknown_values<float>();
            } else if (type == "point3" && key == "P") {
              shape.positions = parse_unknown_values<float>();
            } else if (type == "integer" && key == "indices") {
              shape.indices = parse_unknown_values<int>();
            } else {
              std::cerr << "unknown key " << key << "  and type: " << type
                        << std::endl;
            }
            position += 1;
          }
          position += 1;
        }
        shape_data.data = shape;
      } else {
        std::cerr << "unsupported shape type " << shape_type << std::endl;
      }
    }
    return shape_data;
  }

  std::pair<std::string_view, MaterialData> parse_material() {
    position += 1;
    auto key = tokens.at(position).value;
    position += 1;
    MaterialData material_data;
    if (tokens.at(position).type == TokenType::Indent) {
      position += 1;
      auto [type, key] = parse_type_and_key();
      position += 1;
      while (tokens.at(position).type != TokenType::Dedent) {
        auto [property_type, property_key] = parse_type_and_key();

        if (type == "string" && key == "diffuse") {
          if (property_key == "reflectance" && property_type == "rgb") {
            auto reflectance = parse_values<float, 3>();
            material_data = MaterialData::make_diffuse(reflectance);
          }
        } else {
          std::cerr << "unsupported material type " << key << std::endl;
        }
        position += 1;
      }
      position += 1;
    }
    return {key, material_data};
  }

  IntegratorData parse_integrator() {
    IntegratorData integrator_data;
    position += 1; // skip integrator identifier
    assert(tokens.at(position).type == TokenType::StringLiteral);
    integrator_data.kind = tokens.at(position).value;
    position += 1;
    position += 1; // skip new line
    if (tokens.at(position).type == TokenType::Indent) {
      position += 1; // skip indent;
      while (tokens.at(position).type != TokenType::Dedent) {
        auto [type, key] = parse_type_and_key();
        if (key == "maxdepth") {
          integrator_data.max_depth = parse_values<int, 1>()[0];
        } else {
          std::cerr << "unknown key " << key << std::endl;
        }
        position += 1; // skip new line
      }
      position += 1; // skip dedent;
    }
    return integrator_data;
  }

  std::array<float, 16> parse_transform() {
    position += 1;
    auto res = parse_values<float, 16>();
    position += 1; // skip new line
    return res;
  }

  std::array<std::string_view, 2> parse_type_and_key() {
    auto &type_and_key = tokens.at(position).value;
    auto split_index = type_and_key.find(" ");
    auto type = type_and_key.substr(0, split_index);
    auto key = type_and_key.substr(split_index + 1);
    position += 1;
    return {type, key};
  }

  CameraData parse_camera() {
    position += 1;
    CameraData camera;
    camera.kind = tokens.at(position).value;
    position += 1;
    position += 1;
    if (tokens.at(position).type == TokenType::Indent) {
      position += 1;
      auto [type, key] = parse_type_and_key();
      if (type == "float" && key == "fov") {
        camera.fov = parse_values<float, 1>()[0];
      }
      position += 1;
      position += 1;
      position += 1;
    }
    return camera;
  }

  SamplerData parse_sampler() {
    position += 1;
    SamplerData sampler_data;
    sampler_data.kind = tokens.at(position).value;
    position += 1;
    position += 1;
    if (tokens.at(position).type == TokenType::Indent) {
      position += 1;
      while (tokens.at(position).type != TokenType::Dedent) {
        auto [type, key] = parse_type_and_key();
        if (key == "pixelsamples") {
          sampler_data.samples = parse_values<int, 1>()[0];
        } else {
          std::cerr << "unknown key " << key << std::endl;
        }
        position += 1;
      }
      position += 1;
    }
    return sampler_data;
  }

  PixelFilterData parse_pixel_filter() {
    position += 1;
    PixelFilterData pixel_filter_data;
    pixel_filter_data.kind = tokens[position].value;
    position += 1;
    position += 1;
    if (tokens.at(position).type == TokenType::Indent) {
      position += 1;
      while (tokens.at(position).type != TokenType::Dedent) {
        auto [type, key] = parse_type_and_key();
        if (key == "xradius") {
          if (type == "float") {
            pixel_filter_data.x_radius = parse_values<float, 1>()[0];
          }
        } else if (key == "yradius") {
          if (type == "float") {
            pixel_filter_data.y_radius = parse_values<float, 1>()[0];
          }
        }
        position += 1;
      }
      position += 1;
    }
    return pixel_filter_data;
  }

  FilmData parse_film() {
    position += 1;
    FilmData film_data;
    film_data.kind = tokens.at(position).value;
    position += 1;
    position += 1;
    if (tokens.at(position).type == TokenType::Indent) {
      position += 1;
      while (tokens.at(position).type != TokenType::Dedent) {
        auto [type, key] = parse_type_and_key();
        if (key == "filename" && type == "string") {
          film_data.filename = parse_string_values<1>()[0];
        } else if (type == "integer") {
          if (key == "xresolution") {
            film_data.x_resolution = parse_values<int, 1>()[0];
          } else if (key == "yresolution") {
            film_data.y_resolution = parse_values<int, 1>()[0];
          } else {
            std::cerr << "unknown key " << key << std::endl;
          }
        } else {

          std::cerr << "unknown property " << key << " : " << type << std::endl;
        }
        position += 1;
      }
      position += 1;
    }
    return film_data;
  }

  template <int n> std::array<std::string_view, n> parse_string_values() {
    std::array<std::string_view, n> res;
    position += 1;
    for (int i = 0; i < n; i++) {
      res[i] = tokens.at(position).value;
      position += 1;
    }
    position += 1;
    return res;
  }

  template <typename T> std::vector<T> parse_unknown_values() {
    std::vector<T> res;
    position += 1;
    while (tokens.at(position).type != TokenType::ListEnd) {
      if (std::is_same_v<T, int>) {
        std::string v = std::string(tokens.at(position).value);
        res.push_back(std::stoi(v, nullptr, 10));
      } else if (std::is_same_v<T, float>) {
        std::string v = std::string(tokens.at(position).value);
        res.push_back(std::stof(v));
      } else {
        std::cerr << "unsupported value types " << std::endl;
      }
      position += 1;
    }
    position += 1;
    return res;
  }

  template <typename T, int n> std::array<T, n> parse_values() {
    std::array<T, n> res;
    position += 1; // skip list start
    for (int i = 0; i < n; i++) {
      if (std::is_same_v<T, int>) {
        std::string v = std::string(tokens.at(position).value);
        res[i] = std::stoi(v, nullptr, 10);
      } else if (std::is_same_v<T, float>) {
        std::string v = std::string(tokens.at(position).value);
        res[i] = std::stof(v);
      } else {
        std::cerr << "unsupported value types " << std::endl;
      }
      position += 1;
    }

    position += 1; // skip list end;
    return res;
  }
};
