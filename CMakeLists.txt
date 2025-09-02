cmake_minimum_required(VERSION 3.31)
project(Renderer)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 查找 Conan 生成的包
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(glad REQUIRED)
find_package(OpenGL REQUIRED)
find_package(spdlog REQUIRED)

# 创建一个函数来链接通用库
function(link_renderer_libs target)
    target_link_libraries(${target}
            glfw
            glm::glm
            glad::glad
            OpenGL::GL
            spdlog::spdlog
    )
endfunction()

add_executable(opengl_01 GettingStarted/opengl_01/opengl_01.cpp)
add_executable(opengl_02 GettingStarted/opengl_02/opengl_02.cpp)
add_executable(opengl_03 GettingStarted/opengl_03/opengl_03.cpp
        GettingStarted/opengl_03/ImageLoader.cpp
        GettingStarted/opengl_03/ImageLoader.h
        Utils/Logger.cpp
        Utils/Logger.h)

link_renderer_libs(opengl_01)
link_renderer_libs(opengl_02)
link_renderer_libs(opengl_03)
