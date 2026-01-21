#ifndef _WA_TIME_HPP
#define _WA_TIME_HPP
#include <memory>
#include <GLFW/glfw3.h>
class Time
{
public:
Time(){
    lastTime = static_cast<float>(glfwGetTime());
}
inline void Update();
float deltatime;
static std::unique_ptr<Time>instance;
private:
float lastTime;
};

std::unique_ptr<Time> Time::instance = nullptr;

inline void Time::Update()
{
    float cur = (float)glfwGetTime();
    deltatime = cur - lastTime;
    lastTime = cur;
}

#endif