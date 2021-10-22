#include "openglwindow.hpp"

#include <imgui.h>

#include "abcg.hpp"

void OpenGLWindow::handleEvent(SDL_Event &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.set(static_cast<size_t>(Input::Fire));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.set(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.set(static_cast<size_t>(Input::Right));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Fire));
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Right));
  }

  // Mouse events
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_input.set(static_cast<size_t>(Input::Fire));
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT)
      m_gameData.m_input.reset(static_cast<size_t>(Input::Fire));
  }
}

void OpenGLWindow::initializeGL() {
  // Load a new font
  ImGuiIO &io{ImGui::GetIO()};
  auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  m_font = io.Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  // Create program to render the other objects
  m_objectsProgram = createProgramFromFile(getAssetsPath() + "objects.vert",
                                           getAssetsPath() + "objects.frag");

  abcg::glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  restart();
}

void OpenGLWindow::restart() {
  m_gameData.m_state = State::Playing;

  m_ship.initializeGL(m_objectsProgram);
  m_enemies.initializeGL(m_objectsProgram, 4, 11); //44 enemies
  m_bullets.initializeGL(m_objectsProgram);
}

void OpenGLWindow::update() {
  float deltaTime{static_cast<float>(getDeltaTime())};

  // Wait 5 seconds before restarting
  if (m_gameData.m_state != State::Playing &&
      m_restartWaitTimer.elapsed() > 4) {
    restart();
    return;
  }

  m_ship.update(m_gameData, deltaTime);
  m_enemies.update(deltaTime);
  m_bullets.update(m_ship, m_gameData, deltaTime);
  for (int line = 0; line < m_enemies.m_enemies.size(); line++) {
    auto &enemy_line = m_enemies.m_enemies[line];
    for (int column = 0; column < enemy_line.size(); column++) {
      if (m_enemies.can_shoot(line, column)) {
        m_bullets.update(enemy_line[column], m_ship, m_gameData, deltaTime);
      }
    }
  }

  if (m_gameData.m_state == State::Playing) {
    checkCollisions();
    checkWinCondition();
  }
}

void OpenGLWindow::paintGL() {
  update();

  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  m_enemies.paintGL();
  m_bullets.paintGL();
  m_ship.paintGL(m_gameData);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();

  {
    const auto size{ImVec2(300, 85)};
    const auto position{ImVec2((m_viewportWidth - size.x) / 2.0f,
                               (m_viewportHeight - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoBackground |
                           ImGuiWindowFlags_NoTitleBar |
                           ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    if (m_gameData.m_state == State::GameOver) {
      ImGui::Text("Game Over!");
    } else if (m_gameData.m_state == State::Win) {
      ImGui::Text("*You Win!*");
    }

    ImGui::PopFont();
    ImGui::End();
  }
}


void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::terminateGL() {
  abcg::glDeleteProgram(m_objectsProgram);
  m_enemies.terminateGL();
  m_bullets.terminateGL();
  m_ship.terminateGL();
}


void OpenGLWindow::checkCollisions() {
  // Check collision between ship and enemies
  for (const auto &enemy_line : m_enemies.m_enemies) {
    for (const auto &enemy : enemy_line) {
      if (enemy.m_hit) {continue;}
      const auto enemyTranslation{enemy.m_translation};
      const auto distance{glm::distance(m_ship.m_translation, enemyTranslation)};

      if (distance < m_ship.m_scale * 0.9f + enemy.m_scale * 0.85f) { //olhar a escala
        m_gameData.m_state = State::GameOver;
        m_restartWaitTimer.restart();
      }
    }
  }

  // Check collision between bullets and enemies
  for (auto &bullet : m_bullets.m_bullets) {
    if (bullet.m_dead or bullet.is_enemy) continue; /*enemies can't kill themselves and one another*/

    for (auto &enemy_line : m_enemies.m_enemies) {
      for (auto &enemy : enemy_line) {
        if (enemy.m_hit) {continue;}
        const auto enemyTranslation{enemy.m_translation};
        const auto distance{glm::distance(bullet.m_translation, enemyTranslation)};

        if (distance < m_bullets.m_scale + enemy.m_scale * 0.3f) {
          enemy.m_hit = true;
          m_enemies.num_enemies--;
          bullet.m_dead = true;
        }
      }
    }
  }
  /*Check collision between enemy bullets and ship*/
  for (auto &bullet : m_bullets.m_bullets) {
    if (bullet.m_dead) continue;

    if (m_ship.m_hit) {continue;}
    const auto shipTranslation{m_ship.m_translation};
    const auto distance{glm::distance(bullet.m_translation, shipTranslation)};

    if (distance < m_bullets.m_scale + m_ship.m_scale * 0.3f) {
      m_gameData.m_state = State::GameOver;
      m_restartWaitTimer.restart();
    }
  }
}
/*
X O O O 
X O O O
X O O O
*/

void OpenGLWindow::checkWinCondition() {
  if (m_enemies.num_enemies == 0) {
    m_gameData.m_state = State::Win;
    m_restartWaitTimer.restart();
  }
}

//pega o x do inimigo, o x da nave, subtrai e ve se ta dentro de um range