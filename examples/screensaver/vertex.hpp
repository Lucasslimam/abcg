#ifndef VERTEX_HPP_
#define VERTEX_HPP_

#include <cstdint>
#include <functional>
#include <glm/gtx/hash.hpp>

struct Vertex {
  glm::vec3 position;

  bool operator==(const Vertex& other) const {
    return position == other.position;
  }
};

namespace std {
template <>
  struct hash<Vertex> {
    size_t operator()(Vertex const& vertex) const noexcept {
      const std::size_t h1{std::hash<glm::vec3>()(vertex.position)};
      return h1;
    }
  };
}

#endif