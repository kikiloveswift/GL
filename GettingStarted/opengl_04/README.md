# OpenGL 04 - 3D纹理正方体与坐标变换

这个项目演示了OpenGL中的3D坐标变换，包括：

## 功能特性

1. **3D正方体渲染**
   - 使用VAO、VBO、EBO管理顶点数据
   - 每个面都贴上相同的纹理贴图
   - 启用深度测试确保正确的3D渲染

2. **多纹理映射**
   - 使用`ImageLoader`类加载6个不同的PNG纹理
   - 正方体的每个面使用不同的纹理贴图：
     - 前面：Gemini_Generated_Image_nxkhggnxkhggnxkh1.png
     - 后面：Gemini_Generated_Image_nxkhggnxkhggnxkh2.png
     - 左面：Gemini_Generated_Image_nxkhggnxkhggnxkh3.png
     - 右面：Gemini_Generated_Image_nxkhggnxkhggnxkh4.png
     - 上面：Gemini_Generated_Image_nxkhggnxkhggnxkh5.png
     - 下面：Gemini_Generated_Image_nxkhggnxkhggnxkh6.png

3. **MVP矩阵变换**
   - **Model矩阵**: 控制物体的旋转
   - **View矩阵**: 设置摄像机位置
   - **Projection矩阵**: 透视投影

4. **键盘交互控制**
   - `W`: 向上旋转（绕X轴负方向）
   - `S`: 向下旋转（绕X轴正方向）
   - `A`: 向左旋转（绕Y轴负方向）
   - `D`: 向右旋转（绕Y轴正方向）
   - `ESC`: 退出程序

## 技术要点

### 顶点数据结构
每个顶点包含：
- 位置坐标 (x, y, z)
- 纹理坐标 (u, v)
- 面ID (0-5，用于选择对应的纹理)

### Shader程序
- **Vertex Shader**: 处理MVP矩阵变换，传递纹理坐标和面ID
- **Fragment Shader**: 根据面ID选择对应的纹理进行采样

### 坐标系统
- 使用右手坐标系
- Z轴正方向指向观察者
- 透视投影，视野角度45度

## 编译和运行

```bash
# 编译
cmake --build cmake-build-debug --target opengl_04

# 运行
./cmake-build-debug/opengl_04
```

## 依赖库

- GLFW: 窗口管理和输入处理
- GLAD: OpenGL函数加载
- GLM: 数学库（向量和矩阵运算）
- STB_IMAGE: 图像加载（通过ImageLoader封装）

## 学习要点

1. **坐标变换管道**: 理解从局部坐标到屏幕坐标的变换过程
2. **矩阵运算**: 学习如何使用GLM进行矩阵计算
3. **多纹理技术**: 掌握如何为不同面使用不同纹理
4. **顶点属性**: 学习如何传递自定义属性（面ID）到shader
5. **纹理数组**: 理解如何在shader中使用纹理数组
6. **深度测试**: 理解Z-buffer的作用
7. **实时交互**: 学习如何处理用户输入并更新渲染状态
