#include <iostream>
#include <math.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Object.h"
#include <stdlib.h> 
#include <time.h> 

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
unsigned int createShader(const char* filename, const char* type);
unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader);
void LoadTexture(unsigned int& texture, const char* tFileName);
glm::mat4 getPerspective();
glm::mat4 getView();
unsigned int ModelVAO(Object* Model);
void DrawModel(const char* target, glm::mat4 M);

Object* catModel = new Object("obj/cat.obj");
Object* boxModel = new Object("obj/CardboardBox1.obj");

int windowWidth = 800, windowHeight = 600;

unsigned int boxVAO, catVAO;
double now = 0, lasttime = 0;
float degree = 0.0f, sec = 0.0f;
glm::vec3 press_1 = glm::vec3(1, 1, 1);
glm::vec3 press_2 = glm::vec3(1, 1, 1);
glm::vec3 press_3 = glm::vec3(1, 1, 1);
bool effect1 = false, effect2 = false, effect3 = false, max_limit_scale = false, min_limit_color = false;
float x = 1.0, y = 1.0, z = 1.0;

int main()
{
	// Initialization
	srand(time(NULL));
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFE_OPENGL_FORWARD_COMPACT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "HW2", NULL, NULL);
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

	// Shaders
	unsigned int vertexShader, fragmentShader, shaderProgram;
	vertexShader = createShader("vertexShader.vert", "vert");
	fragmentShader = createShader("fragmentShader.frag", "frag");
	shaderProgram = createProgram(vertexShader, fragmentShader);
	glUseProgram(shaderProgram);

	// Texture
	unsigned int catTexture, boxTexture;
	LoadTexture(catTexture, "obj/Cat_diffuse.jpg");
	LoadTexture(boxTexture, "obj/CardboardBox1_RMA.tga");

	// TO DO:
	// Create VAO, VBO
	catVAO = ModelVAO(catModel);
	boxVAO = ModelVAO(boxModel);

	// Display loop
	// TO DO:
	// Draw the models with shaders
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);
	while (!glfwWindowShouldClose(window))
	{
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = getView();
		glm::mat4 prespective = getPerspective();
		unsigned int mLoc, vLoc, pLoc, sLoc, cLoc, bLoc;
		mLoc = glGetUniformLocation(shaderProgram, "M");
		vLoc = glGetUniformLocation(shaderProgram, "V");
		pLoc = glGetUniformLocation(shaderProgram, "P");
		sLoc = glGetUniformLocation(shaderProgram, "S");
		cLoc = glGetUniformLocation(shaderProgram, "catColor");
		bLoc = glGetUniformLocation(shaderProgram, "boxColor");
		glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(prespective));
		
		//BOX
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, boxTexture);
		model = glm::scale(model, glm::vec3(0.0625, 0.05, 0.05));
		model = glm::rotate(model, glm::radians(degree), glm::vec3(0, 1, 0));
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(model));

		glm::vec3 scale = glm::vec3(1, 1, 1);
		glm::vec3 catColor = glm::vec3(1, 1, 1);
		glm::vec3 boxColor = press_3;
		glUniform3fv(sLoc, 1, glm::value_ptr(scale));
		glUniform3fv(cLoc, 1, glm::value_ptr(catColor));
		glUniform3fv(bLoc, 1, glm::value_ptr(boxColor));
		glBindVertexArray(boxVAO);
		glDrawArrays(GL_TRIANGLES, 0, boxModel->positions.size());
		glBindVertexArray(0);
		//---------------------//
		
		//CAT
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, catTexture);

		model = glm::scale(model, glm::vec3(16, 20, 20));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(model));
	
		scale = press_1;
		catColor = press_2;
		boxColor = glm::vec3(1, 1, 1);
		glUniform3fv(sLoc, 1, glm::value_ptr(scale));
		glUniform3fv(cLoc, 1, glm::value_ptr(catColor));
		glUniform3fv(bLoc, 1, glm::value_ptr(boxColor));
		glBindVertexArray(catVAO);
		glDrawArrays(GL_TRIANGLES, 0, catModel->positions.size());
		glBindVertexArray(0);
		//---------------------//

		glfwSwapBuffers(window);
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		now = glfwGetTime();
		double time = now - lasttime;
		lasttime = now;
		degree += time * 90.0f;

		if (effect1)
		{
			float limit_max_scale = 1.3, limit_min_scale = 0.5;
			if (!max_limit_scale)
			{
				press_1.x -= (time / 8);
				press_1.y += (time / 5);
				if (press_1.y >= limit_max_scale)
					max_limit_scale = true;
			}
			else
			{
				press_1.x += (time / 8);
				press_1.y -= (time / 5);
				if (press_1.y <= limit_min_scale)
					max_limit_scale = false;
			}
		}
		if (effect2)
		{
			float limit_max_color = 1, limit_min_color = 0.33;
			if (!min_limit_color)
			{
				press_2.x -= (time / 6); //0.33
				press_2.y -= (time / 8); //0.21
				press_2.z -= (time / 32); //0.06
				if (press_2.x <= limit_min_color)
					min_limit_color =true;
			}
			else
			{
				press_2.x += (time / 6);
				press_2.y += (time / 8);
				press_2.z += (time / 32);
				if (press_2.x >= limit_max_color)
					min_limit_color = false;
			}
		}
		if (effect3)
		{
			float r, g, b;
			do 
			{
				r = (float)rand() / (float)RAND_MAX;
				g = (float)rand() / (float)RAND_MAX;
				b = (float)rand() / (float)RAND_MAX;
			} while (r > 0 && b > 0 && b > 0);
			
			if ((int)now%2 == 0)
				x = r, y = g, z = b;
				
			if (press_3.x < x)
				press_3.x += (time / (2 / x));
			else 
				press_3.x -= (time / (2 / x));
			if (press_3.y < y)
				press_3.y += (time / (2 / y));
			else
				press_3.y -= (time / (2 / y));
			if (press_3.z < z)
				press_3.z += (time / (2 / z));
			else
				press_3.z -= (time / (2 / z));
		}
	}
	glfwTerminate();
	return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	else if (action == GLFW_PRESS)
	{
		cout << "KEY " << char(key) << " PRESSED" << endl;
		if (key == GLFW_KEY_1)
		{
			if (effect1 == false) effect1 = true;
			else effect1 = false;
		}
		else if (key == GLFW_KEY_2)
		{
			if (effect2 == false) effect2 = true;
			else effect2 = false;
		}
		else if (key == GLFW_KEY_3)
		{
			if (effect3 == false) effect3 = true;
			else effect3 = false;
		}
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
		cout << "ERROR::SHADER::" << type << "::COMPLIATION_FAILED\n" << infoLog << endl;
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

void LoadTexture(unsigned int& texture, const char* tFileName) {
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
		0.1f, 100.0f);
}

glm::mat4 getView()
{
	return glm::lookAt(
		glm::vec3(0, 5, 5),
		glm::vec3(0, 0, 0),
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