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

void Camera::orbit(float angle) {
  
  glm::mat4 transform{glm::mat4(1.0f)};
  //Rotate camera around the lookat

  transform = glm::translate(transform, m_at);
  transform = glm::rotate(transform, -angle, m_up);
  transform = glm::translate(transform, -m_at);

  m_eye = transform * glm::vec4(m_eye, 1.0f);

  computeViewMatrix();
}