//stone_texture: https://i.pinimg.com/originals/de/54/51/de5451d7a5be01c75ccff84e7198abc4.jpg
//water_texture: https://images.freecreatives.com/wp-content/uploads/2016/12/Transparent-Water-Texture.jpg
//snow_texture: https://opengameart.org/sites/default/files/snow01.png
//grass_texture: https://i.pinimg.com/originals/07/c2/b8/07c2b8bcad9fb6a6efb8d3c62e5f3080.jpg
//PerlinNoise: https://github.com/Reputeless/PerlinNoise
//Loading Textures: https://learnopengl.com/Getting-started/Textures

#include "playground.h"

// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/PerlinNoise.hpp>
#include <common/stb_image.h>
#include <iostream>

double frequency = 4.0;
std::int32_t	octaves = 16;
std::uint32_t	seed = 12345;
const siv::PerlinNoise perlin(seed);
GLuint textures[4];
//unsigned int stoneTexture;
//unsigned int waterTexture;
//unsigned int snowTexture;
//unsigned int grassTexture;
int main(void)
{
	//Initialize window
	bool windowInitialized = initializeWindow();
	if (!windowInitialized) return -1;

	//Initialize vertex buffer
	bool vertexbufferInitialized = initializeVertexbuffer();
	if (!vertexbufferInitialized) return -1;

	glEnable(GL_DEPTH_TEST);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("SimpleVertexShader.vert", "SimpleFragmentShader.frag");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glGenTextures(4, textures);


	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// load and generate the texture
	const char* texture[4] = { "stone_texture.jpg", "water_texture.jpg",
							  "snow_texture.jpg", "grass_texture.jpg" };
	for (int i = 0; i < 4; i++) {
		int width, height, nrChannels;
		unsigned char* data = stbi_load(texture[i], &width, &height, &nrChannels, 0);
		if (data)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}



	initializeMVPTransformation();

	curr_x = 0;
	curr_y = 0;

	//start animation loop until escape key is pressed
	do {

		updateAnimationLoop();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);


	//Cleanup and close window
	cleanupVertexbuffer();
	glDeleteProgram(programID);
	closeWindow();

	return 0;
}

void updateAnimationLoop()
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use our shader
	glUseProgram(programID);

	if (glfwGetKey(window, GLFW_KEY_W)) curr_y += 1.0;
	else if (glfwGetKey(window, GLFW_KEY_S)) curr_y -= 1.0;
	if (glfwGetKey(window, GLFW_KEY_A)) curr_x -= 1.0;
	else if (glfwGetKey(window, GLFW_KEY_D)) curr_x += 1.0;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) altitude += 1.0;
	else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) altitude -= 1.0;
	else if (glfwGetKey(window, GLFW_KEY_SPACE)) initializeVertexbuffer();

	initializeMVPTransformation();

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glUniform1i(glGetUniformLocation(programID, "stoneTexture"), 0);
	glUniform1i(glGetUniformLocation(programID, "waterTexture"), 1);
	glUniform1i(glGetUniformLocation(programID, "snowTexture"), 2);
	glUniform1i(glGetUniformLocation(programID, "grassTexture"), 3);

	glBindVertexArray(vertexbuffer);
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, vertexbuffer_size * 3); // 3 indices starting at 0 -> 1 triangle

	glDisableVertexAttribArray(0);

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	//https://learnopengl.com/Getting-started/Camera
	if (mouse_x_old == NULL)
	{
		mouse_x_old = xpos;
		mouse_y_old = ypos;
	}
	float xoffset = xpos - mouse_x_old;
	float yoffset = mouse_y_old - ypos;
	mouse_x_old = xpos;
	mouse_y_old = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	if (yaw > -89.0f)
		yaw = -89.0f;
	if (yaw < -189.0f)
		yaw = -189.0f;
}
bool initializeWindow()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Demo: Cube", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return false;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	// Dark blue background
	glClearColor(0.4f, 0.4f, 0.6f, 0.0f);
	return true;
}

bool initializeMVPTransformation()
{
	// Get a handle for our "MVP" uniform
	GLuint MatrixIDnew = glGetUniformLocation(programID, "MVP");
	MatrixID = MatrixIDnew;


	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 300.0f);
	//glm::mat4 Projection = glm::frustum(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(0 - curr_x, 20 + altitude, 0 + curr_y), // Camera is at (4,3,-3), in World Space
		glm::vec3(50 - curr_x - cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * 50, 0 + sin(glm::radians(pitch)) * 50, 50 + curr_y - sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * 50), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);

	Model = glm::rotate(Model, curr_angle, glm::vec3(0.0f, 1.0f, 1.0f));

	glm::mat4 transformation;//additional transformation for the model
	transformation[0][0] = 1.0; transformation[1][0] = 0.0; transformation[2][0] = 0.0; transformation[3][0] = curr_x;
	transformation[0][1] = 0.0; transformation[1][1] = 1.0; transformation[2][1] = 0.0; transformation[3][1] = curr_y;
	transformation[0][2] = 0.0; transformation[1][2] = 0.0; transformation[2][2] = 1.0; transformation[3][2] = 0.0;
	transformation[0][3] = 0.0; transformation[1][3] = 0.0; transformation[2][3] = 0.0; transformation[3][3] = 1.0;

	// Our ModelViewProjection : multiplication of our 3 matrices
	MVP = Projection * View * Model;//* transformation; // Remember, matrix multiplication is the other way around


	return true;

}
float getHight(float x, float z)
{
	const double fx = 100 / frequency;
	const double fz = 100 / frequency;
	return perlin.accumulatedOctaveNoise2D_0_1(x / fx, z / fz, octaves) * 20;
	//return z;
}
bool initializeVertexbuffer()
{
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	const int size = 100;
	vertexbuffer_size = 2 * size * size;
	GLfloat g_vertex_buffer_data[2 * size * size * 9];
	int x = 0 - curr_x;
	int z = 0 + curr_y;
	for (size_t i = 0; i < 2 * size * size; i++)
	{
		//Triangle 1
		g_vertex_buffer_data[i * 9 + 0] = (float)x;
		g_vertex_buffer_data[i * 9 + 1] = getHight(x, z);
		g_vertex_buffer_data[i * 9 + 2] = (float)z;

		g_vertex_buffer_data[i * 9 + 3] = (float)x + 1.0;
		g_vertex_buffer_data[i * 9 + 4] = getHight(x + 1.0, z);
		g_vertex_buffer_data[i * 9 + 5] = (float)z;

		g_vertex_buffer_data[i * 9 + 6] = (float)x;
		g_vertex_buffer_data[i * 9 + 7] = getHight(x, z + 1.0);
		g_vertex_buffer_data[i * 9 + 8] = (float)z + 1;
		//Triangle 2		 
		i++;
		g_vertex_buffer_data[i * 9 + 0] = (float)x + 1.0;
		g_vertex_buffer_data[i * 9 + 1] = getHight(x + 1.0, z + 1.0);
		g_vertex_buffer_data[i * 9 + 2] = (float)z + 1;

		g_vertex_buffer_data[i * 9 + 3] = (float)x + 1.0;
		g_vertex_buffer_data[i * 9 + 4] = getHight(x + 1.0, z);
		g_vertex_buffer_data[i * 9 + 5] = (float)z;

		g_vertex_buffer_data[i * 9 + 6] = (float)x;
		g_vertex_buffer_data[i * 9 + 7] = getHight(x, z + 1.0);
		g_vertex_buffer_data[i * 9 + 8] = (float)z + 1.0;
		x++;
		if (x >= size - curr_x)
		{
			x = 0 - curr_x;
			z++;
		}
	}


	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	return true;
}

bool cleanupVertexbuffer()
{
	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	return true;
}

bool closeWindow()
{
	glfwTerminate();
	return true;
}

