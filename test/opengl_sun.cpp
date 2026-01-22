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
    Material mt{
        0, 1, 32.0f
    };
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
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
    glBindVertexArray(0);

    Texture texPt(RES_PATH "textures/PointLight.png");
    Shader sdPt(RES_PATH "shaders/pt.vert", nullptr, RES_PATH "shaders/pt.frag");

    //聚光灯
    SpotLight spt{
        .position = camera.position,  // 光源位置（右+上+前）
        .direction = camera.front,
        .ambient = glm::vec3(0.2f, 0.1f, 0.05f), // 暖黄色环境光（弱）
        .diffuse = glm::vec3(0.8f, 0.4f, 0.1f),  // 橙黄色漫反射（主色调）
        .specular = glm::vec3(1.0f, 1.0f, 1.0f), // 白色镜面光（高光）
        .constant = 1.0f,                        // 衰减常数项固定1.0
        .linear = 0.09f,                         // 一次衰减系数（常用值）
        .quadratic = 0.032f,                      // 二次衰减系数（适合中近距离光源）
        .innerCutOff = glm::cos(glm::radians(12.5f)),
        .outerCutOff = glm::cos(glm::radians(17.5f))
    };

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
        mt.SetUniform("mtr", cube);
        pt.SetUniform("plt", cube);
        spt.position = camera.position;
        spt.direction = camera.front;
        spt.SetUniform("splt", cube);
        prt.SetUniform("prlt", cube);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
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
        ImGui::DragFloat("->spot innerCutOff", &spt.innerCutOff, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("->spot outerCutOff", &spt.outerCutOff, 0.01f, 0.0f, 1.0f);
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
    return 0;
}