#include "asteroids.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Asteroids::initializeGL(GLuint program, int quantity) {
  terminateGL();

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  m_program = program;
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Create asteroids
  m_asteroids.clear();
  m_asteroids.resize(quantity);

  for (auto &asteroid : m_asteroids) {
    asteroid = createAsteroid();

    // Make sure the asteroid won't collide with the ship
    do {
      asteroid.m_translation = {m_randomDist(m_randomEngine),
                                m_randomDist(m_randomEngine)};
    } while (glm::length(asteroid.m_translation) < 0.5f);
  }
}

void Asteroids::paintGL() {
  abcg::glUseProgram(m_program);

  for (const auto &asteroid : m_asteroids) {
    abcg::glBindVertexArray(asteroid.m_vao);

    abcg::glUniform4fv(m_colorLoc, 1, &asteroid.m_color.r);
    abcg::glUniform1f(m_scaleLoc, asteroid.m_scale);
    abcg::glUniform1f(m_rotationLoc, asteroid.m_rotation);

    for (auto i : {-2, 0, 2}) {
      for (auto j : {-2, 0, 2}) {
        abcg::glUniform2f(m_translationLoc, asteroid.m_translation.x + j,
                          asteroid.m_translation.y + i);

        abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, asteroid.m_polygonSides + 2);
      }
    }

    abcg::glBindVertexArray(0);
  }

  abcg::glUseProgram(0);
}

void Asteroids::terminateGL() {
  for (auto asteroid : m_asteroids) {
    abcg::glDeleteBuffers(1, &asteroid.m_vbo);
    abcg::glDeleteVertexArrays(1, &asteroid.m_vao);
  }
}