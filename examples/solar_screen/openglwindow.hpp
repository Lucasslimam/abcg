#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <vector>

#include "abcg.hpp"
#include "camera.hpp"
#include "ground.hpp"
#include "ball.hpp"
#include "box.hpp"
#include "vertex.hpp"


class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;
  void checkCollision();
 private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  Camera m_camera;

  Ground m_ground;
  Box m_box;
  Ball m_ball;

  
  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  void loadModelFromFile(std::string_view path);
  void update();
};

#endif