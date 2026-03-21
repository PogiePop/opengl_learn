#include <window.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <camera.hpp>
#include <texture.hpp>
#include <time.hpp>
#include <light.hpp>
#include <material.hpp>

#define RES_PATH "../../res/"

// 立方体顶点数组：每个顶点 = 3(位置) + 2(纹理坐标) + 3(法线) （共8个浮点数）
float cubeVertices[] = {
    // 前侧面 (z=0.5) | 法线：(0.0f, 0.0f, 1.0f) 指向z轴正方向
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  // 左上
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  // 右上
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // 左下
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // 右下

    // 后侧面 (z=-0.5) | 法线：(0.0f, 0.0f, -1.0f) 指向z轴负方向（纹理坐标翻转避免镜像）
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f, // 左上
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f, // 右上
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f, // 左下
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f, // 右下

    // 左侧面 (x=-0.5) | 法线：(-1.0f, 0.0f, 0.0f) 指向x轴负方向
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f, // 左上
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f, // 右上
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, // 左下
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f, // 右下

    // 右侧面 (x=0.5) | 法线：(1.0f, 0.0f, 0.0f) 指向x轴正方向
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  // 左上
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,  // 右上
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // 左下
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // 右下

    // 上侧面 (y=0.5) | 法线：(0.0f, 1.0f, 0.0f) 指向y轴正方向
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  // 左上
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,  // 右上
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // 左下
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // 右下

    // 下侧面 (y=-0.5) | 法线：(0.0f, -1.0f, 0.0f) 指向y轴负方向
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f, // 左上
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f, // 右上
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f, // 左下
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f  // 右下
};

// 索引数组完全不变（36个索引，6面×2三角形）
unsigned int cubeIndices[] = {
    // 前侧
    0, 1, 2,  2, 3, 1,
    // 后侧
    4, 5, 6,  6, 7, 5,
    // 左侧
    8, 9,10, 10,11, 9,
    // 右侧
   12,13,14, 14,15,13,
    // 上侧
   16,17,18, 18,19,17,
    // 下侧
   20,21,22, 22,23,21
};

//灯泡
float PointVertices[] =
{
    -0.2f, 0.2f, 0.0f, 1.0f,
    0.2f, 0.2f, 1.0f, 1.0f,
    -0.2f, -0.2f, 0.0f, 0.0f,
    0.2f, -0.2f, 1.0f, 0.0f
};

unsigned int PointIndices[] =
{
    0, 1, 2, 2, 3, 1
};

int main()
{
    Window _window;
    Camera camera;
    _window.SetEventCallback(Window::WindowSizeCallback([](GLFWwindow* window, int width, int height){
        glViewport(0, 0, width, height);
    }));

    _window.SetEventCallback(Window::CursorPosCallback([&](GLFWwindow* window, float xOffset, float yOffset){
        camera.ProcessLensMove(xOffset, yOffset);
    }));

    _window.SetEventCallback(Window::ScrollPosCallback([&](GLFWwindow* window, float yOffset){
        camera.ProcessZoom(yOffset);
    }));

    unsigned int vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 5));
    glBindVertexArray(0);

    Shader cube(RES_PATH "shaders/sun.vert", nullptr, RES_PATH "shaders/sun.frag");
    Texture diff(RES_PATH "textures/diff.png");
    Texture spec(RES_PATH "textures/spec.png");
    
    // ========== 修改 1：修复 Material 赋值 ==========
    // 原代码：mt{0, 1, 32.0f} 错误，应该绑定实际的纹理ID
    Material mt{
        diff.GetID(),   // diffuse0 纹理ID（对应GL_TEXTURE0）
        spec.GetID(),   // specular0 纹理ID（对应GL_TEXTURE1）
        32.0f      // shininess高光指数
    };
    // ================================================

    PointLight pt{
        .position = glm::vec3(2.0f, 3.0f, 1.0f),  // 光源位置（右+上+前）
        .ambient = glm::vec3(0.2f, 0.1f, 0.05f), // 暖黄色环境光（弱）
        .diffuse = glm::vec3(0.8f, 0.4f, 0.1f),  // 橙黄色漫反射（主色调）
        .specular = glm::vec3(1.0f, 1.0f, 1.0f), // 白色镜面光（高光）
        .constant = 1.0f,                        // 衰减常数项固定1.0
        .linear = 0.09f,                         // 一次衰减系数（常用值）
        .quadratic = 0.032f                      // 二次衰减系数（适合中近距离光源）
    };

    //灯泡
    unsigned int ptVao, ptVbo, ptEbo;
    glGenVertexArrays(1, &ptVao);
    glGenBuffers(1, &ptVbo);
    glGenBuffers(1, &ptEbo);
    glBindVertexArray(ptVao);
    glBindBuffer(GL_ARRAY_BUFFER, ptVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PointVertices), PointVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ptEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(PointIndices), PointIndices, GL_STATIC_DRAW);
    
    // ========== 修改 2：修复灯泡顶点属性配置 ==========
    // 原代码顶点属性指针参数错误，修正size和offset
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
    // ================================================
    
    glBindVertexArray(0);

    Texture texPt(RES_PATH "textures/PointLight.png");
    Shader sdPt(RES_PATH "shaders/pt.vert", nullptr, RES_PATH "shaders/pt.frag");

    // ========== 修改 3：优化聚光灯参数定义 ==========
    // 新增角度变量，方便ImGui操作（内部自动转cos值）
    float spotInnerAngle = 12.5f;  // 内锥角（度数）
    float spotOuterAngle = 17.5f;  // 外锥角（度数）
    
    SpotLight spt{
        .position = camera.position,  
        .direction = camera.front,
        .ambient = glm::vec3(0.2f, 0.1f, 0.05f), 
        .diffuse = glm::vec3(0.8f, 0.4f, 0.1f),  
        .specular = glm::vec3(1.0f, 1.0f, 1.0f), 
        .constant = 1.0f,                        
        .linear = 0.09f,                         
        .quadratic = 0.032f,                      
        .innerCutOff = glm::cos(glm::radians(spotInnerAngle)),  // 用角度计算cos值
        .outerCutOff = glm::cos(glm::radians(spotOuterAngle))   // 用角度计算cos值
    };
    // ================================================

    //平行光
    ParallelLight prt{
        .direction = glm::vec3(0.0f, 2.0f, 1.0f),
        .ambient = glm::vec3(0.2f, 0.1f, 0.05f), // 暖黄色环境光（弱）
        .diffuse = glm::vec3(0.8f, 0.4f, 0.1f),  // 橙黄色漫反射（主色调）
        .specular = glm::vec3(1.0f, 1.0f, 1.0f), // 白色镜面光（高光）
    };
    
    _window.Run([&](GLFWwindow* window){
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 绘制灯泡
        sdPt.use();
        glBindVertexArray(ptVao);
        glm::mat4 model_pt = glm::translate(glm::mat4(1.0f), pt.position);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
        sdPt.SetMat4("model", model_pt);
        sdPt.SetMat4("view", view);
        sdPt.SetMat4("projection", projection);
        glActiveTexture(GL_TEXTURE0);
        texPt.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        
        // ========== 修改 4：重置GL状态，避免污染 ==========
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        // ================================================

        // 绘制立方体
        cube.use();
        glBindVertexArray(vao);
        glm::mat4 model;
       
        cube.SetMat4("model", model);
        cube.SetMat4("view", view);
        cube.SetMat4("projection", projection);
        cube.SetVec3("viewPos", camera.position);
        glActiveTexture(GL_TEXTURE0);
        diff.Bind();
        glActiveTexture(GL_TEXTURE1);
        spec.Bind();
        
        // ========== 修改 5：修正Material Uniform名称 ==========
        // 原代码：mt.SetUniform("mtr", cube); 着色器中是material，不是mtr
        mt.SetUniform("material", cube);
        // ================================================
        
        pt.SetUniform("plt", cube);
        
        // ========== 修改 6：更新聚光灯参数 ==========
        // 实时更新聚光灯位置和方向
        spt.position = camera.position;
        spt.direction = camera.front;
        // 实时转换角度为cos值
        spt.innerCutOff = glm::cos(glm::radians(spotInnerAngle));
        spt.outerCutOff = glm::cos(glm::radians(spotOuterAngle));
        // ================================================
        
        spt.SetUniform("splt", cube);
        prt.SetUniform("prlt", cube);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

        // ImGui界面
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        
        ImGui::Begin("imgui");
        ImGui::Text("hello imgui");
        ImGui::Text("PointLight Attribute:");
        ImGui::DragFloat3("->point position", &pt.position[0], 0.1f);
        ImGui::DragFloat3("->point ambient", &pt.ambient[0], 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat3("->point diffuse", &pt.diffuse[0], 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat3("->point specular", &pt.specular[0], 0.01f, 0.0f, 1.0f);
        ImGui::Separator();
        ImGui::Text("SpotLight Attribute:");
        ImGui::DragFloat3("->spot ambient", &spt.ambient[0], 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat3("->spot diffuse", &spt.diffuse[0], 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat3("->spot specular", &spt.specular[0], 0.01f, 0.0f, 1.0f);
        
        // ========== 修改 7：ImGui显示角度而非cos值 ==========
        // 原代码直接修改cos值，现在修改角度，更直观
        ImGui::DragFloat("->spot inner angle (deg)", &spotInnerAngle, 0.5f, 0.0f, 30.0f);
        ImGui::DragFloat("->spot outer angle (deg)", &spotOuterAngle, 0.5f, 0.0f, 45.0f);
        // ================================================
        
        ImGui::Separator();
        ImGui::Text("ParallelLight Attribute:");
        ImGui::DragFloat3("->parallel direction", &prt.direction[0], 0.1f);
        ImGui::DragFloat3("->parallel ambient", &prt.ambient[0], 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat3("->parallel diffuse", &prt.diffuse[0], 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat3("->parallel specular", &prt.specular[0], 0.01f, 0.0f, 1.0f);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });
    
    // ========== 修改 8：清理资源 ==========
    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &ptVao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ptVbo);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &ptEbo);
    // ================================================
    
    return 0;
}