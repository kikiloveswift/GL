cmake_minimum_required(VERSION 3.31)
project(Renderer)

set(CMAKE_CXX_STANDARD 20)

# 查找 Conan 生成的包
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(glad REQUIRED)
find_package(OpenGL REQUIRED)

# 创建一个函数来链接通用库
function(link_renderer_libs target)
    target_link_libraries(${target}
        glfw
        glm::glm
        glad::glad
        OpenGL::GL
    )
endfunction()

add_executable(Renderer main.cpp
        GettingStarted/opengl_01.cpp)

add_executable(opengl_01 GettingStarted/opengl_01.cpp)

link_renderer_libs(Renderer)
link_renderer_libs(opengl_01)
