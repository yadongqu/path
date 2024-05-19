#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <pbrt.h>
TEST_CASE("test tokenizer") {
  std::ifstream infile("../../../assets/cornell-box/scene-v4.pbrt");
  std::stringstream buffer;
  buffer << infile.rdbuf();
  auto content = buffer.str();

  Tokenizer tokenizer(content.c_str());
  auto tokens = tokenizer.tokenize();
  for (auto &token : tokens) {
    std::cout << token << std::endl;
  }
}

TEST_CASE("test parser") {
  std::ifstream infile("../../../assets/cornell-box/scene-v4.pbrt");
  std::stringstream buffer;
  buffer << infile.rdbuf();
  auto content = buffer.str();

  Tokenizer tokenizer(content.c_str());
  auto tokens = tokenizer.tokenize();

  Parser parser(tokens);
  parser.parse();
}
