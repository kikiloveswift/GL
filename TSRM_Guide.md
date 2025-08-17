# TSRM 矩阵变换详解

## 📖 概述

TSRM 是 3D 图形学中最基础也是最重要的概念之一，代表了四种基本的几何变换：
- **T** - Translation（平移）
- **S** - Scale（缩放）
- **R** - Rotation（旋转）
- **M** - Model（模型矩阵）

这些变换的组合构成了模型矩阵（Model Matrix），用于将物体从本地坐标系变换到世界坐标系。

## 🔢 数学基础

### 齐次坐标系

在 3D 图形学中，我们使用 4×4 矩阵和齐次坐标来表示变换：

```
点: (x, y, z, 1)
向量: (x, y, z, 0)
```

### 变换矩阵的组合顺序

**关键概念**：矩阵乘法不满足交换律！变换的顺序非常重要。

标准的变换顺序是：**M = T × R × S**

```
最终矩阵 = 平移矩阵 × 旋转矩阵 × 缩放矩阵
```

## 🎯 各种变换详解

### 1. 缩放变换 (Scale)

缩放变换改变物体的大小，沿着各个轴独立进行。

#### 数学表示

```
S = [sx  0   0   0 ]
    [0   sy  0   0 ]
    [0   0   sz  0 ]
    [0   0   0   1 ]
```

#### 特点
- `sx, sy, sz` 分别是 X、Y、Z 轴的缩放因子
- `sx = sy = sz = 1`：无缩放
- `sx = sy = sz > 1`：等比放大
- `sx = sy = sz < 1`：等比缩小
- 不同轴使用不同缩放因子：非等比缩放

#### 示例
```cpp
// GLM 代码示例
glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.5f, 0.5f));
// X轴放大2倍，Y轴放大1.5倍，Z轴缩小到0.5倍
```

### 2. 旋转变换 (Rotation)

旋转变换围绕某个轴旋转物体。

#### 绕 X 轴旋转
```
Rx(θ) = [1   0      0     0]
        [0   cos(θ) -sin(θ) 0]
        [0   sin(θ)  cos(θ) 0]
        [0   0       0      1]
```

#### 绕 Y 轴旋转
```
Ry(θ) = [cos(θ)  0  sin(θ)  0]
        [0       1  0       0]
        [-sin(θ) 0  cos(θ)  0]
        [0       0  0       1]
```

#### 绕 Z 轴旋转
```
Rz(θ) = [cos(θ) -sin(θ) 0  0]
        [sin(θ)  cos(θ) 0  0]
        [0       0      1  0]
        [0       0      0  1]
```

#### 欧拉角组合

组合旋转通常使用欧拉角：**R = Rz(γ) × Ry(β) × Rx(α)**

```cpp
// GLM 代码示例
glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(1, 0, 0));
glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0, 1, 0));
glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(60.0f), glm::vec3(0, 0, 1));
glm::mat4 rotation = rotationZ * rotationY * rotationX;
```

### 3. 平移变换 (Translation)

平移变换移动物体的位置。

#### 数学表示
```
T = [1  0  0  tx]
    [0  1  0  ty]
    [0  0  1  tz]
    [0  0  0  1 ]
```

#### 特点
- `tx, ty, tz` 分别是 X、Y、Z 轴的平移距离
- 平移不改变物体的形状和大小
- 只改变物体的位置

#### 示例
```cpp
// GLM 代码示例
glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, -2.0f, 3.0f));
// 向X轴正方向移动5单位，Y轴负方向移动2单位，Z轴正方向移动3单位
```

## ⚡ 变换顺序的重要性

### 为什么是 T × R × S？

1. **缩放 (S) 首先应用**：
   - 在物体的本地坐标系中进行
   - 不会影响物体的位置和方向

2. **旋转 (R) 其次应用**：
   - 围绕原点旋转已缩放的物体
   - 此时物体仍在原点附近

3. **平移 (T) 最后应用**：
   - 将旋转后的物体移动到最终位置
   - 不会影响物体的形状和方向

### 错误顺序的后果

如果使用 **S × R × T** 顺序：
- 平移会先执行，物体移动到新位置
- 然后旋转会围绕原点旋转，导致物体"绕圈"
- 最后缩放可能会产生意外的效果

## 🎮 实践示例

### 完整的变换流程

```cpp
// 1. 定义变换参数
glm::vec3 translation = glm::vec3(5.0f, 2.0f, -3.0f);
glm::vec3 rotation = glm::vec3(45.0f, 30.0f, 60.0f);  // 度数
glm::vec3 scale = glm::vec3(2.0f, 1.5f, 0.8f);

// 2. 创建各个变换矩阵
glm::mat4 T = glm::translate(glm::mat4(1.0f), translation);

glm::mat4 Rx = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0));
glm::mat4 Ry = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0));
glm::mat4 Rz = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));
glm::mat4 R = Rz * Ry * Rx;

glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);

// 3. 组合成模型矩阵
glm::mat4 M = T * R * S;

// 4. 应用到顶点
glm::vec4 vertex = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);  // 原始顶点
glm::vec4 transformedVertex = M * vertex;  // 变换后的顶点
```

## 🔍 调试技巧

### 1. 分步验证

```cpp
// 分别应用每个变换，观察结果
glm::vec4 original = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
glm::vec4 scaled = S * original;
glm::vec4 rotated = R * scaled;
glm::vec4 translated = T * rotated;

std::cout << "原始: " << glm::to_string(original) << std::endl;
std::cout << "缩放后: " << glm::to_string(scaled) << std::endl;
std::cout << "旋转后: " << glm::to_string(rotated) << std::endl;
std::cout << "平移后: " << glm::to_string(translated) << std::endl;
```

### 2. 矩阵可视化

```cpp
void printMatrix(const std::string& name, const glm::mat4& mat) {
    std::cout << name << ":\n";
    for (int i = 0; i < 4; i++) {
        std::cout << "[ ";
        for (int j = 0; j < 4; j++) {
            std::cout << std::fixed << std::setprecision(3) 
                      << std::setw(8) << mat[j][i] << " ";
        }
        std::cout << "]\n";
    }
    std::cout << std::endl;
}
```

## 🎯 常见应用场景

### 1. 游戏物体变换
- 角色移动：平移
- 角色转向：Y轴旋转
- 角色缩放：等比缩放

### 2. 动画制作
- 关键帧之间的插值
- 骨骼动画的变换层次

### 3. 物理模拟
- 刚体的位置和方向更新
- 碰撞检测前的坐标变换

## 📚 进阶概念

### 1. 变换层次结构
- 父子关系的变换传递
- 场景图的构建

### 2. 四元数旋转
- 避免万向节锁问题
- 更平滑的旋转插值

### 3. 逆变换
- 从世界坐标回到本地坐标
- 光线追踪中的应用

## 🛠️ Demo 使用指南

运行我们的 TSRM Demo 来实践这些概念：

```bash
./buildDir/TSRM_Demo
```

### 控制说明
- **1-4**: 选择不同的立方体
- **WASD + QE**: 平移控制
- **方向键 + ZC**: 旋转控制
- **+/-**: 缩放控制
- **M**: 显示矩阵信息
- **A**: 动画模式
- **R**: 重置当前物体

### 学习建议
1. 先单独测试每种变换
2. 观察变换顺序的影响
3. 使用矩阵显示功能理解数学原理
4. 尝试组合不同的变换

## 🔗 相关资源

- [LearnOpenGL - Transformations](https://learnopengl.com/Getting-started/Transformations)
- [GLM Documentation](https://glm.g-truc.net/0.9.9/index.html)
- [3D Math Primer for Graphics and Game Development](https://gamemath.com/)

---

**记住**：理解 TSRM 是掌握 3D 图形学的关键第一步。通过实践和实验，你会逐渐建立起对 3D 变换的直觉理解！
