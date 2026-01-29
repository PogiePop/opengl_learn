#include <window.hpp>
#include <camera.hpp>
#include <mesh_utils.hpp>
#include <model.hpp>
#define RES_PATH "../../res/"

// 定义测试模式枚举（便于管理）
enum TestMode {
    MODE_LINE = 0,        // 生成线
    MODE_HOUSE = 1,       // 生成房子
    MODE_EXPLODE = 2,     // 模型爆炸
    MODE_NORMAL_VIEW = 3  // 法线可视化
};

std::vector<glm::vec3> points = {
    {-0.5f, 0.5f, 0.0f},
    {0.5f, 0.5f, 0.0f},
    {-0.5f, -0.5f, 0.0f},
    {0.5f, -0.5f, 0.0f}
};

int main()
{
    Window _window;
    Camera camera;

    // ========== 新增：交互控制变量 ==========
    TestMode current_mode = MODE_LINE;          // 当前测试模式（默认生成线）
    float explode_magnitude = 1.0f;             // 爆炸程度（默认1.0）
    float normal_magnitude = 0.5f;              // 法线长度（默认0.5）
    ImVec4 normal_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // 法线颜色（默认红色）
    const float magnitude_min = 0.0f;
    const float magnitude_max = 5.0f;



    // ========== 原有事件回调 ==========
    _window.SetEventCallback(Window::WindowSizeCallback([](GLFWwindow* window, int width, int height){
        glViewport(0, 0, width, height);
    }));

    _window.SetEventCallback(Window::CursorPosCallback([&](GLFWwindow* window, float xOffset, float yOffset){
        camera.ProcessLensMove(xOffset, yOffset);
    }));

    _window.SetEventCallback(Window::ScrollPosCallback([&](GLFWwindow* window, float yOffset){
        camera.ProcessZoom(yOffset);
    }));

    // ========== 加载所有需要的着色器 ==========
    // 模型基础着色器
    Shader model_shader(RES_PATH "shaders/modelt.vert", nullptr, RES_PATH "shaders/modelt.frag");
    // 生成线：1.vert + 1.geom + 1.frag
    Shader line_shader(RES_PATH "shaders/geometry/1.vert", RES_PATH "shaders/geometry/1.geom", RES_PATH "shaders/geometry/1.frag");
    // 生成房子：1.vert + 2.geom + 1.frag
    Shader house_shader(RES_PATH "shaders/geometry/1.vert", RES_PATH "shaders/geometry/2.geom", RES_PATH "shaders/geometry/1.frag");
    // 模型爆炸：1.vert + explode.geom + 2.frag
    Shader explode_shader(RES_PATH "shaders/geometry/1.vert", RES_PATH "shaders/geometry/explode.geom", RES_PATH "shaders/geometry/2.frag");
    // 法线可视化：2.vert + nmvs.geom + nmvs.frag
    Shader normal_view_shader(RES_PATH "shaders/geometry/2.vert", RES_PATH "shaders/geometry/nmvs.geom", RES_PATH "shaders/geometry/nmvs.frag");

    // ========== 加载网格和模型 ==========
    Mesh pts = CreatePointMesh(points);          // 点网格（用于生成线/房子）
    Model liuyin(RES_PATH "textures/models/liuyin/琉音.pmx"); // 测试模型

    // ========== 渲染循环 ==========
    _window.Run([&](GLFWwindow* window){
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 通用MVP矩阵
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.zoom), 
            (float)_window.GetWidth() / _window.GetHeight(), 
            0.1f, 100.0f
        );

        // ========== 1. 根据模式切换着色器并绘制 ==========
        switch (current_mode) {
            case MODE_LINE: // 生成线
                line_shader.use();
                line_shader.SetMat4("model", model);
                line_shader.SetMat4("view", view);
                line_shader.SetMat4("projection", projection);
                pts.DrawPoint(); // 绘制点网格（几何着色器生成线）
                break;

            case MODE_HOUSE: // 生成房子
                house_shader.use();
                house_shader.SetMat4("model", model);
                house_shader.SetMat4("view", view);
                house_shader.SetMat4("projection", projection);
                pts.DrawPoint(); // 绘制点网格（几何着色器生成房子）
                break;

            case MODE_EXPLODE: // 模型爆炸
                explode_shader.use();
                explode_shader.SetMat4("model", model);
                explode_shader.SetMat4("view", view);
                explode_shader.SetMat4("projection", projection);
                explode_shader.SetFloat1("magnitude", explode_magnitude); // 传递爆炸程度
                explode_shader.SetFloat1("time", (float)glfwGetTime());
                liuyin.Draw(explode_shader); // 绘制模型（几何着色器实现爆炸）
                break;

            case MODE_NORMAL_VIEW: // 法线可视化
                normal_view_shader.use();
                normal_view_shader.SetMat4("model", model);
                normal_view_shader.SetMat4("view", view);
                normal_view_shader.SetMat4("projection", projection);
                // 传递法线参数
                normal_view_shader.SetFloat1("magnitude", normal_magnitude); // 法线长度
                normal_view_shader.SetVec3("color", glm::vec3(normal_color.x, normal_color.y, normal_color.z)); // 法线颜色
                liuyin.Draw(normal_view_shader); // 绘制模型（几何着色器显示法线）
                model_shader.use();
                model_shader.SetMat4("model", model);
                model_shader.SetMat4("view", view);
                model_shader.SetMat4("projection", projection);
                liuyin.Draw(model_shader);
                break;

            default: // 默认生成线
                line_shader.use();
                line_shader.SetMat4("model", model);
                line_shader.SetMat4("view", view);
                line_shader.SetMat4("projection", projection);
                pts.Draw(line_shader);
                break;
        }

        // ========== 2. 新增：ImGui交互面板 ==========
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        // 创建ImGui窗口
        ImGui::Begin("几何着色器测试面板");
        
        // 2.1 Radio按钮：切换测试模式
        ImGui::Text("选择测试模式：");
        ImGui::RadioButton("生成线", (int*)&current_mode, MODE_LINE);
        ImGui::RadioButton("生成房子", (int*)&current_mode, MODE_HOUSE);
        ImGui::RadioButton("模型爆炸", (int*)&current_mode, MODE_EXPLODE);
        ImGui::RadioButton("法线可视化", (int*)&current_mode, MODE_NORMAL_VIEW);

        // 2.2 根据模式显示可调参数
        ImGui::Separator();
        switch (current_mode) {
            case MODE_EXPLODE: // 模型爆炸：调节爆炸程度
                ImGui::Text("爆炸程度控制：");
                ImGui::SliderFloat(
                    "爆炸强度 (magnitude)", 
                    &explode_magnitude, 
                    magnitude_min, magnitude_max, 
                    "%.2f"
                );
                // 快速重置按钮
                if (ImGui::Button("重置爆炸强度")) {
                    explode_magnitude = 1.0f;
                }
                break;

            case MODE_NORMAL_VIEW: // 法线可视化：调节长度+颜色
                ImGui::Text("法线可视化控制：");
                // 法线长度
                ImGui::SliderFloat(
                    "法线长度 (magnitude)", 
                    &normal_magnitude, 
                    magnitude_min, magnitude_max, 
                    "%.2f"
                );
                // 法线颜色选择器
                ImGui::ColorEdit3("法线颜色", (float*)&normal_color);
                // 快速重置按钮
                if (ImGui::Button("重置法线参数")) {
                    normal_magnitude = 0.5f;
                    normal_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                }
                break;

            default: // 生成线/房子：无可调参数
                ImGui::Text("当前模式无可调参数");
                break;
        }

        // 2.3 显示当前状态（便于调试）
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "当前状态：");
        const char* mode_names[] = {"生成线", "生成房子", "模型爆炸", "法线可视化"};
        ImGui::Text("测试模式：%s", mode_names[current_mode]);
        if (current_mode == MODE_EXPLODE) {
            ImGui::Text("爆炸强度：%.2f", explode_magnitude);
        } else if (current_mode == MODE_NORMAL_VIEW) {
            ImGui::Text("法线长度：%.2f", normal_magnitude);
            ImGui::Text("法线颜色：(%.2f, %.2f, %.2f)", normal_color.x, normal_color.y, normal_color.z);
        }
        ImGui::Separator();
        if(ImGui::Button("重置位置")){
            camera.position = glm::vec3(0.0f, 0.0f, 3.0f);
        }

        ImGui::End(); // 结束ImGui窗口

        // 渲染ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });
    return 0;
}