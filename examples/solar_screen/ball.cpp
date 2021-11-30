#include "ball.hpp"
#include <vector>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include <stdio.h>

#include <fmt/core.h>
#include <imgui.h>
#include <tiny_obj_loader.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

#include "openglwindow.hpp"


const float PI = 3.1415926535;

void Ball::generateSphere() {
  // clear memory of prev arrays
  m_vertices.clear();
  m_indices.clear();

  Vertex vertex{};
  float xy;                           // vertex position
  float lengthInv = 1.0f / m_radius;    // vertex normal

  float sectorStep = 2 * PI / m_sectorCount;
  float stackStep = PI / m_stackCount;
  float sectorAngle, stackAngle;

  for(int i = 0; i <= m_stackCount; ++i) { 
    stackAngle = PI / 2 - i * stackStep;           // starting from pi/2 to -pi/2
    xy = m_radius * std::cos(stackAngle);                // r * cos(u)
    vertex.position.z = m_radius * std::sin(stackAngle); // r * sin(u)
         
    // add (sectorCount+1) vertices per stack
    // the first and last vertices have same position and normal, but different tex coords
    for (int j = 0; j <= m_sectorCount; ++j){
      sectorAngle = j * sectorStep;           // starting from 0 to 2pi

      // vertex position (x, y, z)
      vertex.position.x = xy * std::cos(sectorAngle);      // r * cos(u) * cos(v)
      vertex.position.y = xy * std::sin(sectorAngle);     // r * cos(u) * sin(v)
      
      // normalized vertex normal (nx, ny, nz)
      vertex.normal = lengthInv*vertex.position;
      
      // vertex tex coord (s, t) range between [0, 1]
      float s = (float)j / m_sectorCount;
      float t = (float)i / m_stackCount;
      vertex.texCoord = glm::vec2(s, t);

      m_vertices.push_back(vertex);
    }
  }
  int k1, k2;
  for(int i = 0; i < m_stackCount; ++i) {
    k1 = i * (m_sectorCount + 1);     // beginning of current stack
    k2 = k1 + m_sectorCount + 1;      // beginning of next stack

    for(int j = 0; j < m_sectorCount; ++j, ++k1, ++k2) {
      if(i != 0){
        m_indices.push_back(k1);
        m_indices.push_back(k2);
        m_indices.push_back(k1 + 1);
      }
      if(i != (m_stackCount-1)) {
        m_indices.push_back(k1 + 1);
        m_indices.push_back(k2);
        m_indices.push_back(k2 + 1);
      }
    }
  }
}

void Ball::loadModelFromFile(std::string_view path) {
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

void Ball::initializeGL(GLuint program) {
  terminateGL();

  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  m_program = program;
  m_viewMatrixLoc = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLoc = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLoc = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");

  //Declarar vetor positions

  // Generate VBO
  abcg::glGenBuffers(1, &m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices[0]) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_ebo);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices[0]) * m_indices.size(), m_indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  const GLint positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                              sizeof(Vertex), nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}


void Ball::paintGL() {

  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_vao);
  // Restart thruster blink timer every 100 ms
  if (m_trailBlinkTimer.elapsed() > 100.0 / 1000.0) m_trailBlinkTimer.restart();

  m_modelMatrix = glm::mat4(1.0f);
  m_modelMatrix = glm::translate(m_modelMatrix, m_position);
  m_modelMatrix = glm::rotate(m_modelMatrix, glm::radians(90.0f), glm::vec3(0, 1, 0));
  m_modelMatrix = glm::scale(m_modelMatrix, m_scale);
  

  abcg::glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix[0][0]);
  abcg::glUniform4f(m_colorLoc, m_color.r, m_color.g, m_color.b, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Ball::terminateGL() {
  abcg::glDeleteBuffers(1, &m_vbo);
  abcg::glDeleteBuffers(1, &m_ebo);
  abcg::glDeleteVertexArrays(1, &m_vao);
}

void Ball::update(float deltaTime) {
  m_position += m_velocity*deltaTime;
}

//const funcao nao modifica o objeto
float Ball::getRadius() const {
  return m_radius;
}

void Ball::changeColor() {
  std::uniform_real_distribution<float> rd(0.0f, 1.0f);
  m_color = glm::vec3(rd(m_randomEngine), rd(m_randomEngine), rd(m_randomEngine));
}