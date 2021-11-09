#ifndef GROUND_HPP_
#define GROUND_HPP_

#include "abcg.hpp"

class Ground {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();
  void update();
  void setColorTileOne(glm::vec3 color);
  void setColorTileTwo(glm::vec3 color);

 private:
  GLuint m_VAO{};
  GLuint m_VBO{};

  GLint m_modelMatrixLoc{};
  GLint m_colorLoc{};
  GLuint m_program{};

  glm::vec3 m_colorTileOne{glm::vec3(1.0f)};
  glm::vec3 m_colorTileTwo{glm::vec3(0.5f)};

};

#endif