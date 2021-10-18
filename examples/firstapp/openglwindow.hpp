#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include "abcg.hpp"

enum class SquareState
{
  EMPTY,
  X,
  O
};

class OpenGLWindow : public abcg::OpenGLWindow 
{
protected:
    void initializeGL() override;
    void paintGL() override;
    void paintUI() override;
    void function(int n, int m);

private:
    std::array<float, 4> m_clearColor{0.906f, 0.910f, 0.918f, 1.0f};
    std::array<std::array<SquareState, 3>, 3> boardState{};
    int turn = 0;
};





#endif