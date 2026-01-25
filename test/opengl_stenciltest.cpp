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
    Shader cbSd(RES_PATH "shaders/stencilt.vert", nullptr, RES_PATH "shaders/stencilt.frag");

    //创建border
    Mesh border = CreateCubeMesh();
    _window.Run([&](GLFWwindow* window){
        static int isLinear = 0;
        static float bdScale = 1.1f;
        static glm::vec3 color1 =  glm::vec3(1.0f, 1.0f, 1.0f);
        static glm::vec3 color2 =  glm::vec3(0.5f, 0.6f, 0.5f);
        static glm::vec3 trl = glm::vec3(0.0f);
        static bool isBorder = true;
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        cbSd.use();
        glm::mat4 model = glm::translate(glm::mat4(1.0f), trl);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
        cbSd.SetMat4("model", model);
        cbSd.SetMat4("view", view);
        cbSd.SetMat4("projection", projection);
        cbSd.SetInt1("isLinear", isLinear);
        cbSd.SetVec3("color", color1);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        cube.Draw(cbSd);
        cbSd.SetVec3("color", color2);
        glDisable(GL_DEPTH_TEST);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glm::mat4 model1 = glm::scale(model, glm::vec3(bdScale));
        cbSd.SetMat4("model", model1);
        if(isBorder)
        border.Draw(cbSd);
        glEnable(GL_DEPTH_TEST);
        glStencilMask(0xFF);


        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("stencil test");
        ImGui::Checkbox("->isBorder", &isBorder);
        ImGui::Text("cube position:");
        ImGui::DragFloat3("->postion", &trl[0], 0.1f);
        ImGui::Separator();
        ImGui::Text("color:");
        ImGui::DragFloat3("->cube color", &color1[0], 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat3("->border color", &color2[0], 0.01f, 0.0f, 1.0f);
        ImGui::Separator();
        ImGui::Text("scale:");
        ImGui::DragFloat("->border scale", &bdScale, 0.01f, 1.01f, 5.0f);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });
    return 0;
}