#include <window.hpp>
#include <mesh.hpp>
#include <model.hpp>
#include <camera.hpp>
#define RES_PATH "../../res/"

int main()
{
    Window _window;
    Camera camera;

    _window.SetEventCallback(Window::WindowSizeCallback([](GLFWwindow* window, int width, int height){
        glViewport(0, 0, width, height);
    }));

    _window.SetEventCallback(Window::CursorPosCallback([&](GLFWwindow* window, float xOffset, float yOffset){
        camera.ProcessLensMove(xOffset, yOffset);
    }));

    _window.SetEventCallback(Window::ScrollPosCallback([&](GLFWwindow* window, float yOffset){
        camera.ProcessZoom(yOffset);
    }));

    //加载模型
     Model liuyin(RES_PATH "textures/models/liuyin/琉音.pmx");

    //创建shader
     Shader sun(RES_PATH "shaders/modelt.vert", nullptr, RES_PATH "shaders/modelt.frag");

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    _window.Run([&](GLFWwindow* window){
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        camera.ProcessInput(window, Time::instance->deltatime);
        glm::mat4 model;
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
        sun.use();
        sun.SetMat4("model", model);
        sun.SetMat4("view", view);
        sun.SetMat4("projection", projection);
        liuyin.Draw(sun);

    });
    return 0;
}