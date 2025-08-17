#define GL_SILENCE_DEPRECATION
#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <iomanip>
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// 窗口大小
const unsigned int SCR_WIDTH = 1400;
const unsigned int SCR_HEIGHT = 800;

// 变换参数
struct Transform {
    glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);    // 欧拉角 (度)
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
};

// 全局变换状态
Transform transform;
bool showMatrix = false;
bool animationMode = false;
int currentOrder = 0; // 0: T*R*S, 1: R*S*T, 2: S*R*T, 3: T*S*R, 4: R*T*S, 5: S*T*R

const char* orderNames[] = {
    "T × R × S (标准)",
    "R × S × T (旋转优先)",
    "S × R × T (缩放优先)", 
    "T × S × R (平移优先)",
    "R × T × S (旋转-平移-缩放)",
    "S × T × R (缩放-平移-旋转)"
};

// 顶点着色器源码
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertexColor = aColor;
}
)";

// 片段着色器源码
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 vertexColor;

uniform float alpha;

void main()
{
    FragColor = vec4(vertexColor, alpha);
}
)";

// 立方体顶点数据（位置 + 颜色）
float cubeVertices[] = {
    // 前面 (红色)
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

    // 后面 (绿色)
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,

    // 左面 (蓝色)
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

    // 右面 (黄色)
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

    // 底面 (紫色)
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,

    // 顶面 (青色)
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f
};

// 错误回调函数
void error_callback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

// 窗口大小改变回调
void framebuffer_size_callback(GLFWwindow* /* window */, int width, int height) {
    glViewport(0, 0, width, height);
}

// 键盘输入处理
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    // 切换变换顺序 (1-6)
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) currentOrder = 0;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) currentOrder = 1;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) currentOrder = 2;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) currentOrder = 3;
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) currentOrder = 4;
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) currentOrder = 5;
    
    // 显示矩阵信息
    static bool mPressed = false;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !mPressed) {
        showMatrix = !showMatrix;
        mPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) {
        mPressed = false;
    }
    
    // 动画模式切换
    static bool aPressed = false;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !aPressed) {
        animationMode = !animationMode;
        aPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
        aPressed = false;
    }
    
    // 重置变换
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        transform = Transform();
    }
    
    float speed = 0.02f;
    
    // 平移控制 (WASD + QE)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) transform.translation.y += speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) transform.translation.y -= speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) transform.translation.x -= speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) transform.translation.x += speed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) transform.translation.z += speed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) transform.translation.z -= speed;
    
    // 旋转控制 (方向键 + Z/C)
    float rotSpeed = 2.0f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) transform.rotation.x += rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) transform.rotation.x -= rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) transform.rotation.y += rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) transform.rotation.y -= rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) transform.rotation.z += rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) transform.rotation.z -= rotSpeed;
    
    // 缩放控制 (+ -)
    float scaleSpeed = 0.01f;
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
        transform.scale += scaleSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        transform.scale -= scaleSpeed;
        // 防止负缩放
        transform.scale.x = std::max(0.1f, transform.scale.x);
        transform.scale.y = std::max(0.1f, transform.scale.y);
        transform.scale.z = std::max(0.1f, transform.scale.z);
    }
}

// 编译着色器
unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    return shader;
}

// 打印矩阵
void printMatrix(const std::string& name, const glm::mat4& mat) {
    std::cout << "\n" << name << ":\n";
    for (int i = 0; i < 4; i++) {
        std::cout << "[ ";
        for (int j = 0; j < 4; j++) {
            std::cout << std::fixed << std::setprecision(3) << std::setw(8) << mat[j][i] << " ";
        }
        std::cout << "]\n";
    }
}

// 创建变换矩阵（根据不同顺序）
glm::mat4 createModelMatrix(const Transform& t, int order) {
    // 创建基础变换矩阵
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), t.translation);
    
    glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(t.rotation.x), glm::vec3(1, 0, 0));
    glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(t.rotation.y), glm::vec3(0, 1, 0));
    glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(t.rotation.z), glm::vec3(0, 0, 1));
    glm::mat4 rotation = rotationZ * rotationY * rotationX;
    
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), t.scale);
    
    glm::mat4 model;
    
    // 根据顺序组合矩阵
    switch(order) {
        case 0: // T × R × S (标准)
            model = translation * rotation * scale;
            break;
        case 1: // R × S × T
            model = rotation * scale * translation;
            break;
        case 2: // S × R × T
            model = scale * rotation * translation;
            break;
        case 3: // T × S × R
            model = translation * scale * rotation;
            break;
        case 4: // R × T × S
            model = rotation * translation * scale;
            break;
        case 5: // S × T × R
            model = scale * translation * rotation;
            break;
        default:
            model = translation * rotation * scale;
            break;
    }
    
    // 可选：打印矩阵信息
    if (showMatrix) {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "当前变换顺序: " << orderNames[order] << std::endl;
        std::cout << "平移: (" << t.translation.x << ", " << t.translation.y << ", " << t.translation.z << ")";
        std::cout << "\n旋转: (" << t.rotation.x << "°, " << t.rotation.y << "°, " << t.rotation.z << "°)";
        std::cout << "\n缩放: (" << t.scale.x << ", " << t.scale.y << ", " << t.scale.z << ")";
        
        printMatrix("平移矩阵 (T)", translation);
        printMatrix("旋转矩阵 (R)", rotation);
        printMatrix("缩放矩阵 (S)", scale);
        printMatrix("最终模型矩阵", model);
        
        // 显示一个测试点的变换结果
        glm::vec4 testPoint(1.0f, 0.0f, 0.0f, 1.0f);
        glm::vec4 result = model * testPoint;
        std::cout << "\n测试点 (1,0,0,1) 变换后: (" 
                  << result.x << ", " << result.y << ", " << result.z << ", " << result.w << ")" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
    }
    
    return model;
}

void printInstructions() {
    std::cout << "\n" << std::string(70, '=') << std::endl;
    std::cout << "           变换顺序对比 Demo - 控制说明" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    std::cout << "变换顺序选择:" << std::endl;
    std::cout << "  1       : T × R × S (标准顺序)" << std::endl;
    std::cout << "  2       : R × S × T (旋转优先)" << std::endl;
    std::cout << "  3       : S × R × T (缩放优先)" << std::endl;
    std::cout << "  4       : T × S × R (平移优先)" << std::endl;
    std::cout << "  5       : R × T × S (旋转-平移-缩放)" << std::endl;
    std::cout << "  6       : S × T × R (缩放-平移-旋转)" << std::endl;
    std::cout << "\n平移控制 (Translation):" << std::endl;
    std::cout << "  W/S     : Y轴 上/下移动" << std::endl;
    std::cout << "  A/D     : X轴 左/右移动" << std::endl;
    std::cout << "  Q/E     : Z轴 前/后移动" << std::endl;
    std::cout << "\n旋转控制 (Rotation):" << std::endl;
    std::cout << "  ↑/↓     : X轴旋转 (俯仰)" << std::endl;
    std::cout << "  ←/→     : Y轴旋转 (偏航)" << std::endl;
    std::cout << "  Z/C     : Z轴旋转 (翻滚)" << std::endl;
    std::cout << "\n缩放控制 (Scale):" << std::endl;
    std::cout << "  +/-     : 整体缩放" << std::endl;
    std::cout << "\n其他控制:" << std::endl;
    std::cout << "  R       : 重置变换" << std::endl;
    std::cout << "  M       : 切换矩阵信息显示" << std::endl;
    std::cout << "  A       : 切换动画模式" << std::endl;
    std::cout << "  ESC     : 退出程序" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    std::cout << "建议：先设置一些变换参数，然后切换不同的顺序观察差异！" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
}

int main() {
    // 打印使用说明
    printInstructions();
    
    // 设置错误回调
    glfwSetErrorCallback(error_callback);
    
    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // 配置 GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "变换顺序对比 Demo", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // 编译着色器
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    
    // 创建着色器程序
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // 检查链接错误
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // 创建 VAO, VBO
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    // 启用深度测试和混合
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // 设置初始变换参数（便于观察差异）
    transform.translation = glm::vec3(2.0f, 1.0f, 0.0f);
    transform.rotation = glm::vec3(30.0f, 45.0f, 0.0f);
    transform.scale = glm::vec3(1.5f, 0.8f, 1.2f);
    
    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        
        // 设置视图和投影矩阵
        glm::mat4 view = glm::lookAt(
            glm::vec3(8.0f, 6.0f, 8.0f),  // 相机位置
            glm::vec3(0.0f, 0.0f, 0.0f),  // 看向原点
            glm::vec3(0.0f, 1.0f, 0.0f)   // 上方向
        );
        
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f), 
            (float)SCR_WIDTH / (float)SCR_HEIGHT, 
            0.1f, 100.0f
        );
        
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        // 动画模式
        if (animationMode) {
            float time = glfwGetTime();
            transform.rotation.y = time * 30.0f;
            transform.translation.x = sin(time) * 3.0f;
            transform.scale = glm::vec3(1.0f + sin(time * 2.0f) * 0.5f);
        }
        
        // 绘制立方体
        glBindVertexArray(VAO);
        
        glm::mat4 model = createModelMatrix(transform, currentOrder);
        
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        
        unsigned int alphaLoc = glGetUniformLocation(shaderProgram, "alpha");
        glUniform1f(alphaLoc, 0.8f);
        
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        // 在屏幕上显示当前顺序信息
        glfwSetWindowTitle(window, (std::string("变换顺序对比 Demo - 当前: ") + orderNames[currentOrder]).c_str());
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // 清理资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    
    glfwTerminate();
    return 0;
}
