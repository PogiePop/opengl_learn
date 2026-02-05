#include <window.hpp>
#include <camera.hpp>
#include <model.hpp>
#include <mesh_utils.hpp>
#define RES_PATH "../../res/"

void DrawGamma(Window& _window, Camera& camera, Shader& sd, Model& md);
void DrawGammaWithSRGB(Window& _window, Camera& camera, Shader& sd, Model& md);

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
     Model liuyin(RES_PATH "textures/models/liuyin/琉音.pmx", _instance_ep, true);

    //创建shader
     Shader sun(RES_PATH "shaders/modelt.vert", nullptr, RES_PATH "shaders/modelt.frag");
    _window.Run([&](GLFWwindow* window){
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        DrawGamma(_window, camera, sun, liuyin);
    });
    return 0;
}


void DrawGamma(Window& _window, Camera& camera, Shader& sd, Model& md)
{
    glm::mat4 model;
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
    sd.use();
    sd.SetMat4("model", model);
    sd.SetMat4("view", view);
    sd.SetMat4("projection", projection);
    glEnable(GL_FRAMEBUFFER_SRGB);
    md.Draw(sd);
    glDisable(GL_FRAMEBUFFER_SRGB);
}