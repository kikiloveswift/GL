//
// Created by liqiang on 2025/8/31.
//
/*
 * Target: use openGL render an image, maybe Jinx?
 ***/
#include "../../Utils/Logger.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "../../shader/Shader.h"
#include "ImageLoader.h"

int SCREEN_WINDTH = 800;
int SCREEN_HEIGHT = 600;

float vertices[] = {
    // position         // colors            // textures coords
    0.5f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,    1.0f, 1.0f, // top right
    0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,    1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
    -0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f,    0.0f, 1.0f, // top left
};

unsigned int indices[] = {
    0, 1, 3,   // 第一个三角形
    1, 2, 3    // 第二个三角形
};

void FrameCallback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

int main(int argc, char *argv[]) {
    Logger::init();
    LOG_INFO("Opengl 03 started");

    // NOTE: - INIT GLFW
    if (glfwInit() != GLFW_TRUE) {
        LOG_ERROR("GLFW INIT ERROR");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCREEN_WINDTH, SCREEN_HEIGHT, "Jinx", NULL, NULL);
    if (!window) {
        LOG_ERROR("GLFW create window error");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG_ERROR("GLAD Init Error");
        glfwTerminate();
        return -1;
    }

    // Init Shader
    Shader shader("../GettingStarted/opengl_03/opengl_03.vert", "../GettingStarted/opengl_03/opengl_03.frag");
    
    // generate VAO VBO EBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    // VAO
    glBindVertexArray(VAO);
    
    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Color (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Texture (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // loadTexture
    GLuint texture = ImageLoader::loadTexture("../Resources/jinx.png");
    if (texture == 0) {
        LOG_ERROR("Failed to load texture");
        glfwTerminate();
        return -1;
    }

    // use framebuffer size to fillup viewport
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);
    glfwSetFramebufferSizeCallback(window, FrameCallback);
    
    // use uniform
    shader.use();
    shader.setInt("ourTexture", 0);
    shader.setBool("flipY", true);

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        
        // clear color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        // use shader
        shader.use();
        
        // bind VAO
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // 清理资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    LOG_INFO("App finished");
    return 0;
}

void FrameCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}


