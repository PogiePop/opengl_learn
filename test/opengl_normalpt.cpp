#include <window.hpp>
#include <camera.hpp>
#include <mesh_utils.hpp>

#include "light.hpp"
#include "material.hpp"
#include "model.hpp"
#define RES_PATH "../../res/"


int main() {
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

    Texture t1(RES_PATH "textures/brickwall.jpg", "diffuse", true);
    Texture t2(RES_PATH "textures/brickwall_normal.jpg", "normal", true);
    Texture t3(RES_PATH "textures/spec.png", "specular", true);
     Mesh quad = CreateQuadMesh(2.0f, std::vector<Texture>{t1, t2, t3}, false, true);
    Shader sd1(RES_PATH "shaders/normalpt/1.vert", nullptr, RES_PATH "shaders/normalpt/1.frag");
    Shader sd2(RES_PATH "shaders/normalpt/2.vert", RES_PATH "shaders/normalpt/2.geom", RES_PATH "shaders/normalpt/2.frag");
    //加载模型
    Model liuyin(RES_PATH "textures/models/liuyin/琉音.pmx", _instance_ep, false, true);
    //平行光
    ParallelLight prt{
        .direction = glm::vec3(0.0f, 2.0f, 1.0f),
        .ambient = glm::vec3(0.2f, 0.1f, 0.05f), // 暖黄色环境光（弱）
        .diffuse = glm::vec3(0.8f, 0.4f, 0.1f),  // 橙黄色漫反射（主色调）
        .specular = glm::vec3(1.0f, 1.0f, 1.0f), // 白色镜面光（高光）
    };

    PointLight pt{
        .position = glm::vec3(2.0f, 3.0f, 1.0f),  // 光源位置（右+上+前）
        .ambient = glm::vec3(0.2f, 0.1f, 0.05f), // 暖黄色环境光（弱）
        .diffuse = glm::vec3(0.8f, 0.4f, 0.1f),  // 橙黄色漫反射（主色调）
        .specular = glm::vec3(1.0f, 1.0f, 1.0f), // 白色镜面光（高光）
        .constant = 1.0f,                        // 衰减常数项固定1.0
        .linear = 0.09f,                         // 一次衰减系数（常用值）
        .quadratic = 0.032f                      // 二次衰减系数（适合中近距离光源）
    };

    Material mt;
    mt.shininess = 32.0f;

    _window.Run([&](GLFWwindow* window) {
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sd1.use();
        sd1.SetVec3("viewPos", camera.position);
        sd1.SetFloat1("material.shininess", mt.shininess);
        pt.SetUniform("plt", sd1);
        glm::mat4 model;
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / (float)_window.GetHeight(), 0.1f, 100.0f);
        sd1.SetMat4("model", model);
        sd1.SetMat4("view", view);
        sd1.SetMat4("projection", projection);
        //quad.Draw(sd1);
        liuyin.Draw(sd1);
        sd2.use();
        sd2.SetMat4("model", model);
        sd2.SetMat4("view", view);
        sd2.SetMat4("projection", projection);
        // glBindTexture(GL_TEXTURE_2D, t2.GetID());
        // sd2.SetInt1("material.normal0", 0);
        sd2.SetVec3("color", glm::vec3(1.0f));
        //liuyin.Draw(sd2);
        //quad.Draw(sd2);
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("法线贴图测试");
        //ImGui::DragFloat3("->光源方向", &prt.direction[0], 0.1f);
        ImGui::DragFloat3("->position", &pt.position[0], 0.1f);
        ImGui::DragFloat3("->ambient", &pt.ambient[0], 0.01f, 0.0, 1.0);
        ImGui::DragFloat3("->diffuse", &pt.diffuse[0], 0.01f, 0.0, 1.0);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });
    return 0;
}