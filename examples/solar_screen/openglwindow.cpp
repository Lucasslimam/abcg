#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <unordered_map>
#include <glm/gtc/matrix_inverse.hpp>

void OpenGLWindow::handleEvent(SDL_Event& ev) {
  if (ev.type == SDL_KEYDOWN) {
    if (ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w)
      m_dollySpeed = 1.0f;
    if (ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s)
      m_dollySpeed = -1.0f;
    if (ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a)
      m_panSpeed = -1.0f;
    if (ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d)
      m_panSpeed = 1.0f;
    if (ev.key.keysym.sym == SDLK_q) m_truckSpeed = -1.0f;
    if (ev.key.keysym.sym == SDLK_e) m_truckSpeed = 1.0f;
  }
  if (ev.type == SDL_KEYUP) {
    if ((ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_w) &&
        m_dollySpeed > 0)
      m_dollySpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_s) &&
        m_dollySpeed < 0)
      m_dollySpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_a) &&
        m_panSpeed < 0)
      m_panSpeed = 0.0f;
    if ((ev.key.keysym.sym == SDLK_RIGHT || ev.key.keysym.sym == SDLK_d) &&
        m_panSpeed > 0)
      m_panSpeed = 0.0f;
    if (ev.key.keysym.sym == SDLK_q && m_truckSpeed < 0) m_truckSpeed = 0.0f;
    if (ev.key.keysym.sym == SDLK_e && m_truckSpeed > 0) m_truckSpeed = 0.0f;
  }
}

void OpenGLWindow::initializeGL() {
  abcg::glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_astro_program = createProgramFromFile(getAssetsPath() + "earth.vert",
                                    getAssetsPath() + "earth.frag");

  // Load model

  m_sun.m_velocity = glm::vec3(0.0f);
  m_sun.m_speedRotation = 0.2f;
  glm::vec3 m_sun_position = glm::vec3(0.0f, 0.0f, 0.0f);
  float m_sun_radius = 0.5f;
  m_sun.generateSphere(m_sun_position, m_sun_radius);
  m_sun.m_Ka = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
  m_sun.loadModel(getAssetsPath()+ "sun.obj");
  

  glm::vec3 m_earth_position = glm::vec3(3.5f, 0.0f, 0.0f);
  float m_earth_radius = 0.25f;
  m_earth.generateSphere(m_earth_position, m_earth_radius);
  m_earth.m_velocity = glm::vec3(0.0f);
  m_earth.m_speedRotation = 1.0f;
  m_earth.m_parent = &m_sun;
  m_earth.m_angularVelocity = glm::vec3(0.0f, PI/3, 0.0f);
  m_earth.loadModel(getAssetsPath()+ "earth.obj");
  
  m_moon.m_velocity = glm::vec3(0.0f);
  glm::vec3 m_moon_position = glm::vec3(0.5f, 0.0f, 0.0f);
  float m_moon_radius = 0.125f;
  m_moon.m_speedRotation = 1.5f;
  m_moon.generateSphere(m_moon_position, m_moon_radius);
  m_moon.m_parent = &m_earth;
  m_moon.m_angularVelocity = glm::vec3(0.2f, 2*PI, 0.2f);
  m_moon.loadModel(getAssetsPath()+ "moon.obj");

  m_earth.initializeGL(m_astro_program);
  m_sun.initializeGL(m_astro_program);
  m_moon.initializeGL(m_astro_program);
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
  const float deltaTime{static_cast<float>(getDeltaTime())};
  update(deltaTime);

  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  m_earth.paintGL(m_camera, deltaTime);
  m_sun.paintGL(m_camera, deltaTime);
  m_moon.paintGL(m_camera, deltaTime);

  abcg::glUseProgram(0);
}

void OpenGLWindow::paintUI() { abcg::OpenGLWindow::paintUI(); }

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  m_camera.computeProjectionMatrix(width, height);
}

void OpenGLWindow::terminateGL() {

  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void OpenGLWindow::update(float deltaTime) {
  m_earth.update(deltaTime);
  m_sun.update(deltaTime);
  m_moon.update(deltaTime);
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
  m_camera.computeViewMatrix(); 
}