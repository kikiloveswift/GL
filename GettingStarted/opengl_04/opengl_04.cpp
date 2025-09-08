//
// Created by liqiang on 2025/9/3.
//
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../../Utils/Logger.h"
#include "../../shader/Shader.h"
#include "../../Utils/ImageLoader.h"

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

// 全局变量用于鼠标控制 - 使用四元数实现轨迹球旋转
glm::quat currentRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // 单位四元数
float mouseSensitivity = 2.0f;

// 鼠标状态变量
bool mousePressed = false;
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;

void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
glm::vec3 screenToSphere(float x, float y);

int main(int argc, char *argv[]) {
    Logger::init();
    // 1. Init glfw
    if (glfwInit() != GLFW_TRUE) {
        LOG_ERROR("GLFW Init Error");
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Coordinate System", NULL, NULL);
    if (!window) {
        glfwTerminate();
        LOG_ERROR("GLFW window create Error");
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // 设置鼠标回调函数
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // 2. Init glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG_ERROR("GLAD Init Error");
        glfwTerminate();
        return -1;
    }

    // 3. 设置视口
    // glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // 4. 创建正方体的顶点数据 (位置 + 纹理坐标 + 面ID)
    float vertices[] = {
        // 前面 (面ID = 0)
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,
        
        // 后面 (面ID = 1)
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,
        
        // 左面 (面ID = 2)
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  2.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  2.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  2.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  2.0f,
        
        // 右面 (面ID = 3)
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  3.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  3.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  3.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  3.0f,
        
        // 上面 (面ID = 4)
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  4.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  4.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  4.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  4.0f,
        
        // 下面 (面ID = 5)
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  5.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  5.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  5.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  5.0f
    };

    // 正方体的索引数据
    unsigned int indices[] = {
        // 前面
        0, 1, 2,   2, 3, 0,
        // 后面
        4, 5, 6,   6, 7, 4,
        // 左面
        8, 9, 10,  10, 11, 8,
        // 右面
        12, 13, 14, 14, 15, 12,
        // 上面
        16, 17, 18, 18, 19, 16,
        // 下面
        20, 21, 22, 22, 23, 20
    };

    // 5. 创建和配置VAO VBO EBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // 纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // 面ID属性
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 6. 加载6个不同的纹理
    GLuint textures[6];
    const char* textureFiles[6] = {
        "../Resources/Gemini_Generated_Image_nxkhggnxkhggnxkh1.png", // 前面
        "../Resources/Gemini_Generated_Image_nxkhggnxkhggnxkh2.png", // 后面
        "../Resources/Gemini_Generated_Image_nxkhggnxkhggnxkh3.png", // 左面
        "../Resources/Gemini_Generated_Image_nxkhggnxkhggnxkh4.png", // 右面
        "../Resources/Gemini_Generated_Image_nxkhggnxkhggnxkh5.png", // 上面
        "../Resources/Gemini_Generated_Image_nxkhggnxkhggnxkh6.png"  // 下面
    };
    
    for (int i = 0; i < 6; i++) {
        textures[i] = ImageLoader::loadTexture(textureFiles[i]);
        if (textures[i] == 0) {
            LOG_ERROR("Failed to load texture: {}", textureFiles[i]);
            return -1;
        }
    }

    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    // 7. 创建着色器程序
    Shader shader("../GettingStarted/opengl_04/opengl_04.vert",
        "../GettingStarted/opengl_04/opengl_04.frag");

    // 8. 主渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 处理输入
        processInput(window);

        // 清除缓冲区
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 使用着色器程序
        shader.use();

        // 绑定多个纹理
        for (int i = 0; i < 6; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textures[i]);
        }
        
        // 设置纹理采样器数组
        shader.setInt("textures[0]", 0);
        shader.setInt("textures[1]", 1);
        shader.setInt("textures[2]", 2);
        shader.setInt("textures[3]", 3);
        shader.setInt("textures[4]", 4);
        shader.setInt("textures[5]", 5);

        // 创建变换矩阵 - 使用四元数
        glm::mat4 model = glm::mat4(1.0f);
        model = model * glm::mat4_cast(currentRotation);

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        glm::mat4 projection = glm::perspective(glm::radians(60.0f),
                                              (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 
                                              0.1f, 100.0f);

        // 传递矩阵到着色器
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        // 绘制正方体
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // 交换缓冲区并轮询事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 清理资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(6, textures);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// 鼠标移动回调函数 - 轨迹球旋转
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // 只有在鼠标按下时才处理旋转
    if (!mousePressed) {
        lastX = xpos;
        lastY = ypos;
        return;
    }

    // 将当前鼠标位置和上次位置都映射到单位球面
    glm::vec3 currentPoint = screenToSphere(xpos, ypos);
    glm::vec3 lastPoint = screenToSphere(lastX, lastY);
    
    // 计算旋转轴（两点的叉积）
    glm::vec3 axis = glm::cross(lastPoint, currentPoint);
    
    // 如果轴长度很小，说明几乎没有移动
    if (glm::length(axis) < 0.001f) {
        lastX = xpos;
        lastY = ypos;
        return;
    }
    
    // 归一化旋转轴
    axis = glm::normalize(axis);
    
    // 计算旋转角度（两点的点积）
    float angle = acos(glm::clamp(glm::dot(lastPoint, currentPoint), -1.0f, 1.0f));
    
    // 应用灵敏度
    angle *= mouseSensitivity;
    
    // 创建旋转四元数
    glm::quat rotation = glm::angleAxis(angle, axis);
    
    // 更新当前旋转（组合旋转）
    currentRotation = rotation * currentRotation;
    
    // 归一化四元数避免累积误差
    currentRotation = glm::normalize(currentRotation);
    
    lastX = xpos;
    lastY = ypos;
}

// 鼠标按钮回调函数
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mousePressed = true;
            // 获取当前鼠标位置
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            lastX = xpos;
            lastY = ypos;
        }
        else if (action == GLFW_RELEASE) {
            mousePressed = false;
        }
    }
}

// 将屏幕坐标映射到单位球面上（轨迹球算法核心）
glm::vec3 screenToSphere(float x, float y) {
    // 将屏幕坐标转换到[-1, 1]范围
    float nx = (2.0f * x) / WINDOW_WIDTH - 1.0f;
    float ny = 1.0f - (2.0f * y) / WINDOW_HEIGHT; // Y轴翻转
    
    float length2 = nx * nx + ny * ny;
    
    glm::vec3 point;
    if (length2 <= 1.0f) {
        // 点在球面内部，计算Z坐标
        point.x = nx;
        point.y = ny;
        point.z = sqrt(1.0f - length2);
    } else {
        // 点在球面外部，投影到球面边缘
        float length = sqrt(length2);
        point.x = nx / length;
        point.y = ny / length;
        point.z = 0.0f;
    }
    
    return glm::normalize(point);
}
