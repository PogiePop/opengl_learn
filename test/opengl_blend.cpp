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

    Texture grass(RES_PATH "textures/grass.png", "diffuse", true);
    Texture wd(RES_PATH "textures/window.png", "diffuse", true);

    //创建四边形网格
    Mesh quad = CreateQuadMesh(1.0f, std::vector<Texture>{grass});
    Mesh quad2 = CreateQuadMesh(1.0f, std::vector<Texture>{wd});

    Shader blend(RES_PATH "shaders/blendt.vert", nullptr, RES_PATH "shaders/blendt.frag");


    _window.Run([&](GLFWwindow* window){
        static glm::vec3 trl1 = glm::vec3(0.0f);
        static glm::vec3 trl2 = glm::vec3(0.0f, 0.0f, 1.0f);
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        blend.use();
        glm::mat4 model1 = glm::translate(glm::mat4(1.0f), trl1);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
        blend.SetMat4("model", model1);
        blend.SetMat4("view", view);
        blend.SetMat4("projection", projection);
        quad.Draw(blend);
        glm::mat4 model2 = glm::translate(glm::mat4(1.0f), trl2);
        blend.SetMat4("model", model2);
        quad2.Draw(blend);
    });
    return 0;
}