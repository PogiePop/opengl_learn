# learnopengl

✨ LearnOpenGL 全功能实现项目，基于主流开源库构建，涵盖核心功能模块，完整覆盖官方文档所有核心内容。

---

## 一、依赖库及官方仓库地址

项目核心依赖库信息，可直接克隆集成至项目：

|库名称|功能描述|官方仓库地址|
|---|---|---|
|**GLFW**|窗口创建、输入事件（键盘/鼠标）处理|[https://github.com/glfw/glfw](https://github.com/glfw/glfw)|
|**GLAD**|OpenGL 核心函数动态加载器|[https://github.com/Dav1dde/glad](https://github.com/Dav1dde/glad)|
|**GLM**|OpenGL 专用数学库（矩阵、向量、变换运算）|[https://github.com/g-truc/glm](https://github.com/g-truc/glm)|
|**stbi_image**|轻量级多格式图像加载库（PNG/JPG/BMP等）|[https://github.com/nothings/stb](https://github.com/nothings/stb)|
|**Assimp**|跨平台3D模型导入库（支持obj/fbx/3ds等格式）|[https://github.com/assimp/assimp](https://github.com/assimp/assimp)|
---

## 二、核心实现工具模块

封装 LearnOpenGL 核心功能，模块解耦，便于扩展与维护：

|模块名称|功能描述|核心文件|
|---|---|---|
|**Window**|OpenGL 上下文初始化、窗口生命周期管理、帧缓冲大小监听、输入事件转发|`window.h` /`window.cpp`|
|**Camera**|FPS 风格摄像机控制、欧拉角与观察矩阵转换、鼠标/键盘视角交互、缩放调节|`camera.h` / `camera.cpp`|
|**Texture**|2D纹理/立方体贴图加载、纹理参数（过滤/环绕）配置、多纹理采样管理、Gamma校正支持|`texture.h` / `texture.cpp`|
|**Mesh**|顶点/索引/纹理数据封装、VAO/VBO/EBO缓冲管理、网格绘制接口封装|`mesh.h` / `mesh.cpp`|
|**Model**|基于Assimp递归加载3D模型、节点遍历解析、材质与纹理自动绑定、重复纹理规避|`model.h` / `model.cpp`|
|**Light**|方向光/点光源/聚光灯封装、光照参数Uniform传递、多光源混合计算、阴影效果支持|`light.h` / `light.cpp`|
---

## 三、LearnOpenGL 文档实现覆盖

全面覆盖 LearnOpenGL 官方文档（[https://learnopengl.com/](https://learnopengl.com/)）核心章节，实现从入门到进阶的全流程功能：

### 3.1 入门基础

- 窗口创建与 OpenGL 上下文初始化

- 着色器（顶点/片段/几何着色器）编译、链接与使用

- 纹理加载、采样及参数配置（过滤、环绕模式）

- 坐标系统与矩阵变换（模型/观察/投影矩阵）

### 3.2 光照系统

- 基础光照：环境光、漫反射光、镜面反射光计算

- 材质属性与光照贴图（漫反射贴图、镜面光贴图）

- 多光源混合计算（方向光+点光源+聚光灯组合）

- 高级光照：Gamma 校正、HDR、泛光效果实现

### 3.3 模型加载与渲染

- Assimp 库导入 3D 模型完整流程

- 模型节点递归遍历与网格数据解析

- 网格数据封装与缓冲对象（VAO/VBO/EBO）管理

- 多类型纹理材质绑定（漫反射、镜面反射、法线、高度图）

### 3.4 高级 OpenGL 特性

- 深度测试、模板测试、混合模式与面剔除

- 帧缓冲（FBO）与离屏渲染技术

- 立方体贴图与环境映射（天空盒、反射/折射）

- 阴影映射（Shadow Mapping）技术实现

### 3.5 基于物理的渲染（PBR）

- 金属粗糙工作流完整实现

- 图像基光照（IBL）与辐照度贴图

- 预过滤贴图与BRDF查找表

### 3.6 特效与后期处理

- 几何着色器与粒子系统实现

- 屏幕后处理：高斯模糊、边缘检测、色调映射

- 反走样技术（MSAA）应用

---

## 四、编译与运行说明

### 4.1 环境要求

- 编译器：支持 C++17 及以上标准（GCC/Clang/Visual Studio 2019+）

- 构建工具：CMake 3.16 及以上版本

- 显卡驱动：支持 OpenGL 3.3 及以上版本

### 4.2 编译步骤

```bash

# 克隆项目（替换为实际仓库地址）
git clone <项目仓库地址>
cd learnopengl

# 创建构建目录（推荐_out-of-source构建）
mkdir build && cd build

# 生成构建文件（根据系统自动适配）
cmake ..

# 编译项目（Linux/Mac）
make -j4  # -j4 表示4线程并行编译，可根据CPU核心数调整

# Windows系统：
# 用 Visual Studio 打开生成的 learnopengl.sln 文件
# 选择 Release 模式，点击生成解决方案
```

### 4.3 运行说明

1. 编译完成后，可执行文件位于 `build` 目录（Windows 位于 `build/Release`）。

2. 确保模型、纹理等资源文件路径与代码配置一致，避免资源加载失败。

3. 运行后通过键盘/鼠标控制摄像机视角：W/A/S/D 移动、鼠标拖动旋转视角、滚轮缩放。

> 💡 **提示**：建议配合 LearnOpenGL 官方文档逐章节调试，便于理解核心原理与实现细节。