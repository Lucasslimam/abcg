#include "enemies.hpp"
#include <iostream>
#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Enemies::initializeGL(GLuint program, int lines, int columns) {
  terminateGL();
  index_sequence = 8;
  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  m_program = program;
  m_colorLoc = abcg::glGetAttribLocation(m_program, "inColor");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Create asteroids
  m_enemies.clear();
  //m_enemies.resize(quantity);

  for (int k = 0; k < lines; k++) {
    std::vector<Enemy> enemy_line = std::vector<Enemy>();
    for (int m = 0; m < columns; m++) {       /*0.175 para cada margem da tela */
      Enemy enemy = createEnemy(glm::vec2(m*0.15f -0.7675f, 0.9f - k*0.25f), 0.15f);
      //Enemy enemy = createEnemy(glm::vec2(m*0.22f -0.7675f, 0.9f - k*0.25f), 0.15f);
      enemy_line.push_back(enemy);
    }
    m_enemies.push_back(enemy_line);
  }
  num_enemies = lines*columns;
}

void Enemies::paintGL() {
  abcg::glUseProgram(m_program);

  for (const auto &enemy_line : m_enemies) {
    for (const auto &enemy : enemy_line) {
      if (enemy.m_hit) continue;
      abcg::glBindVertexArray(enemy.m_vao);

      //abcg::glUniform4fv(m_colorLoc, 1, &enemy.m_color.r);
      abcg::glUniform1f(m_scaleLoc, enemy.m_scale);

      abcg::glUniform2f(m_translationLoc, enemy.m_translation.x,
                        enemy.m_translation.y);

      abcg::glDrawElements(GL_TRIANGLES, 50, GL_UNSIGNED_INT, nullptr);
      abcg::glBindVertexArray(0);
    }
  }

  abcg::glUseProgram(0);
}

void Enemies::terminateGL() {
  for (auto &enemy_line : m_enemies) {
    for (auto &enemy : enemy_line) {
      abcg::glDeleteBuffers(1, &enemy.m_vbo);
      abcg::glDeleteBuffers(1, &enemy.m_vboColors);
      abcg::glDeleteBuffers(1, &enemy.m_ebo);
      abcg::glDeleteVertexArrays(1, &enemy.m_vao);
    }
  }
}

glm::vec2 Enemies::next_movement() {
  char c = sequence[index_sequence];
  index_sequence = (index_sequence + 1)%(sequence.length());
  switch(c) {
    case 'R':
      return glm::vec2(1.0f, 0.0f);
    case 'L':
      return glm::vec2(-1.0f, 0.0f);
    case 'D':
      return glm::vec2(0.0f, -1.0f);
    default:
      return glm::vec2(0.0f, 0.0f);
  }
}


void Enemies::update(float deltaTime) {

  // Restart thruster blink timer every 100 ms
  if (move_timer.elapsed() > 500.0 / 1000.0) {
    move_timer.restart();
    glm::vec2 move = next_movement();
    for (auto &enemy_line : m_enemies) {
      for (auto &enemy : enemy_line) {
        if (enemy.m_hit) continue;
        enemy.m_translation += move*0.08f;
      }
    }
  }
}

bool Enemies::can_shoot(int line, int column) {
  if (line + 1 >= m_enemies.size()) {
    return true;
  }
  return m_enemies[line +1][column].m_hit;
}



Enemies::Enemy Enemies::createEnemy(glm::vec2 translation,
                                              float scale) {
  Enemy enemy;
  enemy.is_ship = false;

  // Choose a random color (actually, a grayscale)
  enemy.m_scale = scale;
  enemy.m_translation = translation;

  // Create geometry
  std::array<glm::vec2, 40> positions{
    //Asa esquerda
    glm::vec2{-0.640f, 0.484f}, glm::vec2{-0.563f, 0.484f}, glm::vec2{-0.563f, -0.585f},
    glm::vec2{-0.640f, -0.585f},

    glm::vec2{-0.640f, -0.151f}, glm::vec2{-0.255f, -0.120f}, glm::vec2{-0.640f, 0.029f},
    glm::vec2{-0.255f, 0.005f},

    //Asa direita
    glm::vec2{0.640f, 0.484f}, glm::vec2{0.563f, 0.484f}, glm::vec2{0.563f, -0.585f},
    glm::vec2{0.640f, -0.585f},

    glm::vec2{0.640f, -0.151f}, glm::vec2{0.255f, -0.120f}, glm::vec2{0.640f, 0.029f},
    glm::vec2{0.255f, 0.005f},

    //Cabine
    glm::vec2{0.00f, -0.221f}, glm::vec2{-0.084573f, -0.204177f}, glm::vec2{-0.156271f, -0.156271f},
    glm::vec2{-0.204177f, -0.084573f}, glm::vec2{-0.221f, 0.00f}, glm::vec2{-0.204177f, 0.084573f},
    glm::vec2{-0.156271f, 0.156271f}, glm::vec2{-0.084573f, 0.204177f}, glm::vec2{0.00f, 0.221f},
    glm::vec2{0.084573f, 0.204177}, glm::vec2{0.156271f, 0.156271f}, glm::vec2{0.204177f, 0.084573f},
    glm::vec2{0.221f, -0.00f}, glm::vec2{0.204177f, -0.084573f}, glm::vec2{0.156271f, -0.156271f},
    glm::vec2{0.084573f, -0.204177f}
  };

  for (auto &position : positions) {
    //position /= glm::vec2{15.5f, 15.5f};
    position *= 0.7f; //0.15
  }

  std::array<int, 50> indices
  {0, 1, 2,     1, 2, 3,    4, 5, 6,     5, 6, 7,
   8, 9, 10,    9, 10, 11,  12, 13, 14,  13, 14, 15,
   
   16, 17, 18,   16, 19, 20,    16, 21, 22,   16, 23, 24,
   16, 25, 26,  16, 27, 28,    16, 29, 30,   16, 17, 31};

  std::vector<glm::vec4> colors = std::vector<glm::vec4>();

  for (int i = 0; i < positions.size(); i++) {
    colors.push_back(glm::vec4{87.0f, 46.0f, 47.0f, 255.0f}/255.0f);
  }

  // Generate VBO
  abcg::glGenBuffers(1, &enemy.m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, enemy.m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);


  // Generate VBO of Colors
  abcg::glGenBuffers(1, &enemy.m_vboColors);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, enemy.m_vboColors);
  abcg::glBufferData(GL_ARRAY_BUFFER, (colors.size())*sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);


  // Generate EBO
  abcg::glGenBuffers(1, &enemy.m_ebo);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, enemy.m_ebo);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};
  GLint colorAttribute{abcg::glGetAttribLocation(m_program, "inColor")};

  // Create VAO
  abcg::glGenVertexArrays(1, &enemy.m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(enemy.m_vao);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, enemy.m_vbo);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, enemy.m_ebo);

  //VAO Colors
  abcg::glEnableVertexAttribArray(colorAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, enemy.m_vboColors);
  abcg::glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);

  return enemy;
}