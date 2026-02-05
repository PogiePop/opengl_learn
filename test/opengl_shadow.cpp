#include <window.hpp>
#include <camera.hpp>
#include <material.hpp>
#include <mesh_utils.hpp>
#include <light.hpp>
#include <model.hpp>
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

    //创建深度缓冲纹理
    unsigned int depth = 0;
    glGenTextures(1, &depth);
    glBindTexture(GL_TEXTURE_2D, depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); 
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    //创建帧缓冲
    unsigned int depthFbo = 0;
    glGenFramebuffers(1, &depthFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);
    //将绘制/读取设为空
    glReadBuffer(GL_NONE);
    glDrawBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Texture diff(RES_PATH "textures/diff.png", "diffuse", true);
    Texture spec(RES_PATH "textures/spec.png", "specular", true);
    Mesh cube = CreateCubeMesh(1.0f, std::vector<Texture>{diff, spec});
    Mesh shadow = CreateQuadMesh();
    Mesh scene = CreateQuadMesh(2.0f);
     //加载模型
     Model liuyin(RES_PATH "textures/models/liuyin/琉音.pmx");

    //创建shader
     Shader sun(RES_PATH "shaders/modelt.vert", nullptr, RES_PATH "shaders/modelt.frag");

    Shader sd1(RES_PATH "shaders/shadow/1.vert", nullptr, RES_PATH "shaders/shadow/1.frag");
    Shader sd2(RES_PATH "shaders/shadow/2.vert", nullptr, RES_PATH "shaders/shadow/2.frag");
    Shader sd3(RES_PATH "shaders/shadow/3.vert", nullptr, RES_PATH "shaders/shadow/3.frag");
    //Shader sd3(RES_PATH "", nullptr, RES_PATH "");
    //平行光
    ParallelLight prt{
         .direction = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f)), // 光源方向（指向场景）
        .ambient = glm::vec3(0.1f),
        .diffuse = glm::vec3(0.8f),
        .specular = glm::vec3(1.0f)
    };

    Material mt{
        0, 1, 32.0f
    };

    glm::vec3 lightPos = _vt_pos;
    _window.Run([&](GLFWwindow* window){
        camera.ProcessInput(window, Time::instance->deltatime);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 sc_model = glm::scale(glm::mat4(1.0f), glm::vec3(100.0f, 1.0f, 100.0f));
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 3.0f, 0.0f));
        glm::mat4 model1 = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 0.0));
        glm::mat4 model2 = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 1.0f, 0.0));
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)_window.GetWidth() / _window.GetHeight(), 0.1f, 100.0f);
        glm::mat4 lightMatrix = prt.GetProjectionMatrix() * prt.GetLookAtMatrix();
        glViewport(0, 0, 1024, 1024);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFbo);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        sd3.use();
        sd3.SetMat4("model", model);
        sd3.SetMat4("lightMatrix", lightMatrix);
        cube.Draw(sd3);
        sd3.SetMat4("model", model1);
        cube.Draw(sd3);
        liuyin.Draw(sd3);
        glCullFace(GL_BACK);
        glViewport(0, 0, _window.GetWidth(), _window.GetHeight());
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sd1.use();
        sd1.SetMat4("model", model);
        sd1.SetMat4("view", view);
        sd1.SetMat4("projection", projection);
        sd1.SetVec3("viewPos", camera.position);
        sd1.SetVec3("lightPos", lightPos);
        sd1.SetMat4("lightMatrix", lightMatrix);
        mt.SetUniform("material", sd1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depth);
        sd1.SetInt1("shadowMap", 2);
        prt.SetUniform("pt", sd1);
        cube.Draw(sd1);

        sd1.SetMat4("model", sc_model);
        cube.Draw(sd1);

        sd1.SetMat4("model", model1);
        cube.Draw(sd1);

        sd1.SetMat4("model", model2);
        liuyin.Draw(sd1);
        
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("阴影测试");
        ImGui::DragFloat3("->光源方向", &prt.direction[0], 0.1f);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    });
    return 0;
}

