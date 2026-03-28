#include <window.hpp>
#include <mesh_utils.hpp>
#include <camera.hpp>
#include <light.hpp>
#define RES_PATH "../../res/"

void AdaptWindowSize(const unsigned int &fbo, const unsigned int &rbo, const unsigned int *color, int width, int height);
void CreateBlur(const Mesh &scene, const Shader &blur, const GLuint *blurFbo, const GLuint *blurBuffers, const GLuint &colorBuffer, int amount);
void AdaptWindowSize1(const unsigned int* fbo, const unsigned int *colorBuffers, int width, int height);
int main()
{

    Window _window;
    Camera camera;

    // 帧缓冲
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // 创建颜色附件
    GLuint colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for (int i = 0; i < 2; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _window.GetWidth(), _window.GetHeight(), 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }

    GLuint attachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);

    // 创建深度缓冲
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        std::println("帧缓冲创建成功");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLuint blurFbo[2];
    GLuint blurColorBuffers[2];
    glGenFramebuffers(2, blurFbo);
    glGenTextures(2, blurColorBuffers);
    for (int i = 0; i < 2; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, blurFbo[i]);
        glBindTexture(GL_TEXTURE_2D, blurColorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, _window.GetWidth(), _window.GetHeight(), 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurColorBuffers[i], 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 设置回调
    _window.SetEventCallback(Window::WindowSizeCallback([&](GLFWwindow *window, int width, int height)
                                                        {
       glViewport(0, 0, width, height); 
       AdaptWindowSize(fbo, rbo, colorBuffers, width, height);
       AdaptWindowSize1(blurFbo, blurColorBuffers, width, height);
     }));
       
    _window.SetEventCallback(Window::CursorPosCallback([&](GLFWwindow *window, float xOffset, float yOffset)
                                                       { camera.ProcessLensMove(xOffset, yOffset); }));
    _window.SetEventCallback(Window::ScrollPosCallback([&](GLFWwindow *window, float yOffset)
                                                       { camera.ProcessZoom(yOffset); }));

    Texture diff(RES_PATH "textures/diff.png", "diffuse", true);
    Texture spec(RES_PATH "textures/spec.png", "specular", true);
    Mesh box = CreateCubeMesh(1.0f, std::vector{diff, spec});
    Mesh scene = CreateQuadMesh(2.0f);
    Mesh lightBox = CreateCubeMesh();
    Shader light(RES_PATH "shaders/bloom/2.vert", nullptr, RES_PATH "shaders/bloom/2.frag");
    Shader blm(RES_PATH "shaders/bloom/1.vert", nullptr, RES_PATH "shaders/bloom/1.frag");
    Shader blur(RES_PATH "shaders/bloom/3.vert", nullptr, RES_PATH "shaders/bloom/3.frag");
    Shader blur_final(RES_PATH "shaders/bloom/4.vert", nullptr, RES_PATH "shaders/bloom/4.frag");

    PointLight pt{
        .position = glm::vec3(2.0f, 3.0f, 1.0f), // 光源位置（右+上+前）
        .ambient = glm::vec3(2.0f, 2.0f, 2.0f),  // 暖黄色环境光（弱）
        .diffuse = glm::vec3(0.8f, 0.4f, 0.1f),  // 橙黄色漫反射（主色调）
        .specular = glm::vec3(1.0f, 1.0f, 1.0f), // 白色镜面光（高光）
        .constant = 1.0f,                        // 衰减常数项固定1.0
        .linear = 0.09f,                         // 一次衰减系数（常用值）
        .quadratic = 0.032f                      // 二次衰减系数（适合中近距离光源）
    };

    bool bloom = true;
    bool bloomKeyPressed = false;
    float exposure = 1.0f;

    _window.Run([&](GLFWwindow *window)
                {
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glDrawBuffers(2, attachments);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        blm.use();
        glm::mat4 model;
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / (float)_window.GetHeight(), 0.1f, 100.0f);
        blm.SetMat4("model", model);
        blm.SetMat4("view", view);
        blm.SetMat4("projection", projection);
        blm.SetFloat1("material.shininess", 32.0f);
        blm.SetVec3("viewPos", camera.position);
        pt.SetUniform("point", blm);
        box.Draw(blm);
        light.use();
        model = glm::translate(model, pt.position);
        light.SetMat4("model", model);
        light.SetMat4("view", view);
        light.SetMat4("projection", projection);
        light.SetVec3("colors", pt.ambient);
        lightBox.Draw(light);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glBindTexture(GL_TEXTURE_2D, colorBuffers[1]);
        // blur.use();
        // scene.Draw(blur);
        CreateBlur(scene, blur, blurFbo, blurColorBuffers, colorBuffers[1], 20);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        blur_final.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, blurColorBuffers[0]);
        blur_final.SetInt1("bloom", bloom);
        blur_final.SetFloat1("exposure", exposure);
        blur_final.SetInt1("scene", 0);
        blur_final.SetInt1("bloomBlur", 1);
        scene.Draw(blur_final);


         ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        // 创建ImGui窗口
        ImGui::Begin("泛光");
        ImGui::DragFloat3("->光源颜色", glm::value_ptr(pt.ambient), 0.1f);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); });
    return 0;
}

void AdaptWindowSize(const unsigned int &fbo, const unsigned int &rbo, const unsigned int *colorBuffers, int width, int height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    for (int i = 0; i < 2; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
}

void AdaptWindowSize1(const unsigned int* fbo, const unsigned int *colorBuffers, int width, int height)
{
    for (int i = 0; i < 2; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo[i]);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
}

void CreateBlur(const Mesh &scene, const Shader &blur, const GLuint *blurFbo, const GLuint *blurBuffers, const GLuint &colorBuffer, int amount)
{
    GLboolean horizontal = true, first_iteration = true;
    blur.use();
    blur.SetInt1("image", 0);
    for (GLuint i = 0; i < amount; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, blurFbo[horizontal]);
        blur.SetInt1("horizontal", horizontal);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffer : blurBuffers[!horizontal]);
        scene.Draw(blur);
        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}