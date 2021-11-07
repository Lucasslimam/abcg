#ifndef BOX_HPP_
#define BOX_HPP_

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

class Box {
 public:
  virtual ~Box() = default;

  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();
  void loadModelFromFile(std::string_view path);
  void update(float deltaTime);

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_viewMatrixLoc{};
  GLint m_projMatrixLoc{};
  GLint m_modelMatrixLoc{};
  GLint m_colorLoc{};
  

  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_ebo{};

  abcg::ElapsedTimer m_trailBlinkTimer;

  glm::vec3 m_position{glm::vec3()};
  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;
};
#endif