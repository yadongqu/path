#include <functional>
#include <iostream>
#include <string>
#include <vector>

enum class Token {
  Comment,
  Number,
  StringLiteral,
  ListStart,
  ListEnd,
  Identifier,
  NewLine,
  WhiteSpace,
  Indent,
  Dedent
};

struct Tokenizer {
  const char *source;
  size_t position;
  size_t row;
  size_t column;

  Tokenizer(const char *source)
      : source(source), position(0), row(0), column(0) {}

  std::vector<Token> tokenize() {
    std::vector<Token> tokens;
    size_t indent_level = 0;
    size_t indent_length = 4;
    bool in_array = false;
    while (position < strlen(source)) {
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
            tokens.push_back(Token::Dedent);
          }
          indent_level -= n;
        } else if (count > indent_level) {
          size_t n = count - indent_level;
          for (size_t i = 0; i < n; ++i) {
            tokens.push_back(Token::Indent);
          }
          indent_level += n;
        }
      }

      if (b == '\n') {
        if (!in_array) {
          tokens.push_back(Token::NewLine);
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
        tokens.push_back(Token::Identifier);
        position += count;
        column += count;
      } else if (b == '[') {
        tokens.push_back(Token::ListStart);
        position += 1;
        column += 1;
        in_array = true;
      } else if (b == ']') {
        tokens.push_back(Token::ListEnd);
        position += 1;
        column += 1;
        in_array = false;
      } else if (b == '"') {
        size_t count = peek_while([&](char ch) { return ch != '"'; }, 1);
        tokens.push_back(Token::StringLiteral);
        position += count + 1;
        column += count + 1;
      } else if (b == '-' || isdigit(b)) {
        size_t count = peek_while(
            [&](char ch) {
              return isdigit(ch) || ch == '.' || ch == '-' || ch == 'e';
            },
            1);
        tokens.push_back(Token::Number);
        position += count;
        column += count;
      } else {
        std::cerr << "Unexpected character: " << position << " " << b
                  << std::endl;
        exit(1);
      }
    }
    while (indent_level > 0) {
      tokens.push_back(Token::Dedent);
      indent_level -= 1;
    }
    return tokens;
  }

  size_t peek_while(std::function<bool(char)> p, size_t offset) {
    size_t consumed = offset;

    while (position + consumed < strlen(source) &&
           p(source[position + consumed])) {
      consumed += 1;
    }

    return consumed;
  }
};

struct Parser {
  const std::vector<Token> &tokens;
  size_t position;

  Parser(const std::vector<Token> &tokens) : tokens(tokens), position(0) {}

  // Implement parse method here
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
  std::string kind;
  size_t samples;

  SamplerData() : kind("independent"), samples(1) {}
};

struct FilmData {
  std::string kind;
  size_t x_resolution;
  size_t y_resolution;
  std::string filename;

  FilmData()
      : kind("hdr"), x_resolution(512), y_resolution(512),
        filename("image.exr") {}
};

struct PixelFilterData {
  std::string kind;
  float x_radius;
  float y_radius;

  PixelFilterData() : kind("triangle"), x_radius(1.0f), y_radius(1.0f) {}
};

struct SceneData {
  IntegratorData integrator;
  CameraData camera;
  SamplerData sampler;
  FilmData film;
  PixelFilterData pixel_filter;

  SceneData() : integrator(), camera(), sampler(), film(), pixel_filter() {}
};

int main() {
  const char *source = "your source code here";
  Tokenizer tokenizer(source);
  std::vector<Token> tokens = tokenizer.tokenize();
  Parser parser(tokens);
  // Implement parse method here
  return 0;
}