#include <fmt/core.h>

#include "openglwindow.hpp"

#include <imgui.h>

#include <vector>


void OpenGLWindow::initializeGL() {
  auto windowSettings{getWindowSettings()};
  fmt::print("Initial window size: {}x{}\n", windowSettings.width,
             windowSettings.height);
}

void OpenGLWindow::paintGL() {
  // Set the clear color
  abcg::glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2],
                     m_clearColor[3]);
  // Clear the color buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT);
}


void OpenGLWindow::paintUI() {
  // Parent class will show fullscreen button and FPS meter
  //abcg::OpenGLWindow::paintUI();

  // Our own ImGui widgets go below
  {
    auto windowSettings{getWindowSettings()};
    // Window begin
    ImGui::SetNextWindowSize(ImVec2(windowSettings.width, windowSettings.height));
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::Begin("Jogo da Velha!");

    // Static text
    
    ImGui::Text("Current window size: %dx%d (in windowed mode)",
                windowSettings.width, windowSettings.height);

    // ColorEdit to change the clear color
    //ImGui::ColorEdit3("clear color", m_clearColor.data());

    ImVec2 buttonSize{-1, (0.8f*windowSettings.height/3.0f)};

    ImGui::Columns(3, nullptr, false);
    for (int i = 0; i < 3; i++) {
      ImGui::PushID(i);
      for (int k = 0; k < 3; k++) {
        ImGui::PushID(k);
        switch (boardState[i][k])
        {
        case SquareState::X:
          ImGui::Button("X", buttonSize);
          break;
        case SquareState::O:
          ImGui::Button("O", buttonSize);
          break;
        case SquareState::EMPTY:
          if (ImGui::Button(" ", buttonSize)) {
            boardState[i][k] = (turn % 2 == 0) ? SquareState::X : SquareState::O;
            turn++;
          }
          break;
        }
        ImGui::PopID();
      }
      ImGui::PopID();
      ImGui::NextColumn();
    }

    // Window end
    ImGui::End();
  }
}