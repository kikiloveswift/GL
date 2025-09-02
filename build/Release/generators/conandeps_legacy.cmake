message(STATUS "Conan: Using CMakeDeps conandeps_legacy.cmake aggregator via include()")
message(STATUS "Conan: It is recommended to use explicit find_package() per dependency instead")

find_package(spdlog)
find_package(glfw3)
find_package(glm)
find_package(glad)

set(CONANDEPS_LEGACY  spdlog::spdlog  glfw  glm::glm  glad::glad )