#include <window.hpp>
#include <camera.hpp>
#include <mesh_utils.hpp>
#define RES_PATH "../../res/"

// 定义测试状态枚举（用于切换不同测试）
enum class TestType {
    PointsTest = 0,
    MidCubeTest,
    FrontFacingTest,
    UniformTest
};

// 全局/主函数内的状态变量（捕获到lambda中）
TestType current_test = TestType::PointsTest;
float cube_size = 1.0f; // 立方体大小（ImGui可调整）

// 预声明所有复用的资源对象（循环外创建）
Mesh pts_mesh;
Shader sdpt_shader;

Mesh cube_mid_mesh;
Shader sd_mid_shader;

Texture front_tex, back_tex;
Mesh cube_front_mesh;
Shader sd_front_shader;

Mesh cube_uniform_mesh;
Shader sd_uniform_shader;
unsigned int ubo; // UBO缓冲区

// 四个Uniform立方体的配置（位置+颜色）
struct CubeConfig {
    glm::vec3 position;
    glm::vec4 color;
};
std::vector<CubeConfig> uniform_cubes = {
    {{ 2.0f,  0.0f,  0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}, // 红色 - 右
    {{-2.0f,  0.0f,  0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}}, // 绿色 - 左
    {{ 0.0f,  2.0f,  0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}, // 蓝色 - 上
    {{ 0.0f, -2.0f,  0.0f}, {1.0f, 1.0f, 0.0f, 1.0f}}  // 黄色 - 下
};

std::vector<glm::vec3> points = {
    glm::vec3(0.0f, 0.0f, 0.0f),   // 原点
    glm::vec3(1.0f, 0.0f, 0.0f),   // X轴
    glm::vec3(0.0f, 1.0f, 0.0f)    // Y轴
};

// 初始化所有测试资源（循环外执行）
void InitTestResources(Window& window) {
    // 1. 点测试资源
    pts_mesh = CreatePointMesh(points);
    sdpt_shader = Shader(RES_PATH "shaders/highglsl/pt.vert", nullptr, RES_PATH "shaders/highglsl/pt.frag");

    // 2. 普通立方体测试资源
    cube_mid_mesh = CreateCubeMesh(cube_size);
    sd_mid_shader = Shader(RES_PATH "shaders/highglsl/cube.vert", nullptr, RES_PATH "shaders/highglsl/cube.frag");

    // 3. 正反面纹理测试资源
    front_tex = Texture(RES_PATH "textures/wo1.png", "diffuse", true);
    back_tex = Texture(RES_PATH "textures/l1.png", "diffuse", true);
    cube_front_mesh = CreateCubeMesh(cube_size, std::vector<Texture>{front_tex, back_tex});
    sd_front_shader = Shader(RES_PATH "shaders/highglsl/difftex.vert", nullptr, RES_PATH "shaders/highglsl/difftex.frag");

    // 4. UBO测试资源
    cube_uniform_mesh = CreateCubeMesh(cube_size);
    sd_uniform_shader = Shader(RES_PATH "shaders/highglsl/datat.vert", nullptr, RES_PATH "shaders/highglsl/datat.frag");
    
    // 创建UBO（适配单个立方体的MVP+颜色，四个立方体循环更新）
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*3 + sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW); // 用DYNAMIC_DRAW适配频繁更新
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // 绑定UBO块
    sd_uniform_shader.use();
    sd_uniform_shader.BindBlock("Matrix", 2);
}

// ImGui交互模块（绘制UI+处理状态切换）
void DrawImGuiUI() {
    // ImGui窗口布局
    ImGui::Begin("测试控制台");
    ImGui::SetWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);

    // 测试状态切换（单选按钮）
    ImGui::Text("请选择测试:");
    ImGui::RadioButton("点 测试", (int*)&current_test, (int)TestType::PointsTest);
    ImGui::RadioButton("立方体位置与颜色 测试", (int*)&current_test, (int)TestType::MidCubeTest);
    ImGui::RadioButton("前后方不同纹理 测试", (int*)&current_test, (int)TestType::FrontFacingTest);
    ImGui::RadioButton("uniform对象 测试", (int*)&current_test, (int)TestType::UniformTest);

    // // 立方体大小调整
    // ImGui::Separator();
    // ImGui::SliderFloat("Cube Size", &cube_size, 0.1f, 2.0f, "%.1f");

    // 当前状态提示
    ImGui::Separator();
    const char* test_names[] = {"点", "立方体位置与颜色", "前后方不同纹理", "uniform对象"};
    ImGui::Text("当前 测试: %s", test_names[(int)current_test]);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

    ImGui::End();
}

// 执行当前选中的测试
void ExecuteCurrentTest(Window& window, Camera& camera) {
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), 
                                           (float)window.GetWidth() / window.GetHeight(), 
                                           0.1f, 100.0f);

    switch (current_test) {
        case TestType::PointsTest: {
            sdpt_shader.use();
            glm::mat4 model;
            sdpt_shader.SetMat4("model", model);
            sdpt_shader.SetMat4("view", view);
            sdpt_shader.SetMat4("projection", projection);
            glEnable(GL_PROGRAM_POINT_SIZE);
            pts_mesh.DrawPoint();  
            glDisable(GL_PROGRAM_POINT_SIZE);
            break;
        }

        case TestType::MidCubeTest: {
            sd_mid_shader.use();
            glm::mat4 model;
            sd_mid_shader.SetMat4("model", model);
            sd_mid_shader.SetMat4("view", view);
            sd_mid_shader.SetMat4("projection", projection);
            sd_mid_shader.SetInt1("halfw", window.GetWidth() / 2);
            cube_mid_mesh.Draw(sd_mid_shader);
            break;
        }

        case TestType::FrontFacingTest: {
            glDisable(GL_CULL_FACE); // 禁用面剔除
            sd_front_shader.use();
            glm::mat4 model;
            sd_front_shader.SetMat4("model", model);
            sd_front_shader.SetMat4("view", view);
            sd_front_shader.SetMat4("projection", projection);
            cube_front_mesh.Draw(sd_front_shader);
            glEnable(GL_CULL_FACE); // 恢复默认
            break;
        }

        case TestType::UniformTest: {
            sd_uniform_shader.use();
            glBindBuffer(GL_UNIFORM_BUFFER, ubo);

            // 循环绘制四个不同位置/颜色的立方体
            for (const auto& cfg : uniform_cubes) {
                // 构建当前立方体的model矩阵（位置偏移）
                glm::mat4 model = glm::translate(glm::mat4(1.0f), cfg.position);
                
                // 更新UBO数据（MVP + 颜色）
                glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(model));
                glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
                glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*2, sizeof(glm::mat4), glm::value_ptr(projection));
                glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4)*3, sizeof(glm::vec4), glm::value_ptr(cfg.color));

                // 绘制当前立方体
                cube_uniform_mesh.Draw(sd_uniform_shader);
            }

            glBindBuffer(GL_UNIFORM_BUFFER, 0);
            break;
        }
    }
}

int main()
{
    Window _window;
    Camera camera;

    
    InitTestResources(_window);

    // 设置窗口回调
    _window.SetEventCallback(Window::WindowSizeCallback([](GLFWwindow* window, int width, int height){
        glViewport(0, 0, width, height);
    }));

    _window.SetEventCallback(Window::CursorPosCallback([&](GLFWwindow* window, float xOffset, float yOffset){
            camera.ProcessLensMove(xOffset, yOffset);
    }));

    _window.SetEventCallback(Window::ScrollPosCallback([&](GLFWwindow* window, float yOffset){
            camera.ProcessZoom(yOffset);
    }));

    // --------------------------
    // 主循环（整合所有测试）
    // --------------------------
    _window.Run([&](GLFWwindow* window){
        // 1. 前置渲染准备
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 2. ImGui帧开始
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 3. 绘制ImGui交互界面
        DrawImGuiUI();

        // 4. 执行当前选中的测试
        ExecuteCurrentTest(_window, camera);

        // 5. ImGui渲染
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    });

    // --------------------------
    // 资源清理
    // --------------------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glDeleteBuffers(1, &ubo);

    return 0;
}