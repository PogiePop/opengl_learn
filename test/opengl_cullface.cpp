#include <window.hpp>
#include <camera.hpp>
#include <mesh_utils.hpp>
#define RES_PATH "../../res/"


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
    Texture diff(RES_PATH "textures/diff.png", "diffuse", true);
    //创建立方体网格
    Mesh cube = CreateCubeMesh(1.0f, std::vector<Texture>{diff});

    Shader cull(RES_PATH "shaders/cullface.vert", nullptr, RES_PATH "shaders/cullface.frag");


        // 剔除面模式（默认GL_BACK）
    int current_cull_face = GL_BACK;
    // 正向面朝向（默认GL_CCW，逆时针）
    int current_front_face = GL_CCW;
    // 多边形渲染模式（默认GL_LINE，线条）
    int current_polygon_mode = GL_LINE;
    // ImGui下拉框选项文本（与枚举值对应）
    const char* cull_face_options[] = { "GL_BACK", "GL_FRONT", "GL_FRONT_AND_BACK" };
    const int cull_face_values[] = { GL_BACK, GL_FRONT, GL_FRONT_AND_BACK };
    const char* front_face_options[] = { "GL_CCW (逆时针)", "GL_CW (顺时针)" };
    const int front_face_values[] = { GL_CCW, GL_CW };
    const char* polygon_mode_options[] = { "GL_FILL (填充)", "GL_LINE (线条)" };
    const int polygon_mode_values[] = { GL_FILL, GL_LINE };
    
    _window.Run([&](GLFWwindow* window){
        glPolygonMode(GL_FRONT_AND_BACK, current_polygon_mode);
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //剔除背面
        glEnable(GL_CULL_FACE);
        // 设置正向面朝向
        glFrontFace(current_front_face);
        // 设置要剔除的面
        glCullFace(current_cull_face);
        cull.use();
        glm::mat4 model;
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
        cull.SetMat4("model", model);
        cull.SetMat4("view", view);
        cull.SetMat4("projection", projection);
        cube.Draw(cull);

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

         ImGui::Begin("剔除面测试面板"); // 窗口标题
        // 3.1 选择要剔除的面
        ImGui::Text("选择剔除面：");
        // 查找当前cull_face对应的索引（用于Combo显示）
        int cull_face_idx = 0;
        for (int i = 0; i < 3; i++) {
            if (cull_face_values[i] == current_cull_face) {
                cull_face_idx = i;
                break;
            }
        }
        if (ImGui::Combo("##cull_face", &cull_face_idx, cull_face_options, IM_ARRAYSIZE(cull_face_options))) {
            current_cull_face = cull_face_values[cull_face_idx];
        }

        // 3.2 选择正向面朝向
        ImGui::Text("正向面朝向：");
        int front_face_idx = (current_front_face == GL_CCW) ? 0 : 1;
        if (ImGui::Combo("##front_face", &front_face_idx, front_face_options, IM_ARRAYSIZE(front_face_options))) {
            current_front_face = front_face_values[front_face_idx];
        }

        // 3.3 选择渲染模式
        ImGui::Text("渲染模式：");
        int polygon_mode_idx = (current_polygon_mode == GL_FILL) ? 0 : 1;
        if (ImGui::Combo("##polygon_mode", &polygon_mode_idx, polygon_mode_options, IM_ARRAYSIZE(polygon_mode_options))) {
            current_polygon_mode = polygon_mode_values[polygon_mode_idx];
        }

        // 可选：显示当前状态提示
        ImGui::Separator();
        ImGui::Text("当前状态：");
        ImGui::Text("剔除面：%s", cull_face_options[cull_face_idx]);
        ImGui::Text("正向面：%s", front_face_options[front_face_idx]);
        ImGui::Text("渲染模式：%s", polygon_mode_options[polygon_mode_idx]);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });

    return 0;
}