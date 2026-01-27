#include <window.hpp>
#include <camera.hpp>
#include <mesh_utils.hpp>
#include <model.hpp>
#define RES_PATH "../../res/"

std::vector<std::string> skyboxPaths = {
    RES_PATH "textures/skybox/right.jpg",
    RES_PATH "textures/skybox/left.jpg",
    RES_PATH "textures/skybox/top.jpg",
    RES_PATH "textures/skybox/bottom.jpg",
    RES_PATH "textures/skybox/front.jpg",
    RES_PATH "textures/skybox/back.jpg",
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

    //加载天空盒
    Texture skt(skyboxPaths);
    Mesh cube = CreateCubeMesh(1.0f, std::vector<Texture>{skt});
    Shader skybox(RES_PATH "shaders/skybox/skyboxt.vert", nullptr, RES_PATH "shaders/skybox/skyboxt.frag");

    //加载模型
    Model liuyin(RES_PATH "textures/models/liuyin/琉音.pmx");
    Shader md(RES_PATH "shaders/skybox/model.vert", nullptr, RES_PATH "shaders/skybox/model.frag");

    int is_refract = 1;          // 1=折射，0=反射（默认折射）
    float refract_ratio = 1.0f / 1.52f; // 折射系数（默认空气→玻璃）
    const float ratio_min = 0.0f;       // 折射系数最小值
    const float ratio_max = 2.0f;       // 折射系数最大值
    
    _window.Run([&](GLFWwindow* window){
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        skybox.use();
        glm::mat4 model;
        glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
        skybox.SetMat4("model", model);
        skybox.SetMat4("view", view);
        skybox.SetMat4("projection", projection);
        glDepthFunc(GL_LEQUAL);
        cube.DrawCubeMap(skybox);
        md.use();
        glm::mat4 view1 = camera.GetViewMatrix();
        md.SetMat4("model", model);
        md.SetMat4("view", view1);
        md.SetMat4("projection", projection);
        md.SetVec3("viewPos", camera.position);
        md.SetInt1("isRefract", is_refract);       // 传递折射/反射模式
        md.SetFloat1("ratio", refract_ratio);      // 传递折射系数
        liuyin.Draw(md);

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        // 创建ImGui窗口
        ImGui::Begin("折射/反射控制面板");
        
        // 3.1 Radio按钮：切换折射/反射
        ImGui::Text("渲染模式：");
        ImGui::RadioButton("折射 (Refract)", &is_refract, 1);
        ImGui::SameLine(); // 同行显示，更紧凑
        ImGui::RadioButton("反射 (Reflect)", &is_refract, 0);

        // 3.2 滑块：调整折射系数（仅在折射模式下显示）
        if (is_refract == 1) {
            ImGui::Separator();
            ImGui::Text("折射系数 (Ratio)：");
            // 滑块+输入框，实时调整
            ImGui::SliderFloat(
                "##ratio", 
                &refract_ratio, 
                ratio_min, ratio_max, 
                "%.4f" // 显示4位小数
            );
            // 可选：快速恢复默认值
            if (ImGui::Button("恢复默认 (1.0/1.52)")) {
                refract_ratio = 1.0f / 1.52f;
            }
        }

        // 3.3 显示当前状态（便于调试）
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "当前状态：");
        ImGui::Text("模式：%s", is_refract ? "折射" : "反射");
        if (is_refract == 1) {
            ImGui::Text("折射系数：%.4f", refract_ratio);
        }

        ImGui::End(); // 结束ImGui窗口

        // 渲染ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });
    return 0;
}