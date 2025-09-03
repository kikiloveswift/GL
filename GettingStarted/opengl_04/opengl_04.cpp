//
// Created by liqiang on 2025/9/3.
//
#include "Utils/Logger.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void testGLM();

int main(int argc, char *argv[]) {
    Logger::init();
    testGLM();
    return 0;
}

void testGLM() {
    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
    vec = trans * vec;
    LOG_INFO("Res vector: ({:.1f}, {:.1f}, {:.1f})", vec.x, vec.y, vec.z);
}
