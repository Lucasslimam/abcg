#include "bullets.hpp"
#include <iostream>
#include <cppitertools/itertools.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Bullets::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  m_bullets.clear();

  std::array<glm::vec2, 6> positions {
    glm::vec2{0.0f, -1.0f},  glm::vec2{-0.4f, -0.8f}, glm::vec2{0.4f, -0.8f},
    glm::vec2{-0.4f, 0.8f}, 
    glm::vec2{0.4f, 0.8f},
    glm::vec2{0.0f, 1.0f} 
  };

  std::array<int, 12> indices {
    0, 1, 2,    1, 2, 3,    2, 3, 4,   3, 4, 5
  };

  for (auto &position : positions) {
    position /= glm::vec2(1.8f, 0.5f);
  }


  // Generate VBO of positions
  abcg::glGenBuffers(1, &m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);


  // Generate EBO
  abcg::glGenBuffers(1, &m_ebo);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  const GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

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

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Bullets::paintGL() {
  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_vao);

  abcg::glUniform1f(m_rotationLoc, 0);
  abcg::glUniform1f(m_scaleLoc, m_scale);

  for (const auto &bullet : m_bullets) {
    abcg::glUniform2f(m_translationLoc, bullet.m_translation.x,
                      bullet.m_translation.y);
    if(bullet.is_enemy) {
      abcg::glUniform4f(m_colorLoc, 59.0f/238.0f, 238.0f/255.0f, 71.0f/255.0f, 1);
    } else {
      abcg::glUniform4f(m_colorLoc, 59.0f/238.0f, 71.0f/255.0f, 238.0f/255.0f, 1);
    }

    abcg::glDrawElements(GL_TRIANGLES, 50, GL_UNSIGNED_INT, nullptr);
  }

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Bullets::terminateGL() {
  abcg::glDeleteBuffers(1, &m_vbo);
  abcg::glDeleteBuffers(1, &m_ebo);
  abcg::glDeleteVertexArrays(1, &m_vao);
}

void Bullets::update(Ship &ship, const GameData &gameData, float deltaTime) { //muda o nome das var
  // Create a pair of bullets
  if (gameData.m_input[static_cast<size_t>(Input::Fire)] &&
      gameData.m_state == State::Playing) {
    // At least 250 ms must have passed since the last bullets
    if (ship.m_bulletCoolDownTimer.elapsed() > 250.0 / 1000.0) { //add na classe base
      ship.m_bulletCoolDownTimer.restart();

      // Bullets are shot in the direction of the ship's forward vector
      glm::vec2 forward{glm::rotate(glm::vec2{0.0f, 1.0f}, ship.m_rotation)};
      glm::vec2 right{glm::rotate(glm::vec2{1.0f, 0.0f}, ship.m_rotation)};
      const auto cannonOffset{(11.0f / 15.5f) * ship.m_scale};
      const auto bulletSpeed{2.0f};

      Bullet bullet{.m_dead = false, .is_enemy = false,
                    .m_translation = ship.m_translation + right * cannonOffset,
                    .m_velocity = ship.m_velocity + forward * bulletSpeed};
      m_bullets.push_back(bullet);

      bullet.m_translation = ship.m_translation - right * cannonOffset;
      m_bullets.push_back(bullet);

      // Moves ship in the opposite direction
      ship.m_velocity -= forward * 0.1f;
    }
  }

  for (auto &bullet : m_bullets) {
    if (bullet.is_enemy) continue;
    bullet.m_translation -= ship.m_velocity * deltaTime;
    bullet.m_translation += bullet.m_velocity * deltaTime;

    // Kill bullet if it goes off screen
    if (bullet.m_translation.x < -1.1f) bullet.m_dead = true;
    if (bullet.m_translation.x > +1.1f) bullet.m_dead = true;
    if (bullet.m_translation.y < -1.1f) bullet.m_dead = true;
    if (bullet.m_translation.y > +1.1f) bullet.m_dead = true;
  }

  // Remove dead bullets
  m_bullets.remove_if([](const Bullet &p) { return p.m_dead; });
}

//inimigos nao podem atirar se tem bicho na frente (lista de listas com indices)
//de onde o tiro sai
//o tiro só sai se o inimigo esteve no mesmo X que o player

void Bullets::update(Enemies::Enemy &enemy, Ship &ship, const GameData &gameData, float deltaTime) { //muda o nome das var
  // Create a pair of bullets
  float difference = (enemy.m_translation - ship.m_translation).x;
  if (gameData.m_state == State::Playing && !enemy.m_hit && std::abs(difference) <= 0.009f) {
    // At least 250 ms must have passed since the last bullets
    if (enemy.m_bulletCoolDownTimer.elapsed() > 1000.0 / 1000.0) { //add na classe base
      enemy.m_bulletCoolDownTimer.restart();

      // Bullets are shot in the direction of the ship's forward vector
      glm::vec2 forward{glm::vec2{0.0f, -1.0f}};
      //const auto cannonOffset{(11.0f / 15.5f) * enemy.m_scale};
      const auto bulletSpeed{0.04f};

      Bullet bullet{.m_dead = false, .is_enemy = true,
                    .m_translation = enemy.m_translation + glm::vec2(0.0f, -0.6f)*enemy.m_scale,
                    .m_velocity = forward * bulletSpeed};
      m_bullets.push_back(bullet);
    }
  }

  for (auto &bullet : m_bullets) {
    if (!bullet.is_enemy) continue;
    bullet.m_translation -= enemy.m_velocity * deltaTime;
    bullet.m_translation += bullet.m_velocity * deltaTime;

    // Kill bullet if it goes off screen
    if (bullet.m_translation.x < -1.1f) bullet.m_dead = true;
    if (bullet.m_translation.x > +1.1f) bullet.m_dead = true;
    if (bullet.m_translation.y < -1.1f) bullet.m_dead = true;
    if (bullet.m_translation.y > +1.1f) bullet.m_dead = true;
  }

  // Remove dead bullets
  m_bullets.remove_if([](const Bullet &p) { return p.m_dead; });
}