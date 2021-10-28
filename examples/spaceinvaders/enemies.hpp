#ifndef ENEMIES_HPP_
#define ENEMIES_HPP_

#include <list>
#include <random>
#include <vector>
#include "abcg.hpp"
#include "gamedata.hpp"
#include "ship.hpp"
#include "shooter.hpp"

class OpenGLWindow;

class Enemies {
 public:
  void initializeGL(GLuint program, int lines, int columns, bool first_iter);
  void paintGL();
  void terminateGL();

  void update(float deltaTime);
  glm::vec2 next_movement();
  bool can_shoot(int line, int column);
  int num_enemies{0};

  struct Enemy : Shooter {
    virtual ~Enemy() = default;
    GLuint m_vao{};
    GLuint m_vbo{};
    GLuint m_vboColors{};
    GLuint m_ebo{};

    bool m_hit{false};
    int m_polygonSides{};
    std::vector<glm::vec4> colors;
  };

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  std::string sequence = "DLLLLDRRRR";
  int index_sequence = 8; //começa indo para a direita

  std::vector<std::vector<Enemy>> m_enemies;
  std::vector<std::vector<int>> m_countFrames;

  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomDist{-1.0f, 1.0f};

  Enemies::Enemy createEnemy(glm::vec2 translation = glm::vec2(0), float scale = 0.25f, bool isDying = false);

  abcg::ElapsedTimer move_timer;

};

#endif