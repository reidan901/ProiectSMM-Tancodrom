#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <math.h> 

#include <stb_image.h>

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
#include <vector>

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

unsigned int loadSkybox(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	stbi_set_flip_vertically_on_load(false);
	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;

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

	// Create camera
	pCamera = new Camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0, 10.0, 13.0));
	Shader modelShader("model.vs", "model.fs");
	Shader skyboxShader("shaderSkybox.vs", "shaderSkybox.fs");
	Shader lampShader("Lamp.vs", "Lamp.fs");

	std::vector<std::vector<Mesh>> meshes;
	std::vector<std::string> paths;
	std::string pathname1 = "../models/finalwall.obj";
	std::string pathname2 = "../models/Landscape-1.obj";
	std::string pathname3 = "../models/tank.obj";
	std::string pathname4 = "../models/tower.obj";
	paths.push_back(pathname1);
	paths.push_back(pathname2);
	paths.push_back(pathname3);
	paths.push_back(pathname4);
	for (int i = 0; i < paths.size(); i++)
	{
		meshes.push_back(Loader::LoadObj(paths[i]));
	}
	
	Model myModel(meshes[0]);
	Model model2(meshes[1]);
	Model tankModel(meshes[2]);
	Model towerModel(meshes[3]);

	//sun
	float verticesSun[] = {
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	   0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	   0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	   0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	   0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	   0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	   0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSun), verticesSun, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glm::vec3 lightPos(0.0f, 1.0f, 0.0f);

	//skybox
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	std::vector<std::string> faces1 {
		"..\\SkyboxDay\\Right.bmp",			//1
			"..\\SkyboxDay\\Left.bmp",		//2
			"..\\SkyboxDay\\Top.bmp",		//3
			"..\\SkyboxDay\\Bottom.bmp",	//4
			"..\\SkyboxDay\\Front.bmp",		//5
			"..\\SkyboxDay\\Back.bmp"		//6
	};

	std::vector<std::string> faces2{
		"..\\SkyboxNight\\1.png",			//1
			"..\\SkyboxNight\\2.png",		//2
			"..\\SkyboxNight\\3.png",		//3
			"..\\SkyboxNight\\4.png",		//4
			"..\\SkyboxNight\\5.png",		//5
			"..\\SkyboxNight\\6.png"		//6
	};

	unsigned int cubemapTexture1 = loadSkybox(faces1);
	unsigned int cubemapTexture2 = loadSkybox(faces2);

	//VBO skybox
	unsigned int skyboxVBO;
	glGenBuffers(1, &skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	//VAO skybox
	unsigned int skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glm::vec3 lightColor;
	lightColor.x = 1;
	lightColor.y = 1;
	lightColor.z = 0;

	double mixValue = 0.0;

	GLuint MixValueLocation;
	MixValueLocation = glGetUniformLocation(skyboxShader.ID, "mixValue");

	glUniform1f(MixValueLocation, mixValue);
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
		
		glDepthMask(GL_FALSE);
		skyboxShader.Use();

		glm::mat4 viewSkybox = glm::mat4(glm::mat3(pCamera->GetViewMatrix()));
		glm::mat4 projectionSkybox = pCamera->GetProjectionMatrix();

		skyboxShader.SetMat4("projection", projectionSkybox);
		skyboxShader.SetMat4("view", viewSkybox);
		skyboxShader.SetFloat("mixValue", mixValue);
		glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);
		glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox2"), 1);
		//glUniform1f(glGetUniformLocation(skyboxShader.ID, "mixValue"), 0.0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture2);
		
		glBindVertexArray(skyboxVAO);
		
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthMask(GL_TRUE);
		glActiveTexture(GL_TEXTURE0);

		static double lightMovementRadius = 300.0f;
		lightPos.y = lightMovementRadius * glm::sin(currentFrame*0.3);
		lightPos.x = lightMovementRadius * glm::cos(currentFrame*0.3);
		//lamp
		/*glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glDrawArrays(GL_TRIANGLES, 0, 36);*/

		lampShader.Use();
		lampShader.SetMat4("projection", pCamera->GetProjectionMatrix());
		lampShader.SetMat4("view", pCamera->GetViewMatrix());
		
		glm::mat4 modelLight = glm::scale(glm::mat4(1.0), glm::vec3(3.0f));

		modelLight = glm::translate(glm::mat4(1.0), lightPos);
		lampShader.SetMat4("model", modelLight);
		
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		modelShader.Use();
		/* light properties*/
		if (glm::cos(currentFrame * 0.3) < 1.f && glm::cos(currentFrame * 0.3) > 0.f )
		{
			if (mixValue < 0)
			{
				mixValue = 0;
			}
			else
			{
				mixValue -= 0.0001;
			}

			if (lightColor.x < 0.858)
			{
				lightColor.x = 0.858;
			}
			else
			{
				lightColor.x = lightColor.x - 0.0001;
			}
			
			if (lightColor.y > 1)
			{
				lightColor.y = 1;
			}
			else
			{
				lightColor.y = lightColor.y + 0.0001;
			}
		}
		else if(glm::cos(currentFrame * 0.3) > -1.f && glm::cos(currentFrame * 0.3) < 0.f)
		{
			if (mixValue > 1)
			{
				mixValue = 1;
			}
			else
			{
				mixValue += 0.0003;
			}
			

			if (lightColor.x > 1)
			{
				lightColor.x = 1;
			}
			else
			{
				lightColor.x = lightColor.x + 0.0001;
			}

			if (lightColor.y < 0.466)
			{
				lightColor.y = 0.466;
			}
			else
			{
				lightColor.y = lightColor.y - 0.0001;
			}
		}
		glm::vec3 diffuseColor = lightColor; // decrease the influence
		glm::vec3 ambientColor = lightColor*glm::vec3(0.1f); // low influence
		modelShader.SetVec3("viewPos", pCamera->GetPosition());
		modelShader.SetVec3("light.ambient", ambientColor);
		modelShader.SetVec3("light.diffuse", diffuseColor);
		modelShader.SetVec3("light.specular", 1.0f, 1.0f, 1.0f);
		modelShader.SetVec3("light.position", lightPos);
		modelShader.SetMat4("projection", pCamera->GetProjectionMatrix());
		modelShader.SetMat4("view", pCamera->GetViewMatrix());
		

		glm::mat4 model1 = glm::scale(glm::mat4(1.0), glm::vec3(10.f, 10.f, 10.f));
		modelShader.SetMat4("model", model1);
	
		myModel.Draw(modelShader);
		glm::mat4 model = glm::scale(glm::mat4(1.0), glm::vec3(20.f));
		modelShader.SetMat4("model", model);
		model2.Draw(modelShader);
		glm::mat4 model2 = glm::scale(glm::mat4(1.0), glm::vec3(9.f));
		modelShader.SetMat4("model", model2);
		tankModel.Draw(modelShader);
		modelShader.SetMat4("model", model2);
		towerModel.Draw(modelShader);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Cleanup();
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