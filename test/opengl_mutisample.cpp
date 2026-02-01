#include <window.hpp>
#include <camera.hpp>
#include <model.hpp>
#include <mesh_utils.hpp>
#include <iostream>
#define RES_PATH "../../res/"

// 定义采样模式枚举
enum SampleMode {
    MODE_SINGLE_SAMPLE = 0,        // 1.普通采样
    MODE_MULTI_SAMPLE = 1,         // 2.多重采样
    MODE_MULTI_SAMPLE_POST = 2     // 3.离屏渲染多重采样+后处理
};

// 后处理效果枚举（对应着色器state）
const int POST_DEFAULT = 0;       // 默认
const int POST_INVERSION = 1;     // 反相
const int POST_NUCLEAR = 2;       // 核效果
const int POST_BLUR = 3;          // 模糊
const int POST_EDGE = 4;          // 边缘检测

// 原有函数声明
void AdaptWindowSize(const unsigned int& fbo, const unsigned int& rbo, const unsigned int& color, int width, int height);
void AdaptWindowSizeMultiple(const unsigned int& fbo, const unsigned int& rbo, const unsigned int& color, int width, int height);
void DrawSingleSample(Window& _window, Camera& camera, Shader& sd, Mesh& mesh);
void DrawMultiSample(Window& _window, Camera& camera, Shader& sd, Mesh& mesh, const unsigned int& fbo, const unsigned int& color);
void DrawMultiSampleWithAfterProcess(Window& _window, Camera& camera, Shader& sd, Shader& mtsd, Model& mt, Mesh& mat, const unsigned int& fbo, const unsigned int& inter, const unsigned int& color, int post_state); // 新增post_state参数

int main()
{
    Window::SetInitCallback([](){ glfwWindowHint(GL_SAMPLES, 4); });
    Window _window;
    Camera camera;

    // ========== 新增：交互控制变量 ==========
    SampleMode current_sample_mode = MODE_SINGLE_SAMPLE; // 默认普通采样
    int post_process_state = POST_DEFAULT;               // 默认后处理效果

    

    // ========== 原有资源加载 ==========
    Mesh cube = CreateCubeMesh();
    Mesh mat = CreateQuadMesh(2.0f);
    Model liuyin(RES_PATH "textures/models/liuyin/琉音.pmx");
    Shader sd(RES_PATH "shaders/mutisample/1.vert", nullptr, RES_PATH "shaders/mutisample/1.frag");
    Shader fsd(RES_PATH "shaders/framebuffer.vert", nullptr, RES_PATH "shaders/framebuffer.frag");
    Shader mtt(RES_PATH "shaders/modelt.vert", nullptr, RES_PATH "shaders/modelt.frag");

    // ========== 原有帧缓冲创建 ==========
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    unsigned int colorTex;
    glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorTex);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, _window.GetWidth(), _window.GetHeight(), GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorTex, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, _window.GetWidth(), _window.GetHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::println("帧缓冲创建失败");
        return -1;
    }

    unsigned int interFbo;
    glGenFramebuffers(1, &interFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, interFbo);
    unsigned int interColorTex;
    glGenTextures(1, &interColorTex);
    glBindTexture(GL_TEXTURE_2D, interColorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _window.GetWidth(), _window.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, interColorTex, 0);

    unsigned int interRbo;
    glGenRenderbuffers(1, &interRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, interRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _window.GetWidth(), _window.GetHeight());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, interRbo);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::println("fail to create fbo");
        return -1;
    }

    // ========== 原有事件回调 ==========
    _window.SetEventCallback(Window::WindowSizeCallback([&](GLFWwindow *window, int width, int height)
                                                        { glViewport(0, 0, width, height);
                                                            AdaptWindowSizeMultiple(fbo, rbo, colorTex, width, height);
                                                           AdaptWindowSize(interFbo, interRbo, interColorTex, width, height);
                                                        }));

    _window.SetEventCallback(Window::CursorPosCallback([&](GLFWwindow *window, float xOffset, float yOffset)
                                                       { camera.ProcessLensMove(xOffset, yOffset); }));

    _window.SetEventCallback(Window::ScrollPosCallback([&](GLFWwindow *window, float yOffset)
                                                       { camera.ProcessZoom(yOffset); }));

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ========== 渲染循环 ==========
    _window.Run([&](GLFWwindow *window){
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ========== 1. 根据采样模式调用对应绘制函数 ==========
        switch (current_sample_mode) {
            case MODE_SINGLE_SAMPLE: // 普通采样
                DrawSingleSample(_window, camera, sd, cube);
                break;

            case MODE_MULTI_SAMPLE: // 多重采样
                DrawMultiSample(_window, camera, sd, cube, fbo, colorTex);
                break;

            case MODE_MULTI_SAMPLE_POST: // 离屏多重采样+后处理
                // 传递后处理状态给绘制函数
                DrawMultiSampleWithAfterProcess(_window, camera, mtt, fsd, liuyin, mat, fbo, interFbo, interColorTex, post_process_state);
                break;

            default:
                DrawSingleSample(_window, camera, sd, cube);
                break;
        }

        // ========== 2. 新增：ImGui交互面板 ==========
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        // 创建ImGui窗口
        ImGui::Begin("多重采样测试面板");
        
        // 2.1 采样模式选择
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "🔍 采样模式选择");
        ImGui::Separator();
        ImGui::RadioButton("1. 普通采样", (int*)&current_sample_mode, MODE_SINGLE_SAMPLE);
        ImGui::RadioButton("2. 多重采样", (int*)&current_sample_mode, MODE_MULTI_SAMPLE);
        ImGui::RadioButton("3. 离屏渲染多重采样（带后处理）", (int*)&current_sample_mode, MODE_MULTI_SAMPLE_POST);

        // 2.2 后处理效果选择（仅第三种模式显示）
        if (current_sample_mode == MODE_MULTI_SAMPLE_POST) {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "🎨 后处理效果选择");
            ImGui::RadioButton("默认 (无效果)", &post_process_state, POST_DEFAULT);
            ImGui::SameLine();
            ImGui::RadioButton("反相", &post_process_state, POST_INVERSION);
            ImGui::SameLine();
            ImGui::RadioButton("核效果", &post_process_state, POST_NUCLEAR);
            ImGui::SameLine();
            ImGui::RadioButton("模糊", &post_process_state, POST_BLUR);
            ImGui::SameLine();
            ImGui::RadioButton("边缘检测", &post_process_state, POST_EDGE);
        }

        // 2.3 模式说明
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.0f, 0.8f, 1.0f, 1.0f), "📝 模式说明");
        const char* mode_descriptions[] = {
            "普通采样：无抗锯齿，直接渲染到默认帧缓冲",
            "多重采样：开启4xMSAA抗锯齿，渲染到多重采样帧缓冲后拷贝到屏幕",
            "离屏多重采样：先渲染到多重采样FBO，解析后做后处理，再渲染到屏幕"
        };
        ImGui::TextWrapped("%s", mode_descriptions[current_sample_mode]);

        ImGui::End(); // 结束ImGui窗口

        // 渲染ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });


    return 0;
}

// ========== 原有函数实现（仅修改DrawMultiSampleWithAfterProcess） ==========
void DrawSingleSample(Window& _window, Camera& camera, Shader& sd, Mesh& mesh)
{
    glm::mat4 model;
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
    sd.use();
    sd.SetMat4("model", model);
    sd.SetMat4("view", view);
    sd.SetMat4("projection", projection); 
    mesh.Draw(sd);
}

void DrawMultiSample(Window& _window, Camera& camera, Shader& sd, Mesh& mesh, const unsigned int& fbo, const unsigned int& color)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 model;
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
    sd.use();
    sd.SetMat4("model", model);
    sd.SetMat4("view", view);
    sd.SetMat4("projection", projection); 
    mesh.Draw(sd);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, _window.GetWidth(), _window.GetHeight(), 0, 0, _window.GetWidth(), _window.GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

// 新增post_state参数，传递后处理状态
void DrawMultiSampleWithAfterProcess(Window& _window, Camera& camera, Shader& sd, Shader& mtsd, Model& mt, Mesh& mat, const unsigned int& fbo, const unsigned int& inter, const unsigned int& color, int post_state)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::mat4 model;
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
    sd.use();
    sd.SetMat4("model", model);
    sd.SetMat4("view", view);
    sd.SetMat4("projection", projection); 
    mt.Draw(sd);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, inter);
    glBlitFramebuffer(0, 0, _window.GetWidth(), _window.GetHeight(), 0, 0, _window.GetWidth(), _window.GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    mtsd.use();
    glBindTexture(GL_TEXTURE_2D, color);
    mtsd.SetInt1("material.diffuse0", 0);
    // ========== 修改：使用传递的后处理状态 ==========
    mtsd.SetInt1("state", post_state);
    mat.Draw(mtsd);
    glEnable(GL_DEPTH_TEST);
}

void AdaptWindowSizeMultiple(const unsigned int& fbo, const unsigned int& rbo, const unsigned int& color, int width, int height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, color);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, width, height, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, color, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}