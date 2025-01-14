#include "astro.hpp"
#include "camera.hpp"
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


/*
No processamento abaixo utilizamos uma analogia
com a geografia e utilizamos Stacks como os paralelos
e Sectors como os meridianos.

Alem disso, utilizei as seguintes relacoes matematicas

X = (r*cos(u))*cos(v)
Y = (r*cos(u))*sen(v)
Z = r*sen(u)
*/

void Astro::generateSphere(glm::vec3 position, float radius) {
  m_position = position;
  m_radius = radius; 

  m_vertices.clear();
  m_indices.clear();

  Vertex vertex{};
  float xy;
  float lengthInv = 1.0f / m_radius; 
  float sectorStep = 2 * PI / m_sectorCount;
  float stackStep = PI / m_stackCount;
  float sectorAngle, stackAngle;

  /*
  v = (2*PI*(Passo-Sector)) / (Contador-Sector)
  u =  (PI/2) - (PI*(Passo-Stack)/Contador-Stack)
  */

  for(int i = 0; i <= m_stackCount; ++i) { 
    stackAngle = PI / 2 - i * stackStep;  // seguindo intervalo [pi/2, -pi/2]
    xy = m_radius * std::cos(stackAngle);                // r * cos(u)
    vertex.position.z = m_radius * std::sin(stackAngle); // r * sin(u)
         
    for (int j = 0; j <= m_sectorCount; ++j){
      sectorAngle = j * sectorStep; // Indo de 0 rad para 2*PI rad

      // vertex position (x, y, z)
      vertex.position.x = xy * std::cos(sectorAngle);  // r * cos(u) * cos(v)
      vertex.position.y = xy * std::sin(sectorAngle);  // r * cos(u) * sin(v)
      
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

void Astro::initializeGL(GLuint program) {
  terminateGL();

  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

  m_program = program;
  abcg::glUseProgram(m_program);

  /*Dando Locs necessários que serão utilizados no paintGL*/
  m_viewMatrixLoc = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLoc = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLoc = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_normalMatrixLoc = abcg::glGetUniformLocation(m_program, "normalMatrix");
  m_diffuseTexLoc = abcg::glGetUniformLocation(m_program, "diffuseTex");
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  IaLoc = abcg::glGetUniformLocation(m_program, "Ia");
  IdLoc = abcg::glGetUniformLocation(m_program, "Id");
  IsLoc = abcg::glGetUniformLocation(m_program, "Is");
  KaLoc = abcg::glGetUniformLocation(m_program, "Ka");
  KdLoc = abcg::glGetUniformLocation(m_program, "Kd");
  KsLoc = abcg::glGetUniformLocation(m_program, "Ks");
  lightDirLoc = abcg::glGetUniformLocation(m_program, "lightDirWorldSpace");
  shininessLoc = abcg::glGetUniformLocation(m_program, "shininess");


  // Gerando VBO
  abcg::glGenBuffers(1, &m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices[0]) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Gerando EBO
  abcg::glGenBuffers(1, &m_ebo);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices[0]) * m_indices.size(), m_indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Criando VAO
  abcg::glGenVertexArrays(1, &m_vao);

  abcg::glBindVertexArray(m_vao);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  const GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

  GLsizei normalOffset{sizeof(glm::vec3)};
  const GLint normalAttribute{abcg::glGetAttribLocation(m_program, "inNormal")};
  if (normalAttribute >= 0) {  
    abcg::glEnableVertexAttribArray(normalAttribute);
    abcg::glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                              reinterpret_cast<void*>(normalOffset));
  }
  const GLint texCoordAttribute{abcg::glGetAttribLocation(m_program, "inTexCoord")};
  if (texCoordAttribute >= 0) {
    abcg::glEnableVertexAttribArray(texCoordAttribute);
    GLsizei texOffset{sizeof(glm::vec3) + sizeof(glm::vec3)};
    abcg::glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
                                      reinterpret_cast<void*>(texOffset));
  }

  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  
  // End of binding to current VAO
  abcg::glBindVertexArray(0);
  abcg::glUseProgram(0);
}

void Astro::paintGL(const Camera& camera, float deltaTime) {

  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_vao);
  // Restart thruster blink timer every 100 ms
  if (m_trailBlinkTimer.elapsed() > 100.0 / 1000.0) m_trailBlinkTimer.restart();

  glm::mat4 modelMatrix = calcWorldMatrix();
  modelMatrix = rotate(modelMatrix, m_angle, m_rotation);
  m_angle += PI/4*m_speedRotation*deltaTime;

  abcg::glUniform4fv(lightDirLoc, 1, &m_lightDir.x);
  abcg::glUniform1f(shininessLoc, m_shininess);
  abcg::glUniform4fv(IaLoc, 1, &m_Ia.x);
  abcg::glUniform4fv(IdLoc, 1, &m_Id.x);
  abcg::glUniform4fv(IsLoc, 1, &m_Is.x);
  abcg::glUniform4fv(KaLoc, 1, &m_Ka.x);
  abcg::glUniform4fv(KdLoc, 1, &m_Kd.x);
  abcg::glUniform4fv(KsLoc, 1, &m_Ks.x);

  abcg::glUniformMatrix4fv(m_viewMatrixLoc, 1, GL_FALSE, &camera.m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(m_projMatrixLoc, 1, GL_FALSE, &camera.m_projMatrix[0][0]);

  GLint textureUnit{0};
  abcg::glActiveTexture(GL_TEXTURE0 + textureUnit);
  abcg::glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);
  abcg::glUniform1i(m_diffuseTexLoc, textureUnit);

  // Set minification and magnification parameters
  abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Set texture wrapping parameters
  abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  abcg::glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

  abcg::glUniform4f(m_colorLoc, m_color.r, m_color.g, m_color.b, 1.0f);
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Astro::terminateGL() {
  abcg::glDeleteBuffers(1, &m_vbo);
  abcg::glDeleteBuffers(1, &m_ebo);
  abcg::glDeleteVertexArrays(1, &m_vao);
}


void Astro::update(float deltaTime) {
  glm::vec3 angle = m_angularVelocity*deltaTime;

  glm::mat4 transform{glm::mat4(1.0f)};
  transform = glm::rotate(transform, -angle.y, glm::vec3(0.0f, 1.0f, 0.0f));
  transform = glm::rotate(transform, -angle.x, glm::vec3(1.0f, 0.0f, 0.0f));
  transform = glm::rotate(transform, -angle.z, glm::vec3(0.0f, 0.0f, 1.0f));
  m_position = transform * glm::vec4(m_position, 1.0f);
}

//const funcao nao modifica o objeto
float Astro::getRadius() const {
  return m_radius;
}

void Astro::changeColor() {
  std::uniform_real_distribution<float> rd(0.0f, 1.0f);
  m_color = glm::vec3(rd(m_randomEngine), rd(m_randomEngine), rd(m_randomEngine));
}


/*Na funcao abaixo eh calculada a modelMatrix do objeto 
independente de qualquer outro*/

glm::mat4 Astro::calcLocalMatrix() {
  glm::mat4 localMatrix = glm::mat4(1.0f);
  localMatrix = glm::translate(localMatrix, m_position);
  localMatrix = glm::rotate(localMatrix, 0.0f, glm::vec3(0, 1, 0));
  localMatrix = glm::scale(localMatrix, m_scale);
  return localMatrix;
}

/*Nessa funcao, de forma quase recursiva, ha o calculo de cada Astro em relacao
ao Astro que ele esta circundando, interferindo no calculo da matrix modelo*/
glm::mat4 Astro::calcWorldMatrix() {
  glm::mat4 world = calcLocalMatrix();
  for (Astro* ball = m_parent; ball; ball=ball->m_parent) {
    world = ball->calcLocalMatrix()*world;
  }
  return world;
}

void Astro::loadDiffuseTexture(std::string_view path) {
  if (!std::filesystem::exists(path)) {
    std::cout << "Erro Textura nao existe\n";
    return;
  }

  abcg::glDeleteTextures(1, &m_diffuseTexture);
  m_diffuseTexture = abcg::opengl::loadTexture(path);
}

void Astro::loadObj(std::string_view path) {
  const auto basePath{std::filesystem::path{path}.parent_path().string() + "/"};

  tinyobj::ObjReaderConfig readerConfig;
  readerConfig.mtl_search_path = basePath;  // Path to material files

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data(), readerConfig)) {
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

  const auto& materials{reader.GetMaterials()};

  // Use properties of first material, if available
  if (!materials.empty()) {
    const auto& mat{materials.at(0)};  // First material
    m_Ka = glm::vec4(mat.ambient[0], mat.ambient[1], mat.ambient[2], 1);
    m_Kd = glm::vec4(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1);
    m_Ks = glm::vec4(mat.specular[0], mat.specular[1], mat.specular[2], 1);
    m_shininess = mat.shininess;

    if (!mat.diffuse_texname.empty())
      loadDiffuseTexture(basePath + mat.diffuse_texname);
    else
      std::cout << "Falha em criar textura!";
  } else {
    // Default values
    std::cout << "Falha em carregar material!";
    m_Ka = {0.1f, 0.1f, 0.1f, 1.0f};
    m_Kd = {0.7f, 0.7f, 0.7f, 1.0f};
    m_Ks = {1.0f, 1.0f, 1.0f, 1.0f};
    m_shininess = 25.0f;
  }
}

void Astro::loadModel(std::string_view path) {
  terminateGL();
  loadObj(path);
}
