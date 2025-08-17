# OpenGL 图形学学习环境

这是一个使用 Meson 构建系统配置的 OpenGL 图形学学习环境，集成了 GLFW、GLM 和 OpenGL 3.3。

## 环境要求

- macOS (M 芯片)
- Meson 构建系统
- GLFW3
- GLM (数学库)
- OpenGL 3.3+

## 安装依赖

### 使用 Homebrew 安装依赖

```bash
# 安装 GLFW
brew install glfw

# 安装 GLM (可选，meson 会自动下载)
brew install glm

# 如果需要 pkg-config
brew install pkg-config
```

## 构建项目

### 1. 配置构建目录

```bash
# 在项目根目录下
meson setup buildDir

# 或者指定调试模式
meson setup buildDir --buildtype=debug
```

### 2. 编译项目

```bash
# 编译
meson compile -C buildDir

# 或者使用 ninja
cd buildDir
ninja
```

### 3. 运行程序

```bash
# 从构建目录运行
./buildDir/Renderer

# 或者安装后运行
meson install -C buildDir
```

## 项目结构

```
Renderer/
├── main.cpp              # 主程序文件
├── meson.build          # Meson 构建配置
├── shaders/             # GLSL 着色器文件
│   ├── vertex.glsl      # 顶点着色器
│   ├── fragment.glsl    # 片段着色器
│   ├── simple_vertex.glsl
│   └── simple_fragment.glsl
├── subprojects/         # 外部依赖
│   └── glm.wrap        # GLM 依赖配置
└── buildDir/           # 构建输出目录
```

## 如何使用外部库

### 1. 系统依赖方式

在 `meson.build` 中使用 `dependency()` 函数：

```meson
# 查找系统安装的库
glfw_dep = dependency('glfw3', required : true)
opengl_dep = dependency('OpenGL', required : true)
```

### 2. Subproject 方式

对于 header-only 库如 GLM，可以使用 subproject：

```meson
# 如果系统没有找到，使用 subproject
if not glm_dep.found()
    glm_proj = subproject('glm')
    glm_dep = glm_proj.get_variable('glm_dep')
endif
```

### 3. 源码集成方式

如果你想直接使用源码进行调试：

1. 将源码放在 `third_party/` 目录下
2. 在 `meson.build` 中添加源码路径：

```meson
# 添加源码目录
inc_dirs = include_directories('third_party/glm')

# 创建可执行文件时包含头文件路径
executable('Renderer', 
    sources,
    include_directories : inc_dirs,
    dependencies : [opengl_dep, glfw_dep])
```

## 调试配置

### 1. 编译调试版本

```bash
# 配置调试模式
meson setup buildDir --buildtype=debug

# 或者重新配置现有构建目录
meson configure buildDir --buildtype=debug
```

### 2. 使用 LLDB 调试

```bash
# 编译调试版本
meson compile -C buildDir

# 使用 LLDB 调试
lldb ./buildDir/Renderer

# 在 LLDB 中设置断点
(lldb) breakpoint set --name main
(lldb) run
```

### 3. 使用 Xcode 调试

```bash
# 生成 Xcode 项目
meson setup buildDir --backend=xcode

# 打开 Xcode 项目
open buildDir/Renderer.xcodeproj
```

## GLSL 着色器开发

### 1. 内联着色器（当前示例）

在 C++ 代码中直接定义着色器源码，适合快速原型开发。

### 2. 外部文件着色器

创建 `.glsl` 文件，在运行时加载：

```cpp
// 读取着色器文件的辅助函数
std::string readShaderFile(const std::string& filepath) {
    std::ifstream file(filepath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// 使用
std::string vertexSource = readShaderFile("shaders/vertex.glsl");
std::string fragmentSource = readShaderFile("shaders/fragment.glsl");
```

### 3. 着色器热重载

可以实现文件监控，自动重新编译着色器：

```cpp
// 检查文件修改时间
bool isShaderModified(const std::string& filepath, time_t& lastModified) {
    struct stat fileStat;
    if (stat(filepath.c_str(), &fileStat) == 0) {
        if (fileStat.st_mtime > lastModified) {
            lastModified = fileStat.st_mtime;
            return true;
        }
    }
    return false;
}
```

## 常用调试技巧

### 1. OpenGL 错误检查

```cpp
void checkGLError(const std::string& operation) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error after " << operation << ": " << error << std::endl;
    }
}
```

### 2. 着色器编译错误检查

代码中已包含着色器编译错误检查，会输出详细的错误信息。

### 3. 矩阵和向量调试

```cpp
// 打印 GLM 矩阵
void printMatrix(const glm::mat4& mat) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << mat[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
```

## 学习建议

1. **从基础开始**：理解顶点缓冲区、着色器、变换矩阵
2. **逐步增加复杂度**：纹理 → 光照 → 阴影 → 后处理
3. **实验不同着色器**：修改 `shaders/` 目录下的文件进行实验
4. **使用调试工具**：RenderDoc、Nsight Graphics 等
5. **参考资源**：LearnOpenGL.com、OpenGL SuperBible

## 常见问题

### Q: 编译时找不到 GLFW
A: 确保已安装 GLFW：`brew install glfw`

### Q: 运行时黑屏
A: 检查 OpenGL 版本兼容性，macOS 需要 Core Profile

### Q: 着色器编译失败
A: 检查 GLSL 版本号，macOS 支持最高到 4.1

### Q: 如何添加新的外部库
A: 在 `meson.build` 中添加 `dependency()` 或创建新的 `.wrap` 文件
