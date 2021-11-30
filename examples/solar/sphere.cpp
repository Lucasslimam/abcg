#include "sphere.hpp"

void Sphere::drawSphere() {
  m_vertices.clear();
  m_indices.clear();
  std::unordered_map<Vertex, GLuint> hash{};
  Vertex vertex{};

  float addX;
  float addY;
  float addZ;
  int numVert = 36;
  int totalAngle = 2.0f*M_PI;

  for (float theta = 0.0f; theta <= totalAngle; theta += (totalAngle/numVert)) {
    for (float phi = 0.0f; phi <= M_PI; phi += (totalAngle/numVert)) {
      addX = m_position.x + m_radius*cos(theta)*sin(phi);
      addY = m_position.y + m_radius*sin(theta)*sin(phi);
      addZ = m_position.z + m_radius*cos(phi);
      vertex.position = {addX, addY, addZ};
      if (hash.count(vertex) == 0) {
        hash[vertex] = m_vertices.size();
        m_vertices.push_back(vertex);
      }
    }
  }
}
