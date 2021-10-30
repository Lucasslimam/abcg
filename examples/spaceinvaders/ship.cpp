#include "ship.hpp"
#include <vector>
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include <stdio.h>

void Ship::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = abcg::glGetAttribLocation(m_program, "inColor");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  m_rotation = 0.0f;
  m_translation = glm::vec2(0, -0.78f);
  m_velocity = glm::vec2(0);
  is_ship = true;


  std::array<glm::vec2, 32> positions{
      //Drawing Left Wing
      glm::vec2{-0.083f, 0.458f}, glm::vec2{-0.347f, -0.246f},  glm::vec2{-0.073f, -0.099f},
      glm::vec2{-0.305, -0.386}, glm::vec2{-0.114f, -0.417f},
      //Drawing Right Wing
      glm::vec2{0.083f, 0.458f}, glm::vec2{0.347f, -0.246f},  glm::vec2{0.073f, -0.099f},
      glm::vec2{0.305, -0.386}, glm::vec2{0.114f, -0.417f},

      //Drawin the Middle
      glm::vec2{0.00f, 0.694f}, glm::vec2{-0.083f, 0.458f},  glm::vec2{+0.083f, 0.458f},
      glm::vec2{+0.073f, -0.099f}, glm::vec2{-0.073f, -0.099f},

      glm::vec2{-0.144f, -0.417f}, glm::vec2{0.144f, -0.417f},

      glm::vec2{-0.144f, -0.417f}, glm::vec2{+0.144f, -0.417f}, glm::vec2{-0.084f, -0.489f}, glm::vec2{+0.084f, -0.489f},

      glm::vec2{-0.084f, -0.489f}, glm::vec2{+0.084f, -0.489f}, glm::vec2{-0.066f, -0.604f}, glm::vec2{+0.066f, -0.604f},

      glm::vec2{-0.066f, -0.604f}, glm::vec2{+0.066f, -0.604f}, glm::vec2{+0.000f, -0.681f},

      glm::vec2{-0.014f, -0.640f}, glm::vec2{+0.014f, -0.640f}, glm::vec2{-0.014f, -0.713f}, glm::vec2{+0.014f, -0.713f}


      };

  // Ajdusting scale
  for (auto &position : positions) {
    position /= glm::vec2{0.4f, 0.4f};
  }
  int num_red_vertices = 10;
  int num_green_vertices = 22;
  int num_black_vertices = 0;
  int num_vertices = num_red_vertices + num_green_vertices + num_black_vertices;
  std::vector<glm::vec4> m_vertexColors = std::vector<glm::vec4>();

  for (int i = 0; i < num_vertices; i++) {
    if (i < num_red_vertices) {
      //Painting Wings
      m_vertexColors.push_back(glm::vec4{87, 46, 47, 255}/255.0f);
    } else if (i < num_green_vertices + num_red_vertices) {
      //Painting the middle
      m_vertexColors.push_back(glm::vec4{170, 166, 155, 255}/255.0f);
    }
  }
  std::vector<glm::vec4> colors(m_vertexColors);

  
  const std::array indices
    {0, 1, 2,         3, 1, 2,       4, 3, 2,
     5, 6, 7,         8, 6, 7,       9, 8, 7,
     10, 11, 12,      11, 12, 13,    14, 13, 11,
     15, 14, 13,      16, 15, 13,    17, 18, 19,
     20, 19, 18,      21, 22, 23,    22, 23, 24,
     27, 26, 25,      28, 29, 30,    29, 30, 31};


  // Generate VBO
  abcg::glGenBuffers(1, &m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, (positions.size())*sizeof(float)*2, positions.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate VBO of Colors
  abcg::glGenBuffers(1, &m_vboColors);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  abcg::glBufferData(GL_ARRAY_BUFFER,  (colors.size())*sizeof(glm::vec4),
                      colors.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_ebo);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};
  GLint colorAttribute{abcg::glGetAttribLocation(m_program, "inColor")};
  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

  //VAO Colors
  abcg::glEnableVertexAttribArray(colorAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
  abcg::glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}


void Ship::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;

  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_vao);

  abcg::glUniform1f(m_scaleLoc, m_scale);
  abcg::glUniform1f(m_rotationLoc, m_rotation);
  abcg::glUniform2fv(m_translationLoc, 1, &m_translation.x);

  // Restart thruster blink timer every 100 ms
  if (m_trailBlinkTimer.elapsed() > 100.0 / 1000.0) m_trailBlinkTimer.restart();

  abcg::glDrawElements(GL_TRIANGLES, 18*3, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Ship::terminateGL() {
  abcg::glDeleteBuffers(1, &m_vbo);
  abcg::glDeleteBuffers(1, &m_vboColors);
  abcg::glDeleteBuffers(1, &m_ebo);
  abcg::glDeleteVertexArrays(1, &m_vao);
}

void Ship::update(const GameData &gameData, float deltaTime) {
  float translation = 1.5f*deltaTime;
  float offset = 0.845;
  if (gameData.m_input[static_cast<size_t>(Input::Left)]) {
    m_translation += glm::vec2{-translation, 0.0f};
    if (m_translation.x <= -offset) {
      m_translation.x = -offset;
    }
  }

  if (gameData.m_input[static_cast<size_t>(Input::Right)]) {
    m_translation += glm::vec2{translation, 0.0f};
    if (m_translation.x >= offset) {
      m_translation.x = offset;
    }
  }
}