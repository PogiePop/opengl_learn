#ifndef _WA_WINDOW_HPP
#define _WA_WINDOW_HPP
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <time.hpp>
#include <functional>
#include <memory>
#include <string>
static const int DEFAULT_WIDTH = 800;
static const int DEFAULT_HEIGHT = 600;
auto glfwWindowDeleter = [](GLFWwindow *window) -> auto
{
    if (window)
    {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
};
class Window
{
public:
    Window(int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT, const std::string &title = "opengl") { Init(width, height, title); }
    ~Window();
    using GLFWwindowPtr = std::unique_ptr<GLFWwindow, decltype(glfwWindowDeleter)>;
    using WindowSizeCallback = std::function<void(GLFWwindow *, int, int)>;
    using CursorPosCallback = std::function<void(GLFWwindow *, float, float)>;
    using ScrollPosCallback = std::function<void(GLFWwindow *, float)>;
    using WindowEventCallback = std::function<void(GLFWwindow *)>;
    inline void Run(WindowEventCallback callback);
    inline void SetEventCallback(WindowSizeCallback callback);
    inline void SetEventCallback(CursorPosCallback callback);
    inline void SetEventCallback(ScrollPosCallback callback);
    inline void SetEventProxy();
    int GetWidth(){ return m_Width; }
    int GetHeight(){ return m_Height; }
private:
    inline void Init(int width, int height, const std::string &title);
    GLFWwindowPtr m_Window;
    WindowSizeCallback m_SizeCall;
    CursorPosCallback m_CursorCall;
    ScrollPosCallback m_ScrollCall;
    int m_Width, m_Height;
    bool firstMouse = true, isMouseLeft = false;
    float lastX = 0.0f, lastY = 0.0f;
};

Window::~Window()
{
    if (!m_Window || !m_Window.get())
        glfwDestroyWindow(m_Window.get());
}

inline void Window::Init(int width, int height, const std::string &title)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_Window = GLFWwindowPtr(glfwCreateWindow(width, height, title.c_str(), NULL, NULL), glfwWindowDeleter);
    if (!m_Window.get())
    {
        glfwTerminate();
        std::println("create window error");
        return;
    }
    glfwMakeContextCurrent(m_Window.get());

    // 获取地址
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        std::println("load proc error");
        return;
    }
    glViewport(0, 0, width, height);

    //开启垂直同步
    glfwSwapInterval(1);

    // 设置回调函数
    SetEventProxy();

    // 设置用户数据
    glfwSetWindowUserPointer(m_Window.get(), this);

    // 开始计时
    Time::instance = std::make_unique<Time>();
}

inline void Window::Run(WindowEventCallback callback)
{
    while (!glfwWindowShouldClose(m_Window.get()))
    {
        Time::instance->Update();
        if (callback)
            callback(m_Window.get());
        if (glfwGetKey(m_Window.get(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(m_Window.get(), GLFW_TRUE);
        glfwSwapBuffers(m_Window.get());
        glfwPollEvents();
    }
}

inline void Window::SetEventCallback(WindowSizeCallback callback)
{
    if(callback)m_SizeCall = callback;
}
inline void Window::SetEventCallback(CursorPosCallback callback)
{
    if(callback)m_CursorCall = callback;
}
inline void Window::SetEventCallback(ScrollPosCallback callback)
{
    if(callback)m_ScrollCall = callback;
}
inline void Window::SetEventProxy()
{

    glfwSetFramebufferSizeCallback(m_Window.get(), [](GLFWwindow* window, int width, int height){
        //获取窗口数据
        Window* self = (Window*)glfwGetWindowUserPointer(window);
        if(self && self->m_SizeCall)
        {
            self->m_Width = width;
            self->m_Height = height;
            self->m_SizeCall(window, width, height);
            //std::printf("window size: %dX%d\n", width, height);
        }       
    });

    glfwSetCursorPosCallback(m_Window.get(), [](GLFWwindow* window, double xPos, double yPos){
         //获取窗口数据
        Window* self = (Window*)glfwGetWindowUserPointer(window);
        if(self && self->m_CursorCall && self->isMouseLeft)
        {
            //计算x, y方向偏移
            if(self->firstMouse)
            {
                self->lastX = (float)xPos;
                self->lastY = (float)yPos;  
                self->firstMouse = false;  
            }
            float xOffset = (float)(xPos - self->lastX);
            float yOffset = (float)(self->lastY - yPos);
            self->lastX = (float)xPos;
            self->lastY = (float)yPos; 
            self->m_CursorCall(window, xOffset, yOffset);
        }       
    });

    glfwSetScrollCallback(m_Window.get(), [](GLFWwindow* window, double xOffset, double yOffset){
        //获取窗口数据
        Window* self = (Window*)glfwGetWindowUserPointer(window);
        if(self && self->m_ScrollCall)
        {
            self->m_ScrollCall(window, (float)yOffset);
            //std::printf("window size: %dX%d\n", width, height);
        }       
    });

    glfwSetMouseButtonCallback(m_Window.get(), [](GLFWwindow* window, int button, int action, int mods){
        //获取窗口数据
        Window* self = (Window*)glfwGetWindowUserPointer(window);
        if(self)
        {
            if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)self->isMouseLeft = true;
            else 
            {
                self->isMouseLeft = false;
                self->firstMouse = true;
            }
        }    
    });

}

#endif