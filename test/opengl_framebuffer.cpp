#include <window.hpp>
#include <camera.hpp>
#include <mesh_utils.hpp>
#define RES_PATH "../../res/"

const int DEFAULT = 0;
const int INVERSION = 1;
const int NUCLEAR = 2;
const int BLUR = 3;
const int EDGE_CHECK = 4;

void AdaptWindowSize(const unsigned int&, const unsigned int&, const unsigned int&, int, int);

int main()
{
    Window _window;
    Camera camera;

    int current_state = DEFAULT; // 默认选中"无效果"
    const char* post_process_options[] = {
        "默认 (无效果)",
        "反色 (INVERSION)",
        "核化 (NUCLEAR)",
        "模糊 (BLUR)",
        "边缘检测 (EDGE_CHECK)"
    };
    const int post_process_values[] = {DEFAULT, INVERSION, NUCLEAR, BLUR, EDGE_CHECK};

    //创建帧缓冲
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    //创建颜色纹理附件
    unsigned int colorTex;
    glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _window.GetWidth(), _window.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //附着到帧缓冲上
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

    //创建渲染缓冲对象
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _window.GetWidth(), _window.GetHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    //检查是否创建成功
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::println("fail to create fbo");
        return -1;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _window.SetEventCallback(Window::WindowSizeCallback([&](GLFWwindow* window, int width, int height){
        glViewport(0, 0, width, height);
        AdaptWindowSize(fbo, rbo, colorTex, width, height);
    }));

    _window.SetEventCallback(Window::CursorPosCallback([&](GLFWwindow* window, float xOffset, float yOffset){
        camera.ProcessLensMove(xOffset, yOffset);
    }));

    _window.SetEventCallback(Window::ScrollPosCallback([&](GLFWwindow* window, float yOffset){
        camera.ProcessZoom(yOffset);
    }));
    

    Texture wood(RES_PATH "textures/wo1.png", "diffuse", true);
    Mesh cube = CreateCubeMesh(1.0f, std::vector<Texture>{wood});
    Mesh quad = CreateQuadMesh(2.0f);
    Shader fsd(RES_PATH "shaders/framebuffer.vert", nullptr, RES_PATH "shaders/framebuffer.frag");
    Shader csd(RES_PATH "shaders/modelt.vert", nullptr, RES_PATH "shaders/modelt.frag");
    _window.Run([&](GLFWwindow* window){
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        //离屏渲染
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        csd.use();
        glm::mat4 model;
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
        csd.SetMat4("view", view);
        csd.SetMat4("model", model);
        csd.SetMat4("projection", projection);
        cube.Draw(csd);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        fsd.use();
        glDisable(GL_DEPTH_TEST);
        quad.ClearTextures();
        quad.AddTexture(Texture(colorTex, "diffuse"));
        fsd.SetInt1("state", current_state);
        quad.Draw(fsd);


        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        // 创建ImGui窗口
        ImGui::Begin("后处理效果控制面板");
        
        // 方式1：下拉框选择（简洁）
        ImGui::Text("选择后处理效果：");
        int selected_idx = 0;
        // 查找当前状态对应的索引
        for (int i = 0; i < 5; i++) {
            if (post_process_values[i] == current_state) {
                selected_idx = i;
                break;
            }
        }
        // 下拉框交互
        if (ImGui::Combo("##post_process", &selected_idx, post_process_options, IM_ARRAYSIZE(post_process_options))) {
            current_state = post_process_values[selected_idx];
        }
        ImGui::End();

        // 渲染ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });
    return 0;
}


void AdaptWindowSize(const unsigned int& fbo, const unsigned int& rbo, const unsigned int& color, int width, int height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //附着到帧缓冲上
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
}