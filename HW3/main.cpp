#include <iostream>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Object.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void loadMaterialLight();
unsigned int createShader(const char* filename, const char* type);
unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader);
void loadTexture(unsigned int& texture, const char* tFileName);
glm::mat4 getPerspective();
glm::mat4 getView();

unsigned int ModelVAO(Object* Model);
void DrawModel(const char* target, glm::mat4 M);

Object* catModel = new Object("obj/cat_l25.obj");

Material material;
Light light;
glm::vec3 cameraPos = glm::vec3(0, 3.5, 3.5);

int windowWidth = 800, windowHeight = 600;
int phongProgram, toonProgram, edgeProgram, gouraudProgram, currentProgram;

unsigned int catVAO;
double lasttime = 0;
float degree = 0.0f;

int main()
{
	// Initialization
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFE_OPENGL_FORWARD_COMPACT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "HW3", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// TODO:
	// Create shaders
	unsigned int vertexShader, fragmentShader;
	vertexShader = createShader("shaders/Phong.vert", "vert");
	fragmentShader = createShader("shaders/Phong.frag", "frag");
	phongProgram = createProgram(vertexShader, fragmentShader);

	vertexShader = createShader("shaders/Gouraud.vert", "vert");
	fragmentShader = createShader("shaders/Gouraud.frag", "frag");
	gouraudProgram = createProgram(vertexShader, fragmentShader);
	

	vertexShader = createShader("shaders/Toon.vert", "vert");
	fragmentShader = createShader("shaders/Toon.frag", "frag");
	toonProgram = createProgram(vertexShader, fragmentShader);
	
	vertexShader = createShader("shaders/Edge.vert", "vert");
	fragmentShader = createShader("shaders/Edge.frag", "frag");
	edgeProgram = createProgram(vertexShader, fragmentShader);

	currentProgram = phongProgram;
	glUseProgram(currentProgram);

	// Texture
	unsigned int catTexture;
	loadTexture(catTexture, "obj/Cat_diffuse.jpg");

	// VAO, VBO
	catVAO = ModelVAO(catModel);

	// Display loop
	loadMaterialLight();
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glUniform1i(glGetUniformLocation(currentProgram, "ourTexture"), 0);
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.4f, 0.2f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// TODO:
		// Draw the cat with current active shader
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, catTexture);
		glm::mat4 view = getView();
		glm::mat4 prespective = getPerspective();

		unsigned int mLoc, vLoc, pLoc, cLoc;
		mLoc = glGetUniformLocation(currentProgram, "M");
		vLoc = glGetUniformLocation(currentProgram, "view");
		pLoc = glGetUniformLocation(currentProgram, "prespective");
		cLoc = glGetUniformLocation(currentProgram, "cameraPos");
		glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(prespective));
		glUniform3fv(cLoc, 1, glm::value_ptr(cameraPos));
		
		unsigned int KaLoc, KdLoc, KsLoc, glossLoc;
		KaLoc = glGetUniformLocation(currentProgram, "Ka");
		KdLoc = glGetUniformLocation(currentProgram, "Kd");
		KsLoc = glGetUniformLocation(currentProgram, "Ks");
		glossLoc = glGetUniformLocation(currentProgram, "gloss");		
		glUniform3fv(KaLoc, 1, glm::value_ptr(material.ambient));
		glUniform3fv(KdLoc, 1, glm::value_ptr(material.diffuse));
		glUniform3fv(KsLoc, 1, glm::value_ptr(material.specular));
		glUniform1f(glossLoc, material.gloss);
		
		unsigned int LaLoc, LdLoc, LsLoc, lightLoc;
		LaLoc = glGetUniformLocation(currentProgram, "La");
		LdLoc = glGetUniformLocation(currentProgram, "Ld");
		LsLoc = glGetUniformLocation(currentProgram, "Ls");
		lightLoc = glGetUniformLocation(currentProgram, "lightPos");
		glUniform3fv(LaLoc, 1, glm::value_ptr(light.ambient));
		glUniform3fv(LdLoc, 1, glm::value_ptr(light.diffuse));
		glUniform3fv(LsLoc, 1, glm::value_ptr(light.specular));
		glUniform3fv(lightLoc, 1, glm::value_ptr(light.position));

		//Draw 
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(degree), glm::vec3(0, 1, 0));
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(catVAO);
		glDrawArrays(GL_TRIANGLES, 0, catModel->positions.size());
		//Time
		double now = glfwGetTime();
		double time = now - lasttime;
		lasttime = now;
		degree += time * 45.0f;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

// TODO:
// Add key callback to switch between shaders
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		currentProgram = phongProgram;
		glUseProgram(currentProgram);
	}
		
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		currentProgram = gouraudProgram;
		glUseProgram(currentProgram);
	}
		
	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		currentProgram = toonProgram;
		glUseProgram(currentProgram);
	}
		
	if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
		currentProgram = edgeProgram;
		glUseProgram(currentProgram);
	}
		
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	windowWidth = width;
	windowHeight = height;
}

unsigned int createShader(const char* filename, const char* type)
{
	unsigned int shader;
	if (strcmp(type, "vert") == 0)
		shader = glCreateShader(GL_VERTEX_SHADER);
	else if (strcmp(type, "frag") == 0)
		shader = glCreateShader(GL_FRAGMENT_SHADER);
	else
	{
		cout << "Unknown Shader Type.\n";
		return 0;
	}

	FILE* fp = fopen(filename, "rb");
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);  //same as rewind(fp);

	char* source = (char*)malloc(sizeof(char) * (fsize + 1));
	fread(source, fsize, 1, fp);
	fclose(fp);

	source[fsize] = 0;

	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		cout << "ERROR::SHADER::" << filename << "::COMPLIATION_FAILED\n" << infoLog << endl;
		return -1;
	}

	return shader;
}

unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader)
{
	unsigned int program = glCreateProgram();

	//Attach our shaders to our program
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	//Link our program
	glLinkProgram(program);

	//Note the different functions here: glGetProgram* instead of glGetShader*.
	int success = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success)
	{
		int maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		char* infoLog = (char*)malloc(sizeof(char) * (maxLength));
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);

		//We don't need the program anymore.
		glDeleteProgram(program);
		//Don't leak shaders either.
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		puts(infoLog);
		free(infoLog);

		return -1;
	}

	//Always detach shaders after a successful link.
	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

	return program;
}

void loadMaterialLight() {
	material.ambient = glm::vec3(1.0, 1.0, 1.0);
	material.diffuse = glm::vec3(1.0, 1.0, 1.0);
	material.specular = glm::vec3(0.7, 0.7, 0.7);
	material.gloss = 10.5;

	light.ambient = glm::vec3(0.2, 0.2, 0.2);
	light.diffuse = glm::vec3(0.8, 0.8, 0.8);
	light.specular = glm::vec3(0.5, 0.5, 0.5);
	light.position = glm::vec3(10, 10, 10);
}

void loadTexture(unsigned int& texture, const char* tFileName) {
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(tFileName, &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);
}

glm::mat4 getPerspective()
{
	return glm::perspective(
		glm::radians(45.0f),
		(float)windowWidth / (float)windowHeight, 
		1.0f, 100.0f);
}

glm::mat4 getView()
{
	return glm::lookAt(
		cameraPos,
		glm::vec3(0, 0.5, 0),
		glm::vec3(0, 1, 0));
}

unsigned int ModelVAO(Object* model)
{
	unsigned int VAO, VBO[3];
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(3, VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * (model->positions.size()), &(model->positions[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * (model->normals.size()), &(model->normals[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * (model->texcoords.size()), &(model->texcoords[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, 0);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	return VAO;
}