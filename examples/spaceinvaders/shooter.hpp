#ifndef SHOOTER_HPP_
#define SHOOTER_HPP_


#include "abcg.hpp"
#include "gamedata.hpp"


class Shooter {
  public:
    virtual ~Shooter() = default;
    bool m_hit{false};
    bool is_ship{false};
    glm::vec4 m_color{1};
    float m_rotation{};
    float m_scale{0.125f};
    glm::vec2 m_translation{glm::vec2(0)};
    glm::vec2 m_velocity{glm::vec2(0)};
    abcg::ElapsedTimer m_bulletCoolDownTimer;
};


#endif
