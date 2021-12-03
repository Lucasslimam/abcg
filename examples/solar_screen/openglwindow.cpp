#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <unordered_map>
#include <glm/gtc/matrix_inverse.hpp>

void OpenGLWindow::initializeGL() {
  abcg::glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program = createProgramFromFile(getAssetsPath() + "ball.vert",
                                    getAssetsPath() + "ball.frag");
                                  
  m_box_program = createProgramFromFile(getAssetsPath() + "lookat.vert",
                                  getAssetsPath() + "lookat.frag");

  m_ground.initializeGL(m_program);

  // Load model

  m_sun.m_velocity = glm::vec3(0.0f);
  glm::vec3 m_sun_position = glm::vec3(0.0f, 0.0f, 0.0f);
  float m_sun_radius = 0.5f;
  m_sun.generateSphere(m_sun_position, m_sun_radius);
  m_sun.m_Ka = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
  

  glm::vec3 m_ball_position = glm::vec3(3.5f, 0.0f, 0.0f);
  float m_ball_radius = 0.25f;
  m_ball.generateSphere(m_ball_position, m_ball_radius);
  m_ball.m_velocity = glm::vec3(0.0f);
  m_ball.m_parent = &m_sun;
  m_ball.m_angularVelocity = glm::vec3(0.0f, PI/2, 0.0f);
  

  m_moon.m_velocity = glm::vec3(0.0f);
  glm::vec3 m_moon_position = glm::vec3(0.5f, 0.0f, 0.0f);
  float m_moon_radius = 0.125f;
  m_moon.generateSphere(m_moon_position, m_moon_radius);
  m_moon.m_parent = &m_ball;
  m_moon.m_angularVelocity = glm::vec3(0.2f, 2*PI, 0.2f);

  loadModelFromFile(getAssetsPath() + "box.obj");
  m_box.m_vertices = std::vector<Vertex>(m_vertices);
  m_box.m_indices = std::vector<GLuint>(m_indices);

  m_ball.initializeGL(m_program);
  m_sun.initializeGL(m_program);
  m_moon.initializeGL(m_program);
  m_box.initializeGL(m_box_program);
  resizeGL(getWindowSettings().width, getWindowSettings().height);
}

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

void OpenGLWindow::loadModelFromFile(std::string_view path) {
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data())) {
    if (!reader.Error().empty()) {
      throw abcg::Exception{abcg::Exception::Runtime(
          fmt::format("Failed to load model {} ({})", path, reader.Error()))};
    }
    throw abcg::Exception{
        abcg::Exception::Runtime(fmt::format("Failed to load model {}", path))};
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  const auto& attrib{reader.GetAttrib()};
  const auto& shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (const auto& shape : shapes) {
    // Loop over indices
    for (const auto offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      const tinyobj::index_t index{shape.mesh.indices.at(offset)};

      // Vertex position
      const std::size_t startIndex{static_cast<size_t>(3 * index.vertex_index)};
      const float vx{attrib.vertices.at(startIndex + 0)};
      const float vy{attrib.vertices.at(startIndex + 1)};
      const float vz{attrib.vertices.at(startIndex + 2)};

      Vertex vertex{};
      vertex.position = {vx, vy, vz};

      // If hash doesn't contain this vertex
      if (hash.count(vertex) == 0) {
        // Add this index (size of m_vertices)
        hash[vertex] = m_vertices.size();
        // Add this vertex
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }
}

void OpenGLWindow::paintGL() {
  update();

  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  abcg::glUseProgram(m_program);

  // Get location of uniform variables (could be precomputed)
  const GLint viewMatrixLoc{
      abcg::glGetUniformLocation(m_program, "viewMatrix")};
  const GLint projMatrixLoc{
      abcg::glGetUniformLocation(m_program, "projMatrix")};

  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE,
                           &m_camera.m_projMatrix[0][0]);

  abcg::glBindVertexArray(m_VAO);

  //Draw box
  //m_box.paintGL();
  //Draw ball
  m_ball.paintGL();
  m_sun.paintGL();
  m_moon.paintGL();
  // Draw ground
  m_ground.paintGL();

  abcg::glUseProgram(0);
}

void OpenGLWindow::paintUI() { abcg::OpenGLWindow::paintUI(); }

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() {
  m_ground.terminateGL();

  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void OpenGLWindow::update() {
  const float deltaTime{static_cast<float>(getDeltaTime())};
  m_ball.update(deltaTime);
  m_sun.update(deltaTime);
  m_moon.update(deltaTime);
  checkCollision();
  m_camera.computeViewMatrix();
  //m_camera.orbit(0.25f*deltaTime);
}

//Function to verify collision, also responsible for the dynamic proposed with the colors.
void OpenGLWindow::checkCollision() {

  for (int i = 0; i < 3; i++) {
    if (std::abs(m_ball.m_position[i]) + m_ball.getRadius() > m_box.getSideLength()/2.0f) {
      if (std::signbit(m_ball.m_position[i]) == std::signbit(m_ball.m_velocity[i])) {
        m_ball.m_velocity[i] *= -1.0f;
        m_box.m_color = m_ball.m_color;
        m_ground.setColorTileOne(m_ball.m_color);
        m_ball.changeColor();
        m_ground.setColorTileTwo(m_ball.m_color);
      }
    }
  } 
}