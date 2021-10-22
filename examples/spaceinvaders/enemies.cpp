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
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Create asteroids
  m_enemies.clear();
  //m_enemies.resize(quantity);

  for (int k = 0; k < lines; k++) {
    std::vector<Enemy> enemy_line = std::vector<Enemy>();
    for (int m = 0; m < columns; m++) {       /*0.175 para cada margem da tela */
      Enemy enemy = createEnemy(glm::vec2(m*0.15f -0.7675f, 0.9f - k*0.25f), 0.15f);
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

      abcg::glUniform4fv(m_colorLoc, 1, &enemy.m_color.r);
      abcg::glUniform1f(m_scaleLoc, enemy.m_scale);

      abcg::glUniform2f(m_translationLoc, enemy.m_translation.x,
                        enemy.m_translation.y);

      abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, enemy.m_polygonSides + 2);
      abcg::glBindVertexArray(0);
    }
  }

  abcg::glUseProgram(0);
}

void Enemies::terminateGL() {
  for (auto &enemy_line : m_enemies) {
    for (auto &enemy : enemy_line) {
      abcg::glDeleteBuffers(1, &enemy.m_vbo);
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

  auto &re{m_randomEngine};  // Shortcut

  enemy.m_polygonSides = 3;

  // Choose a random color (actually, a grayscale)
  std::uniform_real_distribution<float> randomIntensity(0.5f, 1.0f);
  enemy.m_color = glm::vec4(1) * randomIntensity(re);

  enemy.m_color.a = 1.0f;
  enemy.m_scale = scale;
  enemy.m_translation = translation;

  // Create geometry
  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);
  const auto step{M_PI * 2 / enemy.m_polygonSides};
  std::uniform_real_distribution<float> randomRadius(0.5f, 0.5f);
  for (const auto angle : iter::range(0.0, M_PI * 2, step)) {
    const auto radius{randomRadius(re)};
    positions.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO
  abcg::glGenBuffers(1, &enemy.m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, enemy.m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &enemy.m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(enemy.m_vao);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, enemy.m_vbo);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);

  return enemy;
}