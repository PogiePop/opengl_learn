#include <window.hpp>
#include <camera.hpp>
#include <model.hpp>
#include <mesh_utils.hpp>
// 新增：性能统计所需头文件
#include <chrono>
#include <unordered_map>
#include <iomanip>
#define RES_PATH "../../res/"

// 定义实例化绘制测试模式枚举
enum InstanceTestMode {
    MODE_NORMAL_DRAW = 0,          // 普通绘制
    MODE_INSTANCE_UNIFORM = 1,     // 实例绘制（Uniform传递偏移）
    MODE_INSTANCE_GPU = 2,         // 实例绘制（直接加载到GPU）
    MODE_INSTANCE_MODEL = 3        // 实例绘制模型
};



// 原有绘制函数声明
inline void NormalDraw(Window&, Camera&, Mesh&, Shader&);
inline void InstanceDraw1(Window&, Camera&, Mesh&, Shader&);
inline void InstanceDraw2(Window&, Camera&, Mesh&, Shader&);
inline void InstanceDraw3(Window&, Camera&, Model&, Shader&);

int main()
{
    Window _window;
    Camera camera;

    // ========== 新增：交互控制变量 ==========
    InstanceTestMode current_mode = MODE_INSTANCE_MODEL; // 默认模型实例绘制

    // ========== 新增：性能统计变量 ==========
    double last_time = glfwGetTime();    // 上一帧时间
    int frame_count = 0;                 // 帧计数
    float fps = 0.0f;                    // 当前帧率
    float frame_time = 0.0f;             // 当前帧耗时（ms）
    // 记录各模式的绘制耗时（ms）
    std::unordered_map<InstanceTestMode, float> mode_draw_time;
    // 性能统计临时变量
    std::chrono::high_resolution_clock::time_point draw_start, draw_end;



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

    // ========== 原有数据准备 ==========
    std::vector<glm::vec3> offsets;
    std::vector<glm::vec3> colors = {
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.5f}
    };

    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            offsets.push_back(glm::vec3(i*10.0, j*10.0, 0.0));
        }
    }
    std::vector<InstanceData> instances;
    for(const auto elem : offsets) instances.push_back({elem});

    // ========== 加载网格和着色器 ==========
    Mesh quad = CreateQuadMesh();
    Shader sd(RES_PATH "shaders/instance/1.vert", nullptr, RES_PATH "shaders/instance/1.frag");
    Shader sd2(RES_PATH "shaders/instance/2.vert", nullptr, RES_PATH "shaders/instance/1.frag");
    Shader sd3(RES_PATH "shaders/instance/3.vert", nullptr, RES_PATH "shaders/instance/2.frag");
    Shader md(RES_PATH "shaders/instance/3.vert", nullptr, RES_PATH "shaders/instance/md.frag");
    Mesh ins_quad = CreateQuadMeshInstance(1.0f, _empty, colors, offsets);
    Model liuyin(RES_PATH "textures/models/liuyin/琉音.pmx", instances);

    // ========== 渲染循环 ==========
    _window.Run([&](GLFWwindow* window){
        // ========== 新增：性能统计 - 计算FPS和帧耗时 ==========
        double current_time = glfwGetTime();
        frame_count++;
        // 每1秒更新一次FPS
        if (current_time - last_time >= 1.0) {
            fps = (float)frame_count / (float)(current_time - last_time);
            last_time = current_time;
            frame_count = 0;
        }
        // 记录当前帧开始时间（用于计算帧耗时）
        auto frame_start = std::chrono::high_resolution_clock::now();

        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ========== 1. 根据选中模式调用对应绘制函数（新增耗时统计） ==========
        draw_start = std::chrono::high_resolution_clock::now(); // 绘制开始计时
        switch (current_mode) {
            case MODE_NORMAL_DRAW: // 普通绘制
                NormalDraw(_window, camera, quad, sd);
                break;

            case MODE_INSTANCE_UNIFORM: // 实例绘制（Uniform）
                InstanceDraw1(_window, camera, quad, sd2);
                break;

            case MODE_INSTANCE_GPU: // 实例绘制（GPU加载）
                InstanceDraw2(_window, camera, ins_quad, sd3);
                break;

            case MODE_INSTANCE_MODEL: // 实例绘制模型
                InstanceDraw3(_window, camera, liuyin, md);
                break;

            default: // 默认模型实例绘制
                InstanceDraw3(_window, camera, liuyin, md);
                break;
        }
        draw_end = std::chrono::high_resolution_clock::now(); // 绘制结束计时
        // 计算当前模式绘制耗时（转换为毫秒）
        mode_draw_time[current_mode] = std::chrono::duration_cast<std::chrono::microseconds>(draw_end - draw_start).count() / 1000.0f;

        // ========== 计算当前帧总耗时 ==========
        auto frame_end = std::chrono::high_resolution_clock::now();
        frame_time = std::chrono::duration_cast<std::chrono::microseconds>(frame_end - frame_start).count() / 1000.0f;

        // ========== 2. 新增：ImGui交互面板（含性能显示） ==========
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        // 创建ImGui窗口
        ImGui::Begin("实例化绘制测试面板");
        
        // 2.1 性能监控区域（新增核心）
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "📊 实时性能监控");
        ImGui::Separator();
        ImGui::Text("当前帧率 (FPS)：%.1f", fps);
        ImGui::Text("当前帧总耗时：%.2f ms", frame_time);
        ImGui::Text("当前模式绘制耗时：%.2f ms", mode_draw_time[current_mode]);
        ImGui::Separator();

        // 2.2 Radio按钮：切换绘制模式
        ImGui::Text("选择绘制模式：");
        ImGui::RadioButton("普通绘制", (int*)&current_mode, MODE_NORMAL_DRAW);
        ImGui::RadioButton("实例绘制(Uniform)", (int*)&current_mode, MODE_INSTANCE_UNIFORM);
        ImGui::RadioButton("实例绘制(GPU加载)", (int*)&current_mode, MODE_INSTANCE_GPU);
        ImGui::RadioButton("实例绘制模型", (int*)&current_mode, MODE_INSTANCE_MODEL);

        // 2.3 显示当前模式说明（便于理解不同模式的区别）
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "当前模式说明：");
        const char* mode_descriptions[] = {
            "普通绘制：逐次调用Draw绘制100个四边形（性能最差）",
            "实例绘制(Uniform)：通过Uniform数组传递偏移，一次Draw绘制100个",
            "实例绘制(GPU加载)：数据直接加载到GPU，一次Draw绘制1000个",
            "实例绘制模型：实例化渲染3D模型（琉音）"
        };
        ImGui::TextWrapped("%s", mode_descriptions[current_mode]);

        // 2.4 显示实例数量信息 + 性能对比（新增）
        ImGui::Separator();
        ImGui::Text("📈 性能对比参考：");
        if (current_mode == MODE_NORMAL_DRAW || current_mode == MODE_INSTANCE_UNIFORM) {
            ImGui::Text("当前绘制实例数：100");
            ImGui::Text("性能提示：普通绘制CPU开销是Uniform实例的100倍左右");
        } else if (current_mode == MODE_INSTANCE_GPU) {
            ImGui::Text("当前绘制实例数：1000");
            ImGui::Text("性能提示：GPU实例绘制CPU开销最低，帧率最高");
        } else if (current_mode == MODE_INSTANCE_MODEL) {
            ImGui::Text("当前模型实例数：%zu", instances.size());
            ImGui::Text("性能提示：模型实例化比四边形开销高，但仍优于普通绘制");
        }

        ImGui::End(); // 结束ImGui窗口

        // 渲染ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });



    return 0;
}

// ========== 原有绘制函数实现（保留不变） ==========
inline void NormalDraw(Window& _window, Camera& camera, Mesh& mesh, Shader& sd)
{
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
    sd.use();
    sd.SetMat4("view", view);
    sd.SetMat4("projection", projection);
    for(int i = 0; i < 100; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(i*2.0, j*2.0, 0.0));
            sd.SetMat4("model", model);
            mesh.Draw(sd);
        }
    }
}

inline void InstanceDraw1(Window& _window, Camera& camera, Mesh& mesh, Shader& sd)
{
    glm::mat4 model;
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
    sd.use();
    sd.SetMat4("model", model);
    sd.SetMat4("view", view);
    sd.SetMat4("projection", projection);
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            sd.SetVec3("offsets[" + std::to_string(i * 10 + j) + "]", glm::vec3(i*2.0, j*2.0, 0.0));
        }
    }
    mesh.DrawInstance(sd, 100);
}

inline void InstanceDraw2(Window& _window, Camera& camera, Mesh& mesh, Shader& sd)
{
    glm::mat4 model;
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
    sd.use();
    sd.SetMat4("model", model);
    sd.SetMat4("view", view);
    sd.SetMat4("projection", projection);
    mesh.DrawInstance(sd, 1000);
}

inline void InstanceDraw3(Window& _window, Camera& camera, Model& md_model, Shader& sd)
{
    glm::mat4 model;
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
    sd.use();
    sd.SetMat4("model", model);
    sd.SetMat4("view", view);
    sd.SetMat4("projection", projection);
    md_model.DrawInstance(sd);
}