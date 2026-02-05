#include <window.hpp>
#include <camera.hpp>
#include <mesh_utils.hpp>
#include <material.hpp>
#include <light.hpp>
#include <iostream>
#define RES_PATH "../../res/"

// 定义光照模式枚举（对应着色器state）
enum LightMode {
    MODE_PHONG = 0,          // 1. Phong光照
    MODE_BLINN_PHONG = 1     // 2. Blinn-Phong光照
};

int main()
{
    Window _window;
    Camera camera;

    // ========== 新增：交互控制变量 ==========
    LightMode current_light_mode = MODE_PHONG; // 默认Phong光照
    int state = 0;                             // 传递给着色器的状态（0=Phong，1=Blinn-Phong）
    // 点光源可交互参数（初始值与原代码一致）
    glm::vec3 light_pos = glm::vec3(1.0f, 1.0f, 1.0f);        // 光源位置
    glm::vec3 light_ambient = glm::vec3(0.2f, 0.1f, 0.05f);   // 环境光颜色
    glm::vec3 light_diffuse = glm::vec3(0.8f, 0.4f, 0.1f);    // 漫反射颜色
    glm::vec3 light_specular = glm::vec3(1.0f, 1.0f, 1.0f);   // 镜面光颜色
    // 衰减参数（可选调整，保留原默认值）
    float light_constant = 1.0f;
    float light_linear = 0.09f;
    float light_quadratic = 0.032f;

    
    _window.SetEventCallback(Window::WindowSizeCallback([](GLFWwindow* window, int width, int height){
        glViewport(0, 0, width, height);
    }));

    _window.SetEventCallback(Window::CursorPosCallback([&](GLFWwindow* window, float xOffset, float yOffset){
        camera.ProcessLensMove(xOffset, yOffset);
    }));

    _window.SetEventCallback(Window::ScrollPosCallback([&](GLFWwindow* window, float yOffset){
        camera.ProcessZoom(yOffset);
    }));

    // ========== 原有材质/纹理/网格加载 ==========
    Material mt{
        .diffuse = 0,
        .specular = 0,
        .shininess = 32.0f
    };

    // 点光源初始值（后续会被交互变量覆盖）
    PointLight pt{
        .position = light_pos,
        .ambient = light_ambient,
        .diffuse = light_diffuse,
        .specular = light_specular,
        .constant = light_constant,
        .linear = light_linear,
        .quadratic = light_quadratic
    };

    Texture diff(RES_PATH "textures/diff.png", "diffuse", true);
    Texture spec(RES_PATH "textures/spec.png", "specular", true);
    Shader sd(RES_PATH "shaders/blinn/1.vert", nullptr, RES_PATH "shaders/blinn/1.frag");
    Mesh cube = CreateCubeMesh(1.0f, std::vector<Texture>{diff, spec});

    // ========== 渲染循环 ==========
    _window.Run([&](GLFWwindow* window){
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ========== 1. 更新光照参数并传递到着色器 ==========
        sd.use();
        // 同步交互变量到点光源结构体
        pt.position = light_pos;
        pt.ambient = light_ambient;
        pt.diffuse = light_diffuse;
        pt.specular = light_specular;
        pt.constant = light_constant;
        pt.linear = light_linear;
        pt.quadratic = light_quadratic;
        // 传递光照模式状态到着色器
        state = (int)current_light_mode;
        sd.SetInt1("state", state);

        // 原有MVP矩阵和材质/光源参数传递
        glm::mat4 model;
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
        sd.SetMat4("model", model);
        sd.SetMat4("view", view);
        sd.SetMat4("projection", projection);
        mt.SetUniform("mt", sd);
        pt.SetUniform("pt", sd);
        sd.SetVec3("viewPos", camera.position);
        
        // 绘制立方体
        cube.Draw(sd);

        // ========== 2. 新增：ImGui交互面板 ==========
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        // 创建ImGui窗口
        ImGui::Begin("光照测试面板 (Phong/Blinn-Phong)");
        
        // 2.1 光照模式选择
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "💡 光照模式选择");
        ImGui::Separator();
        ImGui::RadioButton("1. Phong 光照", (int*)&current_light_mode, MODE_PHONG);
        ImGui::RadioButton("2. Blinn-Phong 光照", (int*)&current_light_mode, MODE_BLINN_PHONG);
        ImGui::Text("当前着色器状态值：%d", state);

        // 2.2 点光源位置调整
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "📍 点光源位置");
        ImGui::SliderFloat("X 轴", &light_pos.x, -10.0f, 10.0f, "%.1f");
        ImGui::SliderFloat("Y 轴", &light_pos.y, -10.0f, 10.0f, "%.1f");
        ImGui::SliderFloat("Z 轴", &light_pos.z, -10.0f, 10.0f, "%.1f");

        // 2.3 点光源颜色调整
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.0f, 0.8f, 1.0f, 1.0f), "🎨 光源颜色");
        ImGui::ColorEdit3("环境光 (Ambient)", &light_ambient.x);
        ImGui::ColorEdit3("漫反射 (Diffuse)", &light_diffuse.x);
        ImGui::ColorEdit3("镜面光 (Specular)", &light_specular.x);

        // 2.4 可选：衰减参数调整（高级选项）
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "🔌 衰减参数（可选）");
        ImGui::SliderFloat("常数项", &light_constant, 0.0f, 2.0f, "%.2f");
        ImGui::SliderFloat("一次项", &light_linear, 0.0f, 0.5f, "%.3f");
        ImGui::SliderFloat("二次项", &light_quadratic, 0.0f, 0.1f, "%.3f");

        // 2.5 模式说明
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.0f, 1.0f), "📝 模式说明");
        const char* mode_descriptions[] = {
            "Phong光照：直接计算视线与反射光的夹角（高光更锐利）",
            "Blinn-Phong光照：计算视线与半程向量的夹角（高光更柔和，性能更好）"
        };
        ImGui::TextWrapped("%s", mode_descriptions[current_light_mode]);

        ImGui::End(); // 结束ImGui窗口

        // 渲染ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });


    return 0;
}