#pragma once
#include "integrator.h"
#include "scene_data.h"
#include <cstdint>

Scene build_triangle_scene() {
  auto white = Material::make_lambertian(glm::vec3(1.0));
  auto triangle_mesh =
      Mesh{.positions = {glm::vec3(0.0, 1.0, 0.0), glm::vec3(-1.0, -1.0, 0.0),
                         glm::vec3(1.0, -1.0, 0.0)},
           .indices = {0, 1, 2},
           .material = white};
  uint16_t width = 512;
  uint16_t height = 512;
  double aspect = (double)width / (double)height;
  Camera camera(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0),
                glm::vec3(0.0, 1.0, 0.0), 45.0, aspect);
  Scene scene = {
      .meshes = {triangle_mesh},
      .camera = camera,
      .integrator = Integrator::make_normal(),
      .width = width,
      .height = height,
      .bounces = 5,
      .samples = 10,
  };

  return scene;
}

Scene build_cornell_scene() {
  auto white = Material::make_lambertian(glm::vec3(1.0));
  auto red = Material::make_lambertian(glm::vec3(1.0, 0.0, 0.0));
  auto green = Material::make_lambertian(glm::vec3(0.0, 1.0, 0.0));
  auto light_material = Material::make_diffuse_light(glm::vec3(1.0), 20.0);
  // auto metal = Material::(glm::vec3(0.8, 0.85, 0.88));

  auto floor = Mesh{.positions =
                        {
                            glm::vec3(552.8, 0.0, 0.0),
                            glm::vec3(0.0, 0.0, 0.0),
                            glm::vec3(0.0, 0.0, 559.2),
                            glm::vec3(549.6, 0.0, 559.2),
                        },
                    .indices = {0, 1, 2, 2, 3, 0},
                    .material = white};
  auto ceiling = Mesh{.positions =
                          {
                              glm::vec3(556.0, 548.8, 0.0),
                              glm::vec3(556.0, 548.8, 559.2),
                              glm::vec3(0.0, 548.8, 559.2),
                              glm::vec3(0.0, 548.8, 0.0),
                          },
                      .indices = {0, 1, 2, 2, 3, 0},
                      .material = white};

  auto light = Mesh{.positions =
                        {
                            glm::vec3(343.0, 548.7, 227.0),
                            glm::vec3(343.0, 548.7, 332.0),
                            glm::vec3(213.0, 548.7, 332.0),
                            glm::vec3(213.0, 548.7, 227.0),
                        },
                    .indices = {0, 1, 2, 2, 3, 0},
                    .material = light_material};

  auto back_wall = Mesh{.positions =
                            {
                                glm::vec3(549.6, 0.0, 559.2),
                                glm::vec3(0.0, 0.0, 559.2),
                                glm::vec3(0.0, 548.8, 559.2),
                                glm::vec3(556.0, 548.8, 559.2),
                            },
                        .indices = {0, 1, 2, 2, 3, 0},
                        .material = white};
  auto left_wall = Mesh{.positions =
                            {
                                glm::vec3(552.8, 0.0, 0.0),
                                glm::vec3(549.6, 0.0, 559.2),
                                glm::vec3(556.0, 548.8, 559.2),
                                glm::vec3(556.0, 548.8, 0.0),
                            },
                        .indices = {0, 1, 2, 2, 3, 0},
                        .material = red};

  auto right_wall = Mesh{.positions =
                             {
                                 glm::vec3(0.0, 0.0, 559.2),
                                 glm::vec3(0.0, 0.0, 0.0),
                                 glm::vec3(0.0, 548.8, 0.0),
                                 glm::vec3(0.0, 548.8, 559.2),
                             },
                         .indices = {0, 1, 2, 2, 3, 0},
                         .material = green};

  auto short_block = Mesh{
      .positions =
          {
              glm::vec3(130.0, 165.0, 65.0),  glm::vec3(82.0, 165.0, 225.0),
              glm::vec3(240.0, 165.0, 272.0), glm::vec3(290.0, 165.0, 114.0),
              glm::vec3(290.0, 0.0, 114.0),   glm::vec3(290.0, 165.0, 114.0),
              glm::vec3(240.0, 165.0, 272.0), glm::vec3(240.0, 0.0, 272.0),
              glm::vec3(130.0, 0.0, 65.0),    glm::vec3(130.0, 165.0, 65.0),
              glm::vec3(290.0, 165.0, 114.0), glm::vec3(290.0, 0.0, 114.0),
              glm::vec3(82.0, 0.0, 225.0),    glm::vec3(82.0, 165.0, 225.0),
              glm::vec3(130.0, 165.0, 65.0),  glm::vec3(130.0, 0.0, 65.0),
              glm::vec3(240.0, 0.0, 272.0),   glm::vec3(240.0, 165.0, 272.0),
              glm::vec3(82.0, 165.0, 225.0),  glm::vec3(82.0, 0.0, 225.0),
          },
      .indices =
          {
              0, 2, 3,  0,  1,  2,  7,  6,  4,  6,  5,  4,  8,  10, 11,
              8, 9, 10, 12, 14, 15, 12, 13, 14, 19, 18, 16, 18, 17, 16,
          },
      .material = white};

  auto tall_block = Mesh{
      .positions =
          {
              glm::vec3(423.0, 330.0, 247.0), glm::vec3(265.0, 330.0, 296.0),
              glm::vec3(314.0, 330.0, 456.0), glm::vec3(472.0, 330.0, 406.0),
              glm::vec3(423.0, 0.0, 247.0),   glm::vec3(423.0, 330.0, 247.0),
              glm::vec3(472.0, 330.0, 406.0), glm::vec3(472.0, 0.0, 406.0),
              glm::vec3(472.0, 0.0, 406.0),   glm::vec3(472.0, 330.0, 406.0),
              glm::vec3(314.0, 330.0, 456.0), glm::vec3(314.0, 0.0, 456.0),
              glm::vec3(314.0, 0.0, 456.0),   glm::vec3(314.0, 330.0, 456.0),
              glm::vec3(265.0, 330.0, 296.0), glm::vec3(265.0, 0.0, 296.0),
              glm::vec3(265.0, 0.0, 296.0),   glm::vec3(265.0, 330.0, 296.0),
              glm::vec3(423.0, 330.0, 247.0), glm::vec3(423.0, 0.0, 247.0),
          },
      .indices =
          {
              3,  2, 0, 2,  1,  0,  4,  6,  7,  4,  5,  6,  11, 10, 8,
              10, 9, 8, 15, 14, 12, 14, 13, 12, 16, 18, 19, 16, 17, 18,
          },
      .material = white};

  int16_t width = 512;
  int16_t height = 512;
  double aspect = (double)width / (double)height;
  auto camera =
      Camera(glm::vec3(278.0, 273.0, -800.0), glm::vec3(278.0, 273.0, 0.0),
             glm::vec3(0.0, 1.0, 0.0), 45.0, aspect);
  auto integrator = Integrator::make_path();
  Scene scene{
      .meshes = {floor, ceiling, back_wall, left_wall, right_wall, short_block,
                 tall_block},
      .camera = camera,
      .integrator = integrator,
      .width = 512,
      .height = 512,
      .bounces = 5,
      .samples = 10,
  };

  return scene;
}