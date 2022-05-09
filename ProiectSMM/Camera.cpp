#include "Camera.h"

Camera::Camera(const int width, const int height, const glm::vec3& position)
{
	startPosition = position;
	Set(width, height, position);
}

void Camera::Set(const int width, const int height, const glm::vec3& position)
{
    this->isPerspective = true;
    this->yaw = YAW;
    this->pitch = PITCH;

    this->FoVy = FOV;
    this->width = width;
    this->height = height;
    this->zNear = zNEAR;
    this->zFar = zFAR;

    this->worldUp = glm::vec3(0, 1, 0);
    this->position = position;

    lastX = width / 2.0f;
    lastY = height / 2.0f;
    bFirstMouseMove = true;

    UpdateCameraVectors();
}

void Camera::Reset(const int width, const int height)
{
    Set(width, height, startPosition);
}

const glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(position, position + forward, up);
}

const glm::mat4 Camera::GetProjectionMatrix() const
{
    glm::mat4 Proj = glm::mat4(1);
    if (isPerspective) {
        float aspectRatio = ((float)(width)) / height;
        Proj = glm::perspective(glm::radians(FoVy), aspectRatio, zNear, zFar);
    }
    else {
        float scaleFactor = 2000.f;
        Proj = glm::ortho<float>(
            -width / scaleFactor, width / scaleFactor,
            -height / scaleFactor, height / scaleFactor, -zFar, zFar);
    }
    return Proj;
}

const glm::vec3 Camera::GetPosition() const
{
    return position;
}

void Camera::ProcessKeyboard(EMovementType movementType, float deltaTime)
{
    float velocity = deltaTime * cameraSpeedFactor;

    switch (movementType)
    {
    case Camera::UNKNOWN:
        break;
    case Camera::FORWARD:
        this->position += this->forward * velocity;
        break;
    case Camera::BACKWARD:
        this->position -= this->forward * velocity;
        break;
    case Camera::RIGHT:
        this->position += this->right * velocity;
        break;
    case Camera::LEFT:
        this->position -= this->right * velocity;
        break;
    case Camera::UP:
        this->position += this->up * velocity;
        break;
    case Camera::DOWN:
        this->position -= this->up * velocity;
        break;
    default:
        break;
    }
}

void Camera::MouseControl(float xPos, float yPos)
{
    if (bFirstMouseMove) {
        lastX = xPos;
        lastY = yPos;
        bFirstMouseMove = false;
    }

    float xChange = xPos - lastX;
    float yChange = lastY - yPos;
    lastX = xPos;
    lastY = yPos;
    if (fabs(xChange) <= 1e-6 && fabs(yChange) <= 1e-6) {
        return;
    }
    xChange *= mouseSensitivity;
    yChange *= mouseSensitivity;
    ProcessMouseMovement(xChange, yChange);
}

void Camera::ProcessMouseScroll(float yOffset)
{
    FoVy -= yOffset;
}

void Camera::Reshape(int width, int height)
{
    this->width = width;
    this->height = height;
    glViewport(0, 0, width, height);
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch)
{
    yaw += xOffset;
    pitch += yOffset;

    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
    this->forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->forward.y = sin(glm::radians(pitch));
    this->forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    this->forward = glm::normalize(this->forward);
    right = glm::normalize(glm::cross(forward, worldUp));
    up = glm::normalize(glm::cross(right, forward));
}
