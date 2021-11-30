#include "sphere.hpp"

void Sphere::drawSphere() {
  float auxTheta = 0.0f;
  float auxPhi = 0.0f;
  float addX, addY, addZ;
  int numVert = 36;
  int totalAngle = 2.0f*M_PI;

  for (float theta = 0.0f; theta <= totalAngle; theta += totalAngle/numVert) {
    for (float phi = 0.0f, phi <= M_PI); phi += totalAngle/numVert) {
        addX = m_position.x + radius*cos(theta)*sin(phi);
        addY = m_position.y + radius*sin(theta)*sin(phi);
        addZ = m_position.z + radius*cos(phi);
        m_vertices.push_back(glm::vec3(addX, addY, addZ));
    }
  }
}
