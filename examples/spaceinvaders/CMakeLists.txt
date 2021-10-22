project(spaceinvaders)
add_executable(${PROJECT_NAME} main.cpp openglwindow.cpp enemies.cpp ship.cpp bullets.cpp)
enable_abcg(${PROJECT_NAME})