project(solar_screen)
add_executable(${PROJECT_NAME} astro.cpp camera.cpp main.cpp openglwindow.cpp)
enable_abcg(${PROJECT_NAME})
