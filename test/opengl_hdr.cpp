#include <window.hpp>
#include <camera.hpp>
#include <mesh_utils.hpp>
#define RES_PATH "../../res/"

void AdaptWindowSize(const unsigned int &, const unsigned int &, const unsigned int &, int, int);


int main() {
    Window _window;
    Camera camera;


    //创建帧缓冲
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    GLuint fbo_color;
    glGenTextures(1, &fbo_color);
    glBindTexture(GL_TEXTURE_2D, fbo_color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1024, 1024, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GLuint fbo_depth;
    glGenRenderbuffers(1, &fbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, fbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 1024);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo_depth);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_color, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::println("帧缓冲创建失败!!!");
        return -1;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _window.SetEventCallback(Window::WindowSizeCallback([&](GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
        AdaptWindowSize(fbo, fbo_depth, fbo_color, width, height);
    }));

    _window.SetEventCallback(Window::CursorPosCallback([&](GLFWwindow *window, float xOffset, float yOffset) {
        camera.ProcessLensMove(xOffset, yOffset);
    }));

    _window.SetEventCallback(Window::ScrollPosCallback([&](GLFWwindow *window, float yOffset) {
        camera.ProcessZoom(yOffset);
    }));

    Shader obj(RES_PATH "shaders/hdr/1.vert", nullptr, RES_PATH "shaders/hdr/1.frag");
    Shader scene(RES_PATH "shaders/hdr/2.vert", nullptr, RES_PATH "shaders/hdr/2.frag");

    Texture diff(RES_PATH "textures/diff.png", "diffuse", true);
    Texture spec(RES_PATH "textures/spec.png", "specular", true);
    Mesh cube = CreateCubeMesh(1.0f, std::vector<Texture>{diff, spec});
    Mesh quad = CreateQuadMesh(2.0f);

    // 修复：定义光源参数（避免全白）
    std::vector<glm::vec3> lightColors = {
        glm::vec3(200.0f, 200.0f, 200.0f), // 主光源（弱化亮度）
        glm::vec3(0.5f, 0.0f, 0.0f), // 红色辅光
        glm::vec3(0.0f, 0.0f, 0.5f), // 蓝色辅光
        glm::vec3(0.0f, 0.5f, 0.0f) // 绿色辅光
    };
    std::vector<glm::vec3> lightPos = {
        glm::vec3(0.0f, 0.0f, 3.0f), // 主光源在立方体前方
        glm::vec3(3.0f, 0.0f, 0.0f), // 红色光在右侧
        glm::vec3(-3.0f, 0.0f, 0.0f), // 蓝色光在左侧
        glm::vec3(0.0f, 3.0f, 0.0f) // 绿色光在上侧
    };

    _window.Run([&](GLFWwindow *window) {
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, 1024, 1024);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        obj.use();
        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 2.0f));
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom),
                                                (float) _window.GetWidth() / (float) _window.GetHeight(), 0.1f, 100.0f);
        obj.SetMat4("model", model);
        obj.SetMat4("view", view);
        obj.SetMat4("projection", projection);
        obj.SetVec3("viewPos", camera.position);
        obj.SetFloat1("material.shininess", 32.0f);
        obj.SetFloat1("constant", 1.0f);
        obj.SetFloat1("linear", 0.09f);
        obj.SetFloat1("quadratic", 0.032f);
        for (int i = 0; i < lightColors.size(); ++i) {
            obj.SetVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
            obj.SetVec3("lightPos[" + std::to_string(i) + "]", lightPos[i]);
        }
        cube.Draw(obj);
        glViewport(0, 0, _window.GetWidth(), _window.GetHeight());
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        scene.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fbo_color);
        glDisable(GL_DEPTH_TEST);
        quad.Draw(scene);
        glEnable(GL_DEPTH_TEST);

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("HDR");
        ImGui::DragFloat3("->lightPos1", glm::value_ptr(lightPos[0]), 0.01f);
        ImGui::DragFloat3("->lightPos2", glm::value_ptr(lightPos[1]), 0.01f);
        ImGui::DragFloat3("->lightPos3", glm::value_ptr(lightPos[2]), 0.01f);
        ImGui::DragFloat3("->lightPos4", glm::value_ptr(lightPos[3]), 0.01f);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });
}

void AdaptWindowSize(const unsigned int &fbo, const unsigned int &rbo, const unsigned int &color, int width,
                     int height) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, 1024, 1024, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //附着到帧缓冲上
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 1024);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
}
