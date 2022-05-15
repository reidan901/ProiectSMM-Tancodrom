#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <math.h> 

#include <GL/glew.h>

#include <GLM.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include "Camera.h"
#include <vector>
#include "Shader.h"
#include "Loader.h"
#include "Mesh.h"
#include "Model.h"

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

GLuint ProjMatrixLocation, ViewMatrixLocation, WorldMatrixLocation;
Camera* pCamera = nullptr;
float Ka = 0.9f;
float Kd = 0.9f;
float Ks = 0.9f;
float n = 1.f;
unsigned int SCR_WIDTH=800, SCR_HEIGHT=600;

void Cleanup()
{
	delete pCamera;
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// timing
double deltaTime = 0.0f;	// time between current frame and last frame
double lastFrame = 0.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		pCamera->Reset(width, height);

	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && Ka < 1.f)
	{
		Ka = Ka + 0.1f;
		std::cout << Ka << '\n';
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && Ka > 0.2f)
	{
		Ka = Ka - 0.1f;
		std::cout << Ka << '\n';
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && Kd < 1.f)
	{
		Kd = Kd + 0.1f;
		std::cout << Kd << '\n';
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && Kd > 0.2f)
	{
		Kd = Kd - 0.1f;
		std::cout << Kd << '\n';
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && Ks < 1.f)
	{
		Ks = Ks + 0.1f;
		std::cout << Ks << '\n';
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && Ks > 0.2f)
	{
		Ks = Ks - 0.1f;
		std::cout << Ks << '\n';
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && n < 256)
	{
		n *= 2;
		std::cout << n << '\n';
	}
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && n != 1)
	{
		n /= 2;
		std::cout << n << '\n';
	}
}

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tancodrom", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewInit();

	glEnable(GL_DEPTH_TEST);

	//// set up vertex data (and buffer(s)) and configure vertex attributes
	//float vertices[] = {
	//   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	//   0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	//   0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	//   0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	//   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	//   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	//   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	//   0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	//   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	//   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	//   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	//   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	//   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	//   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	//   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	//   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	//   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	//   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	//   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	//   0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	//   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	//   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	//   0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	//   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	//   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	//   0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	//   0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	//   0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	//   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	//   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	//   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	//   0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	//   0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	//   0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	//   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	//   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	//};
	//// first, configure the cube's VAO (and VBO)
	//unsigned int VBO, cubeVAO;
	//glGenVertexArrays(1, &cubeVAO);
	//glGenBuffers(1, &VBO);

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindVertexArray(cubeVAO);

	//// position attribute
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//// normal attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	//// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	//unsigned int lightVAO;
	//glGenVertexArrays(1, &lightVAO);
	//glBindVertexArray(lightVAO);

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);

	// Create camera
	pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0, 10.0, 13.0));

	glm::vec3 lightPos(0.0f, 0.0f, 2.0f);

	Shader lightingShader("model.vs", "model.fs");
	//Shader lampShader("Lamp.vs", "Lamp.fs");
	std::vector<std::vector<Mesh>> meshes;
	std::vector<std::string> paths;
	std::string pathname1 = "Tiger_I.obj";
	std::string pathname2 = "field.obj";
	//paths.push_back(pathname1);
	paths.push_back(pathname2);
	for (int i = 0; i < paths.size(); i++)
	{
		meshes.push_back(Loader::LoadObj( paths[i]));
	}
	
	//Model myModel(meshes[0]);
	Model model2(meshes[0]);

	// render loop
	while (!glfwWindowShouldClose(window)) {
		// per-frame time logic
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightingShader.Use();
		/*lightingShader.SetVec3("objectColor", 0.5f, 1.0f, 0.31f);
		lightingShader.SetVec3("lightColor", 1.0f, 1.0f, 1.0f);
		lightingShader.SetVec3("lightPos", lightPos);
		lightingShader.SetVec3("viewPos", pCamera->GetPosition());
		lightingShader.setFloat("Ka", Ka);
		lightingShader.setFloat("Kd", Kd);
		lightingShader.setFloat("Ks", Ks);
		lightingShader.setFloat("n", n);*/


		lightingShader.SetMat4("projection", pCamera->GetProjectionMatrix());
		lightingShader.SetMat4("view", pCamera->GetViewMatrix());

		glm::mat4 model = glm::scale(glm::mat4(1.0), glm::vec3(33.0f));
		lightingShader.SetMat4("model", model);
		//myModel.Draw(lightingShader);
		model2.Draw(lightingShader);
		//// render the cube
		//glBindVertexArray(cubeVAO);
		//glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		//static float lightMovementRadius = 1.0f;
		//lightPos.x = glm::sin(currentFrame) * lightMovementRadius;
		//lightPos.y = glm::cos(currentFrame) * lightMovementRadius;

		//// also draw the lamp object
		//lampShader.Use();
		//lampShader.SetMat4("projection", pCamera->GetProjectionMatrix());
		//lampShader.SetMat4("view", pCamera->GetViewMatrix());
		//model = glm::translate(glm::mat4(1.0), lightPos);
		//model = glm::scale(model, glm::vec3(0.05f)); // a smaller cube
		//lampShader.SetMat4("model", model);

		//glBindVertexArray(lightVAO);
		//glDrawArrays(GL_TRIANGLES, 0, 36);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Cleanup();

	/*glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);*/

	// glfw: terminate, clearing all previously allocated GLFW resources
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		pCamera->ProcessKeyboard(Camera::EMovementType::FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		pCamera->ProcessKeyboard(Camera::EMovementType::BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		pCamera->ProcessKeyboard(Camera::EMovementType::LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		pCamera->ProcessKeyboard(Camera::EMovementType::RIGHT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
		pCamera->ProcessKeyboard(Camera::EMovementType::UP, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
		pCamera->ProcessKeyboard(Camera::EMovementType::DOWN, (float)deltaTime);


}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
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

//----------------------------