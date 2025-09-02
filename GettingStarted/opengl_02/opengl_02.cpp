//
// Created by liqiang on 2025/8/31.
//

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

float vertices[] = {
    -0.5f, -0.5f, 0,
    0.5f, -0.5f, 0,
    0, 0.5f, 0
};

std::string vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)";

std::string fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)";

int InitGLFW() {
    if (glfwInit() != GLFW_TRUE) {
        cout << "GLFW INIT ERROR" << endl;
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

int InitGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "GLAD init error" << endl;
        return -1;
    }
    return 0;
}

void framebuffersizeCallback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

int main(int argc, char *argv[]) {
    if (InitGLFW() == -1) {
        glfwTerminate();
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OPENGL_02", NULL,NULL);
    if (window == NULL) {
        cout << "Window create failed" << endl;
        glfwTerminate();
        return -1;
    }

    // Set current window to Context
    glfwMakeContextCurrent(window);

    if (InitGLAD() == -1) {
        glfwTerminate();
        return -1;
    }

    // Set viewport
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffersizeCallback);

    // VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    // 💥 start of record
    glBindVertexArray(VAO);

    // VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Vertex Shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderSourceChar = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderSourceChar, NULL);
    glCompileShader(vertexShader);

    // check compile if success?
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::COMPILE FAILED!!!" << endl;
    }

    unsigned int fragShader;
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentSources = fragmentShaderSource.c_str();
    glShaderSource(fragShader, 1, &fragmentSources, NULL);
    glCompileShader(fragShader);

    // check fragment if success?
    int frag_success;
    char frag_infoLog[512];
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &frag_success);
    if (!frag_success) {
        glGetShaderInfoLog(fragShader, 512, NULL, frag_infoLog);
        cout << "ERROR::FRAGMENTSHADER::COMPILE FAILED!!!" << endl;
    }

    // now, vertex & frag shader both compile success, now link them into a program.
    // Step Link
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);

    int link_success;
    char link_infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, link_infoLog);
        cout << "ERROR::LINK LINKED ERROR";
    }

    // `glUseProgram` tells which program should be used, `glVertexAttribPointer` tells how to use data
    // (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
    // 1. first params 0 match the vertex shader location = 0. this should be matched.
    // 2. second params tells vertex size is 3.
    // 3. last params is offset value, current is vertex, so, the offset value should be 0.
    /**
    if we store data like this,
    float vertices[] = {
    // positions        color
     0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // first position & color
     0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // second position & color
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // third position & color
    };
    position offset is 0.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    color offset is 3 * sizeof(float)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
     */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // if we link success, we don't need to compile anymore, so delete the vertexShader/fragmentShader pointer
    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);

    // Runloop
    while (!glfwWindowShouldClose(window)) {
        // Event loop
        processInput(window);

        // clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}


void framebuffersizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}