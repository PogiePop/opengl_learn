#include <window.hpp>
#include <camera.hpp>
#include <mesh_utils.hpp>
#include <model.hpp>
#include <light.hpp>
#include <random>
#define RES_PATH "../../res/"

int main()
{
    Window _window;
    Camera camera;

    // 创建gBuffer帧缓冲
    GLuint gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // 创建position颜色附件
    GLuint positions;
    glGenTextures(1, &positions);
    glBindTexture(GL_TEXTURE_2D, positions);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _window.GetWidth(), _window.GetHeight(), 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // 避免采样到屏幕空间纹理之外的深度值
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positions, 0);

    // 创建normal颜色附件
    GLuint normals;
    glGenTextures(1, &normals);
    glBindTexture(GL_TEXTURE_2D, normals);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _window.GetWidth(), _window.GetHeight(), 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normals, 0);

    // 创建颜色 + 镜面反射颜色附件
    GLuint albedSpec;
    glGenTextures(1, &albedSpec);
    glBindTexture(GL_TEXTURE_2D, albedSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _window.GetWidth(), _window.GetHeight(), 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedSpec, 0);

    GLuint attaches[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attaches);

    // 创建深度缓存rbo
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        static_assert("创建帧缓冲失败!!!");
        return -1;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _window.SetEventCallback(Window::WindowSizeCallback([&](GLFWwindow *window, int width, int height)
                                                        {
        glViewport(0, 0, width, height);
        [&](){
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
            glBindTexture(GL_TEXTURE_2D, positions);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _window.GetWidth(), _window.GetHeight(), 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positions, 0);
            glBindTexture(GL_TEXTURE_2D, normals);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _window.GetWidth(), _window.GetHeight(), 0, GL_RGB, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normals, 0);
            glBindTexture(GL_TEXTURE_2D, albedSpec);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _window.GetWidth(), _window.GetHeight(), 0, GL_RGBA, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedSpec, 0);

            GLuint attaches[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
            glDrawBuffers(3, attaches);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _window.GetWidth(), _window.GetHeight());
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        }(); }));

    _window.SetEventCallback(Window::CursorPosCallback([&](GLFWwindow *window, float xOffset, float yOffset)
                                                       { camera.ProcessLensMove(xOffset, yOffset); }));

    _window.SetEventCallback(Window::ScrollPosCallback([&](GLFWwindow *window, float yOffset)
                                                       { camera.ProcessZoom(yOffset); }));

    Model lamier(RES_PATH "textures/models/liuyin/琉音.pmx");
    Mesh scene = CreateQuadMesh(2.0f);

    // 生成随机采样核心
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    // 线性插值函数
    auto lerp = [](GLfloat a, GLfloat b, GLfloat f)
    {
        return a + f * (b - a);
    };
    for (GLuint i = 0; i < 64; ++i)
    {
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        GLfloat scale = GLfloat(i) / 64.0;
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
    // 创建随机核心转动
    std::vector<glm::vec3> ssaoNoise;
    for (GLuint i = 0; i < 16; i++)
    {
        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f);
        ssaoNoise.push_back(noise);
    }

    GLuint noiseTexture;
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, ssaoNoise.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLuint ssaoFBO;
    glGenFramebuffers(1, &ssaoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    GLuint ssaoColorBuffer;

    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _window.GetWidth(), _window.GetHeight(), 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

    // 模糊
    GLuint ssaoBlurFBO, ssaoColorBufferBlur;
    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _window.GetWidth(), _window.GetHeight(), 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);

    // shader
    Shader geomtryShader(RES_PATH "shaders/ssao/1.vert", nullptr, RES_PATH "shaders/ssao/1.frag");
    Shader ssaoShader(RES_PATH "shaders/ssao/2.vert", nullptr, RES_PATH "shaders/ssao/2.frag");
    Shader ssaoBlurShader(RES_PATH "shaders/ssao/3.vert", nullptr, RES_PATH "shaders/ssao/3.frag");
    Shader lightShader(RES_PATH "shaders/ssao/4.vert", nullptr, RES_PATH "shaders/ssao/4.frag");

    PointLight pt{
        .position = glm::vec3(2.0f, 3.0f, 1.0f), // 光源位置（右+上+前）
        .ambient = glm::vec3(1.0f, 1.0f, 1.0f),  // 暖黄色环境光（弱）
        .diffuse = glm::vec3(1.0f, 1.0f, 1.0f),  // 橙黄色漫反射（主色调）
        .specular = glm::vec3(1.0f, 1.0f, 1.0f), // 白色镜面光（高光）
        .constant = 1.0f,                        // 衰减常数项固定1.0
        .linear = 0.09f,                         // 一次衰减系数（常用值）
        .quadratic = 0.032f                      // 二次衰减系数（适合中近距离光源）
    };

    _window.Run([&](GLFWwindow *window)
                {
                    camera.ProcessInput(window, Time::instance->deltatime);
                    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
                    glEnable(GL_DEPTH_TEST);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                    // geomtry
                    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    geomtryShader.use();
                    glm::mat4 model = glm::mat4(1.0f);
                    //glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    glm::mat4 view = camera.GetViewMatrix();
                    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), _window.GetAspect(), 0.1f, 100.0f);
                    geomtryShader.SetMat4("model", model);
                    geomtryShader.SetMat4("view", view);
                    geomtryShader.SetMat4("projection", projection);
                    lamier.Draw(geomtryShader);

                    // ssao
                    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO); 
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    glDisable(GL_DEPTH_TEST);
                    ssaoShader.use();
                    ssaoShader.SetMat4("projection", projection);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, positions);
                    ssaoShader.SetInt1("gPositionDepth", 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, normals);
                    ssaoShader.SetInt1("gNormal", 1);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, noiseTexture);
                    ssaoShader.SetInt1("texNoise", 2);
                    scene.Draw(ssaoShader);
                    //blur
                    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO); 
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    ssaoBlurShader.use();
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
                    ssaoBlurShader.SetInt1("ssaoInput", 0);
                    scene.Draw(ssaoBlurShader);
                    //light
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    lightShader.use();
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, positions);
                    lightShader.SetInt1("gPositionDepth", 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, normals);
                    lightShader.SetInt1("gNormal", 1);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, albedSpec);
                    lightShader.SetInt1("gAlbedo", 2);
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
                    lightShader.SetInt1("ssao", 3);
                    pt.SetUniform("point", lightShader);
                    scene.Draw(lightShader);


                    ImGui_ImplGlfw_NewFrame();
                    ImGui_ImplOpenGL3_NewFrame();
                    ImGui::NewFrame();

                    // 创建ImGui窗口
                    ImGui::Begin("ssao");
                    ImGui::DragFloat3("->光源颜色", glm::value_ptr(pt.diffuse), 0.1f);
                    ImGui::DragFloat3("->光源位置", glm::value_ptr(pt.position), 0.1f);
                    ImGui::End();
                    ImGui::Render();
                    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                });

    return 0;
}