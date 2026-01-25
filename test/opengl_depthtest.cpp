#include <window.hpp>
#include <camera.hpp>
#include <mesh.hpp>
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

    //创建立方体网格
    Mesh cube = CreateCubeMesh();
    Shader cbSd(RES_PATH "shaders/deptht.vert", nullptr, RES_PATH "shaders/deptht.frag");

    _window.Run([&](GLFWwindow* window){
        static int isLinear = 0;
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cbSd.use();
        glm::mat4 model;
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
        cbSd.SetMat4("model", model);
        cbSd.SetMat4("view", view);
        cbSd.SetMat4("projection", projection);
        cbSd.SetInt1("isLinear", isLinear);
        cube.Draw(cbSd);

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("depth test");
        ImGui::DragInt("ISLINEAR?", &isLinear, 0, 1);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });
    return 0;
}