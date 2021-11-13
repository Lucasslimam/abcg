#ifndef BOX_HPP_
#define BOX_HPP_

#include "abcg.hpp"
#include "vertex.hpp"
#include <vector>


class OpenGLWindow;
struct Vertex;


class Box {
 public:
  virtual ~Box() = default;

  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();
  void loadModelFromFile(std::string_view path);
  float getSideLength() const;

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_viewMatrixLoc{};
  GLint m_projMatrixLoc{};
  GLint m_modelMatrixLoc{};
  GLint m_colorLoc{};
  
  float m_angle{90.0f};
  float m_scale{2.0f}; //2.0 para box
  glm::mat4 m_modelMatrix{};
  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_ebo{};

  abcg::ElapsedTimer m_trailBlinkTimer;
  glm::vec3 m_color{glm::vec3(1.0f)};

  glm::vec3 m_position{glm::vec3()};
  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;
};
#endif