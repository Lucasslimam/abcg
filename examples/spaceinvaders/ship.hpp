#ifndef SHIP_HPP_
#define SHIP_HPP_

#include "abcg.hpp"
#include "gamedata.hpp"
#include "shooter.hpp"

class Enemies;
class Bullets;
class OpenGLWindow;

class Ship : Shooter {
 public:
  virtual ~Ship() = default;

  void initializeGL(GLuint program);
  void paintGL(const GameData &gameData);
  void terminateGL();

  void update(const GameData &gameData, float deltaTime);
  void setRotation(float rotation) { m_rotation = rotation; }

 private:
  friend Enemies;
  friend Bullets;
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};
  GLint m_rotationLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_ebo{};
  abcg::ElapsedTimer m_trailBlinkTimer;
};
#endif