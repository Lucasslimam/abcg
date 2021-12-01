#ifndef BALL_HPP_
#define BALL_HPP_

#include "abcg.hpp"
#include "vertex.hpp"
#include <vector>
#include <random>

class OpenGLWindow;
struct Vertex;


class Ball {
 public:
  virtual ~Ball() = default;

  void generateSphere();
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();
  void loadModelFromFile(std::string_view path);
  void update(float deltaTime);
  float getRadius() const;
  void changeColor();

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_viewMatrixLoc{};
  GLint m_projMatrixLoc{};
  GLint m_modelMatrixLoc{};
  GLint m_normalMatrixLoc{};
  GLint m_colorLoc{};
  GLint IaLoc, IdLoc, IsLoc;
  GLint KaLoc, KdLoc, KsLoc;
  GLint shininessLoc, lightDirLoc;

  glm::mat4 m_modelMatrix = glm::mat4(1.0);
  
  float m_radius{0.25};

  int m_stackCount{16};
  int m_sectorCount{16};

  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_ebo{};

  glm::vec3 m_scale{1.0f};
  glm::vec3 m_position{glm::vec3(0)};
  glm::vec3 m_velocity{glm::vec3(1.0f, 0.7f, 0.5f)};
  glm::vec3 m_color{glm::vec3(1.0f)};

  
  abcg::ElapsedTimer m_trailBlinkTimer;
  std::default_random_engine m_randomEngine;

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  glm::vec4 m_lightDir{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 m_Id{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 m_Is{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 m_Ka{0.1f, 0.1f, 0.1f, 1.0f};
  glm::vec4 m_Kd{0.7f, 0.7f, 0.7f, 1.0f};
  glm::vec4 m_Ks{1.0f, 1.0f, 1.0f, 1.0f};
  float m_shininess{25.0f};


};
#endif