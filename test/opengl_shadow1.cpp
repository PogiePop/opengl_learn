//
// Created by wzj on 2026/2/4.
//
#include <window.hpp>
#include <camera.hpp>
#include <mesh_utils.hpp>
#include <light.hpp>

#include "material.hpp"
#define RES_PATH "../../res/"

int main()
{
    Window _window;
    Camera camera;

    _window.SetEventCallback(Window::WindowSizeCallback([](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }));

    _window.SetEventCallback(Window::CursorPosCallback([&](GLFWwindow* window, float xOffset, float yOffset) {
        camera.ProcessLensMove(xOffset, yOffset);
    }));

    _window.SetEventCallback(Window::ScrollPosCallback([&](GLFWwindow* window, float yOffset) {
        camera.ProcessZoom(yOffset);
    }));

    //创建深度缓冲纹理
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
    for (int x = 0; x < 6; x++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + x, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0,
            GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //创建帧缓冲
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    Texture diff(RES_PATH "textures/diff.png", "diffuse", true);
    Texture spec(RES_PATH "textures/spec.png", "specular", true);
    Mesh cube = CreateCubeMesh(1.0f, std::vector<Texture>{ diff, spec });
    Shader sd1(RES_PATH "shaders/shadow/4.vert", RES_PATH "shaders/shadow/4.geom", RES_PATH "shaders/shadow/4.frag");
    Shader sd2(RES_PATH "shaders/shadow/5.vert", nullptr, RES_PATH "shaders/shadow/5.frag");

    PointLight pt{
        .position = glm::vec3(2.0f, 3.0f, 1.0f),  // 光源位置（右+上+前）
        .ambient = glm::vec3(0.2f, 0.1f, 0.05f), // 暖黄色环境光（弱）
        .diffuse = glm::vec3(0.8f, 0.4f, 0.1f),  // 橙黄色漫反射（主色调）
        .specular = glm::vec3(1.0f, 1.0f, 1.0f), // 白色镜面光（高光）
        .constant = 1.0f,                        // 衰减常数项固定1.0
        .linear = 0.09f,                         // 一次衰减系数（常用值）
        .quadratic = 0.032f                      // 二次衰减系数（适合中近距离光源）
    };

    Material mt{
        0, 1, 32.0f
    };

    _window.Run([&](GLFWwindow* window) {
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model;
        glm::mat4 model1 = glm::scale(glm::mat4(1.0f), glm::vec3(100.0f, 1.0f, 100.0f));
        model1 = glm::translate(model1, glm::vec3(0.0f, -2.0f, 0.0f));
        std::vector<glm::mat4> views = pt.GetViewMatrices();
        glm::mat4 pj_lt = pt.GetProjectionMatrix();
        glViewport(0, 0, 1024, 1024);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        sd1.use();
        sd1.SetMat4("model", model);
        sd1.SetVec3("lightPos", pt.position);
        for (int x = 0; x < 6; x++)
            sd1.SetMat4("shadowMatrices[" + std::to_string(x) + "]", pj_lt * views[x]);
        cube.Draw(sd1);
        glCullFace(GL_BACK);
        glViewport(0, 0, _window.GetWidth(), _window.GetHeight());
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / (float)_window.GetHeight(), 0.1f, 100.0f);
        sd2.use();
        sd2.SetMat4("projection", projection);
        sd2.SetMat4("view", view);
        sd2.SetMat4("model", model);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
        sd2.SetInt1("depthMap", 2);
        mt.SetUniform("material", sd2);
        pt.SetUniform("plt", sd2);
        sd2.SetVec3("viewPos", camera.position);
        cube.Draw(sd2);
        sd2.SetMat4("model", model1);
        cube.Draw(sd2);

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("阴影测试");
        ImGui::Text("PointLight Attribute:");
        ImGui::DragFloat3("->point position", &pt.position[0], 0.1f);
        ImGui::DragFloat3("->point ambient", &pt.ambient[0], 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat3("->point diffuse", &pt.diffuse[0], 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat3("->point specular", &pt.specular[0], 0.01f, 0.0f, 1.0f);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });
}