#ifndef _WA_CAMERA_HPP
#define _WA_CAMERA_HPP
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

constexpr float DEFAULT_SENSITIVITY = 0.1f;
constexpr float DEFAULT_PCH = 0.0f;
constexpr float DEFAULT_YAW = -90.0f;
constexpr float DEFAULT_ZOOM = 45.0f;
constexpr float DEFAULT_SPEED = 2.5f;


class Camera
{
public:
Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), 
glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f),
float sensitivity = DEFAULT_SENSITIVITY,
float pitch = DEFAULT_PCH,
float yaw = DEFAULT_YAW,
float zoom = DEFAULT_ZOOM,
float speed = DEFAULT_SPEED
):position(position), worldUp(worldUp), sensitivity(sensitivity), pitch(pitch), yaw(yaw), zoom(zoom),
speed(speed) 
{
    UpdateCameraVectors();
}

inline void ProcessInput(GLFWwindow* window, float deltatime);
inline void ProcessZoom(float yOffset);
inline void ProcessLensMove(float xOffset, float yOffset);
glm::mat4 GetViewMatrix(){ return glm::lookAt(position, position + front, worldUp); } 
public:
glm::vec3 position;
glm::vec3 front;
glm::vec3 up;
glm::vec3 right;
glm::vec3 worldUp;
float sensitivity;
float pitch;
float yaw;
float zoom;
float speed;
private:
inline void UpdateCameraVectors();

};


inline void Camera::UpdateCameraVectors()
{
    /*
    * front.xyz, x,z受到pitch和yaw影响
    * y受到pitch影响
    */
    front.y = glm::sin(glm::radians(pitch));
    front.z = glm::cos(glm::radians(pitch)) * glm::sin(glm::radians(yaw));
    front.x = glm::cos(glm::radians(pitch)) * glm::cos(glm::radians(yaw));
    front = glm::normalize(front); 
    //front->worldUp = right
    right = glm::normalize(glm::cross(front, worldUp));
    //right->front = up
    up = glm::normalize(glm::cross(right, front));

}

inline void Camera::ProcessInput(GLFWwindow* window, float deltatime)
{
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        position += front * speed * deltatime;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        position -= front * speed * deltatime;
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        position -= right * speed * deltatime;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        position += right * speed * deltatime;
    }
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        position -= up * speed * deltatime;
    }
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        position += up * speed * deltatime;
    }

}
inline void Camera::ProcessZoom(float yOffset)
{
    if(zoom < 1.0f)
        zoom = 1.0f;
    if(zoom > 45.0f)
        zoom = 45.0f;
    zoom -= yOffset;
}
inline void Camera::ProcessLensMove(float xOffset, float yOffset)
{
    //避免窗口抖动
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    pitch += yOffset;
    yaw += xOffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;
   UpdateCameraVectors();
}

#endif