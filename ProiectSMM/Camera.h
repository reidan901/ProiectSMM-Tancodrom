#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <GL/glew.h>

#include <GLM.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <glfw3.h>

#include <iostream>

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

class Camera
{
private:
    const float zNEAR = 0.1f;
    const float zFAR = 1000.f;
    const float YAW = -90.0f;
    const float PITCH = 0.0f;
    const float FOV = 45.0f;
    glm::vec3 startPosition;

protected:
    const float cameraSpeedFactor = 200.5f;
    const float mouseSensitivity = 0.1f;

    float zNear;
    float zFar;
    float FoVy;
    int width;
    int height;
    bool isPerspective;

    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    bool bFirstMouseMove = true;
    float lastX = 0.f, lastY = 0.f;

public:
    enum EMovementType
    {
        UNKNOWN,
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };
    Camera(const int width, const int height, const glm::vec3& position);
    void Set(const int width, const int height, const glm::vec3& position);
    const glm::mat4 GetViewMatrix() const;
    const glm::mat4 GetProjectionMatrix() const;
    const glm::vec3 GetPosition() const;
    void ProcessKeyboard(EMovementType movementType, float deltaTime);
    void MouseControl(float xPos, float yPos);
    void ProcessMouseScroll(float yOffset);
    void Reshape(int width, int height);
    void Reset(const int width, const int height);

private:
    void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
    void UpdateCameraVectors();
};

