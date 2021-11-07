#ifndef BALL_HPP_
#define BALL_HPP_

#include "abcg.hpp"
#include <vector>

class OpenGLWindow;
struct Vertex;

/*
struct Vertex {
  glm::vec3 position;

  bool operator==(const Vertex& other) const {
    return position == other.position;
  }
};*/

class Ball {
 public:
  virtual ~Ball() = default;

  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();
  void loadModelFromFile(std::string_view path);
  void update(float deltaTime);
  float getRadius() const;

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_viewMatrixLoc{};
  GLint m_projMatrixLoc{};
  GLint m_modelMatrixLoc{};
  GLint m_colorLoc{};

  glm::mat4 m_modelMatrix = glm::mat4(1.0);
  float m_scale{0.125};

  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_ebo{};
  GLuint m_vboColors{};

  glm::vec3 m_position{glm::vec3(0)};
  glm::vec3 m_velocity{glm::vec3(1.0f, 0.7f, 0.5f)};
  abcg::ElapsedTimer m_trailBlinkTimer;

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;
};
#endif