#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

void Camera::computeProjectionMatrix(int width, int height) {
  m_projMatrix = glm::mat4(1.0f);
  const auto aspect{static_cast<float>(width) / static_cast<float>(height)};
  m_projMatrix = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 20.0f);
}

void Camera::computeViewMatrix() {
  m_viewMatrix = glm::lookAt(m_eye, m_at, m_up);
}

/*Completar*/
glm::mat4 Camera::computeSkyBoxViewMatrix() const {
  return glm::lookAt(glm::vec3(0.0), m_at-m_eye, m_up);
}

void Camera::orbit(float angle) {
  
  glm::mat4 transform{glm::mat4(1.0f)};
  //Rotaciona camera em torno da Lookat

  transform = glm::translate(transform, m_at);
  transform = glm::rotate(transform, -angle, m_up);
  transform = glm::translate(transform, -m_at);

  m_eye = transform * glm::vec4(m_eye, 1.0f);

  computeViewMatrix();
}


void Camera::dolly(float speed) {
  // Computa vetor forward (direcao da view)
  const glm::vec3 forward{glm::normalize(m_at - m_eye)};

  // Move eye e centro para frente (speed > 0) ou para tras (speed < 0)
  m_eye += forward * speed;
  m_at += forward * speed;

  computeViewMatrix();
}

void Camera::truck(float speed) {
  // Computa vetor forward (direcao da view)
  const glm::vec3 forward{glm::normalize(m_at - m_eye)};
  // Computa vetor para a esquerda
  const glm::vec3 left{glm::cross(m_up, forward)};

  // Move eye e centro para a esquerda (speed < 0) para a direita (speed > 0)
  m_at -= left * speed;
  m_eye -= left * speed;

  computeViewMatrix();
}

void Camera::pan(float speed) {
  glm::mat4 transform{glm::mat4(1.0f)};

  // Rotaciona camera em torno do proprio eixo Y dela
  transform = glm::translate(transform, m_eye);
  transform = glm::rotate(transform, -speed, m_up);
  transform = glm::translate(transform, -m_eye);

  m_at = transform * glm::vec4(m_at, 1.0f);

  computeViewMatrix();
}

