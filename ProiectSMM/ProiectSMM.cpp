#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <math.h>

#include <GL/glew.h>
#include <GLM.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <glfw3.h>

#include <iostream>
#include "Camera.h"

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLuint VaoId, VboId, IboId, ColorBufferId, VertexShaderId, FragmentShaderId, ProgramId;
GLuint ProjMatrixLocation, ViewMatrixLocation, WorldMatrixLocation;

Camera* pCamera = nullptr;

const GLchar* VertexShader =
{
   "#version 400\n"\
   "layout(location=0) in vec4 in_Position;\n"\
   "layout(location=1) in vec4 in_Color;\n"\
   "out vec4 ex_Color;\n"\
   "uniform mat4 ProjMatrix;\n"\
   "uniform mat4 ViewMatrix;\n"\
   "uniform mat4 WorldMatrix;\n"\
   "void main()\n"\
   "{\n"\
   "  gl_Position = ProjMatrix * ViewMatrix * WorldMatrix * in_Position;\n"\
   "  ex_Color = in_Color;\n"\
   "}\n"
};

const GLchar* FragmentShader =
{
   "#version 400\n"\
   "in vec4 ex_Color;\n"\
   "out vec4 out_Color;\n"\
   "void main()\n"\
   "{\n"\
   "  out_Color = ex_Color;\n"\
   "}\n"
};

void CreateVBO()
{
    unsigned int Indices[] = {
       0,1,2,
       0,2,3,
       1,5,6,
       1,6,2,
       5,4,7,
       5,7,6,
       4,0,3,
       4,3,7,
       0,5,1,
       0,4,5,
       3,2,6,
       3,6,7
    };

    GLfloat Vertices[] = {
       0.0f, 0.0f, 1.0f, 1.0f,
       1.0f, 0.0f, 1.0f, 1.0f,
       1.0f, 1.0f, 1.0f, 1.0f,
       0.0f, 1.0f, 1.0f, 1.0f,
       0.0f, 0.0f, 0.0f, 1.0f,
       1.0f, 0.0f, 0.0f, 1.0f,
       1.0f, 1.0f, 0.0f, 1.0f,
       0.0f, 1.0f, 0.0f, 1.0f
    };
    
    GLfloat Colors[] = {
       1.0f, 0.0f, 0.0f, 1.0f,
       0.0f, 1.0f, 0.0f, 1.0f,
       0.0f, 0.0f, 1.0f, 1.0f,
       1.0f, 0.0f, 0.0f, 1.0f,
       0.0f, 1.0f, 0.0f, 1.0f,
       0.0f, 0.0f, 1.0f, 1.0f,
       1.0f, 0.0f, 0.0f, 1.0f,
       0.0f, 1.0f, 0.0f, 1.0f
    };

    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glGenBuffers(1, &ColorBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glGenBuffers(1, &IboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void DestroyVBO()
{
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &ColorBufferId);
    glDeleteBuffers(1, &VboId);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);
}

void CreateShaders()
{
    VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShaderId, 1, &VertexShader, NULL);
    glCompileShader(VertexShaderId);

    FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShaderId, 1, &FragmentShader, NULL);
    glCompileShader(FragmentShaderId);

    ProgramId = glCreateProgram();
    glAttachShader(ProgramId, VertexShaderId);
    glAttachShader(ProgramId, FragmentShaderId);
    glLinkProgram(ProgramId);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    glGetProgramiv(ProgramId, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(ProgramId, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glValidateProgram(ProgramId);
    glGetProgramiv(ProgramId, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ProgramId, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(ProgramId);

    ProjMatrixLocation = glGetUniformLocation(ProgramId, "ProjMatrix");
    ViewMatrixLocation = glGetUniformLocation(ProgramId, "ViewMatrix");
    WorldMatrixLocation = glGetUniformLocation(ProgramId, "WorldMatrix");
}

void DestroyShaders()
{
    glUseProgram(0);

    glDetachShader(ProgramId, VertexShaderId);
    glDetachShader(ProgramId, FragmentShaderId);

    glDeleteShader(FragmentShaderId);
    glDeleteShader(VertexShaderId);

    glDeleteProgram(ProgramId);
}

void Initialize()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // culoarea de fond a ecranului
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    CreateVBO();
    CreateShaders();

    pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.5, 0.5, 10));
}

void RenderCube()
{
    glBindVertexArray(VaoId);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IboId);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
}

void RenderFrame()
{
    glm::vec3 cubePosition[] = {
       glm::vec3(0.0f,  0.0f,   0.0f),
    };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(ProgramId);

    glm::mat4 projection = pCamera->GetProjectionMatrix();
    glUniformMatrix4fv(ProjMatrixLocation, 1, GL_FALSE, glm::value_ptr(projection));

    glm::mat4 view = pCamera->GetViewMatrix();
    glUniformMatrix4fv(ViewMatrixLocation, 1, GL_FALSE, glm::value_ptr(view));

    for (unsigned int i = 0; i < sizeof(cubePosition) / sizeof(cubePosition[0]); i++) {
        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 worldTransf = glm::translate(glm::mat4(1.0), cubePosition[i]);
        glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, glm::value_ptr(worldTransf));
        RenderCube();
    }
}

void Cleanup()
{
    DestroyShaders();
    DestroyVBO();

    delete pCamera;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

double deltaTime = 0.0f;
double lastFrame = 0.0f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "WorldOfTanks", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glewInit();

    Initialize();

    while (!glfwWindowShouldClose(window)) {
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        RenderFrame();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    Cleanup();

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        pCamera->ProcessKeyboard(Camera::EMovementType::EForward, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        pCamera->ProcessKeyboard(Camera::EMovementType::EBackward, deltaTime);

    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        pCamera->ProcessKeyboard(Camera::EMovementType::ELeft, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        pCamera->ProcessKeyboard(Camera::EMovementType::ERight, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
        pCamera->ProcessKeyboard(Camera::EMovementType::EPageUp, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
        pCamera->ProcessKeyboard(Camera::EMovementType::EPageDown, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        pCamera->Set(800, 600, glm::vec3(0.5, 0.5, 10));
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    pCamera->Reshape(width, height);
}



void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    pCamera->MouseControl((float)xpos, (float)ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yOffset)
{
    pCamera->ProcessMouseScroll((float)yOffset);
}
