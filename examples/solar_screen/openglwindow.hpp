#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <vector>

#include "abcg.hpp"
#include "camera.hpp"
#include "astro.hpp"
#include "cube.hpp"
#include "vertex.hpp"


class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;
  void handleEvent(SDL_Event& ev) override;

 private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_program{};
  GLuint m_astro_program{};
  GLuint m_cube_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  Camera m_camera;
  Astro m_earth;
  Astro m_sun;
  Astro m_moon;
  Cube m_cube;

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  float m_dollySpeed{0.0f};
  float m_truckSpeed{0.0f};
  float m_panSpeed{0.0f};


  void loadModelFromFile(std::string_view path);
  void update(float deltaTime);
};

#endif