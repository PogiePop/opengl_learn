#include <window.hpp>
#include <camera.hpp>
#include <mesh_utils.hpp>
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

    Texture tex(RES_PATH "textures/wo1.png", "diffuse", true);
    //开启subdata
    Mesh cube = CreateCubeMesh(1.0f, std::vector<Texture>{tex}, true);
    Shader sub(RES_PATH "shaders/cullface.vert", nullptr, RES_PATH "shaders/cullface.frag");

    _window.Run([&](GLFWwindow* window){
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sub.use();
        glm::mat4 model;
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
        sub.SetMat4("model", model);
        sub.SetMat4("view", view);
        sub.SetMat4("projection", projection);
        cube.Draw(sub);
    });
}