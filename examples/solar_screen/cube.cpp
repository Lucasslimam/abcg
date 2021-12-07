#include "cube.hpp"

void Cube::loadCubeTexture(const std::string& path) {
  //if (!std::filesystem::exists(path)) return;

  abcg::glDeleteTextures(1, &m_cubeTexture);
  m_cubeTexture = abcg::opengl::loadCubemap(
      {path + "posx.png", path + "negx.png", path + "posy.png",
       path + "negy.png", path + "posz.png", path + "negz.png"});

}

void Cube::initializeGL(GLuint program) {
  m_program = program;

  m_viewMatrixLoc = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLoc = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_skyTexLoc = abcg::glGetUniformLocation(m_program, "skyTex");
  // Generate VBO
  abcg::glGenBuffers(1, &m_vbo);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(m_positions),
                     m_positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  const GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_vao);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Cube::paintGL(const Camera& camera) {
  abcg::glUseProgram(m_program);
  abcg::glDepthMask(GL_FALSE);

  abcg::glUniformMatrix4fv(m_viewMatrixLoc, 1, GL_FALSE, &camera.computeSkyBoxViewMatrix()[0][0]);
  abcg::glUniformMatrix4fv(m_projMatrixLoc, 1, GL_FALSE, &camera.m_projMatrix[0][0]);
  abcg::glUniform1i(m_skyTexLoc, 0);

  abcg::glBindVertexArray(m_vao);

  abcg::glActiveTexture(GL_TEXTURE1);
  abcg::glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeTexture);

  abcg::glEnable(GL_CULL_FACE);
  abcg::glFrontFace(GL_CW);
  abcg::glDepthFunc(GL_LEQUAL);
  abcg::glDrawArrays(GL_TRIANGLES, 0, m_positions.size());
  abcg::glDepthFunc(GL_LESS);

  abcg::glBindVertexArray(0);
  abcg::glDepthMask(GL_TRUE);
  abcg::glUseProgram(0);
}

void Cube::terminateGL() {
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_vbo);
  abcg::glDeleteVertexArrays(1, &m_vao);
}