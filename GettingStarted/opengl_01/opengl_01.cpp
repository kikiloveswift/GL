//
// Created by liqiang on 2025/8/30.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

int InitGlfw(void) {
    if (!glfwInit()) {
        cout << "Failed to init GLFW" << endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    return 0;
}

int InitGlad(void) {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to init GLAD" << endl;
        return -1;
    }
    return 0;
}

int WIDTH = 800;
int HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main(int argc, char *argv[]) {
    if (InitGlfw() == -1) {
        return -1;
    }

    // 创建 window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OPENGL_01", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    // 使用当前 context
    glfwMakeContextCurrent(window);

    if (InitGlad() == -1) {
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);

    // 注册帧回调
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 开启 runloop 循环
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}