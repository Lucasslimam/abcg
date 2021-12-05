#ifndef ASTRO_HPP_
#define ASTRO_HPP_

#include "abcg.hpp"
#include "vertex.hpp"
#include <vector>
#include <random>
#include <filesystem>

class OpenGLWindow;
struct Vertex;

const float PI = 3.1415926535;

class Astro {
 public:
  virtual ~Astro() = default;

  void generateSphere(glm::vec3 position, float radius);
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();
  void loadModelFromFile(std::string_view path);
  void update(float deltaTime);
  float getRadius() const;
  void changeColor();
  glm::mat4 calcWorldMatrix();
  glm::mat4 calcLocalMatrix();
  void loadObj(std::string_view path, bool standardize = true);
  

 private:
  friend OpenGLWindow;

  Astro* m_parent{NULL};

  GLuint m_program{};
  GLuint m_diffuseTexture{};

  GLint m_viewMatrixLoc{};
  GLint m_projMatrixLoc{};
  GLint m_modelMatrixLoc{};
  GLint m_normalMatrixLoc{};
  GLint m_colorLoc{};
  GLint IaLoc, IdLoc, IsLoc;
  GLint KaLoc, KdLoc, KsLoc;
  GLint shininessLoc, lightDirLoc;
  
  //float m_radius{0.25};
  float m_radius{};

  int m_stackCount{16};
  int m_sectorCount{16};

  GLuint m_vao{};
  GLuint m_vbo{};
  GLuint m_ebo{};

  glm::vec3 m_scale{1.0f};
  glm::vec3 m_rotation{0.0f}; /*rotacao no proprio eixo*/
  glm::vec3 m_position{};
  glm::vec3 m_velocity{glm::vec3(1.0f, 0.7f, 0.5f)};
  glm::vec3 m_angularVelocity{0.0f};

  glm::vec3 m_color{glm::vec3(1.0f)};

  
  abcg::ElapsedTimer m_trailBlinkTimer;
  std::default_random_engine m_randomEngine;

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  glm::vec4 m_lightDir{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec4 m_Id{1.0f, 1.0f, 0.0f, 1.0f};
  glm::vec4 m_Is{1.0f, 1.0f, 0.0f, 1.0f};
  glm::vec4 m_Ka{0.1f, 0.1f, 0.1f, 1.0f};
  glm::vec4 m_Kd{0.7f, 0.7f, 0.7f, 1.0f};
  glm::vec4 m_Ks{1.0f, 1.0f, 1.0f, 1.0f};
  float m_shininess{25.0f};

  void createBuffers();
  void standardize();
  void loadDiffuseTexture(std::string_view path);
};
#endif