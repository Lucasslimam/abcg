project(solar_screen)
add_executable(${PROJECT_NAME} astro.cpp box.cpp camera.cpp ground.cpp main.cpp openglwindow.cpp)
enable_abcg(${PROJECT_NAME})
