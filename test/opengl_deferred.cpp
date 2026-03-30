#include <window.hpp>
#include <mesh_utils.hpp>
#include <camera.hpp>
#include <model.hpp>
#include <light.hpp>
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _window.GetWidth(), _window.GetHeight(), 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
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
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _window.GetWidth(), _window.GetHeight(), 0, GL_RGB, GL_FLOAT, nullptr);
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

    // pass1
    Shader pass1(RES_PATH "shaders/deferred/1.vert", nullptr, RES_PATH "shaders/deferred/1.frag");
    Model liuyin(RES_PATH "textures/models/liuyin/琉音.pmx");
    // pass2(光照处理)
    Shader pass2(RES_PATH "shaders/deferred/2.vert", nullptr, RES_PATH "shaders/deferred/2.frag");
    Mesh scene = CreateQuadMesh(2.0f);
    //混合
    Shader blend(RES_PATH "shaders/deferred/3.vert", nullptr, RES_PATH "shaders/deferred/3.frag");
    Mesh cube = CreateCubeMesh();

    PointLight pt{
        .position = glm::vec3(2.0f, 3.0f, 1.0f), // 光源位置（右+上+前）
        .ambient = glm::vec3(1.0f, 1.0f, 1.0f),  // 暖黄色环境光（弱）
        .diffuse = glm::vec3(0.8f, 0.4f, 0.1f),  // 橙黄色漫反射（主色调）
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
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), _window.GetAspect(), 0.1f, 100.0f);
        //非光照处理阶段
        pass1.use();
        pass1.SetMat4("model", model);
        pass1.SetMat4("view", view);
        pass1.SetMat4("projection", projection);
        liuyin.Draw(pass1);

        //光照处理阶段
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        pass2.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, positions);
        pass2.SetInt1("positions", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normals);
        pass2.SetInt1("normals", 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, albedSpec);
        pass2.SetInt1("albedSpec", 2);
        pass2.SetVec3("viewPos", camera.position);
        pt.SetUniform("point", pass2);
        glDisable(GL_DEPTH_TEST);
        scene.Draw(pass2);
        glEnable(GL_DEPTH_TEST); 
        
        //将gBuffer深度信息复制到默认帧缓冲
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, _window.GetWidth(), _window.GetHeight(), 0, 0, _window.GetWidth(), _window.GetHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        blend.use();
        model = glm::translate(model, glm::vec3(0.5f, 0.1f, -2.1f));
        blend.SetMat4("model", model);
        blend.SetMat4("view", view);
        blend.SetMat4("projection", projection);
        cube.Draw(blend);
        
        });
    return 0;
}