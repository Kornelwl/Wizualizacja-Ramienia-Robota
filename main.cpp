#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"EBO.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"Texture.h"
#include"Mesh.h"
#include"Model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


const GLuint WIDTH = 800, HEIGHT = 800;

int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.4" << std::endl;
	//Initialize library GLFW
	if (!glfwInit())
		return -1;


	//Version settings
	glfwInitHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwInitHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwInitHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); //mozna ustawic zeby uzytkownik nie zmienia³ rozmiaru okna

	// Vertices coordinates
	// Vertices coordinates
	//Jak coœ te cordy to nie jest x, y ,z jak na matmie tylko z jest jak y i y jest jak z XD
	Vertex vertices[] =
	{ //     COORDINATES       /     COLORS     //    TEX2D
		Vertex{glm::vec3(-5.0f, 0.0f, -5.0f),glm::vec3(0.0f, 0.0f,  0.0f),glm::vec2(0.0f, 2.0f)},// Lower left corner
		Vertex{glm::vec3(5.0f, 0.0f,-5.0f), glm::vec3(0.0f, 0.0f,  0.0f), glm::vec2(2.0f, 2.0f)},// Lower right corner
		Vertex{glm::vec3(5.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f,  0.0f), glm::vec2(2.0f, 0.0f)},// Upper corner
		Vertex{glm::vec3(-5.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)} // Inner left
		 
	};

	// Indices for vertices order
	GLuint indices[] =
	{
		0,1,2,
		0,3,2
	};
	
	//Create window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Wizualizacja Ramienia Robota", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//Viewport settings
	gladLoadGL();
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	glViewport(0,0,WIDTH,HEIGHT);
	//Resizing viewport 
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Textury do pod³ogi
	Texture textures[]{
		//Texture
		Texture("drewno.png", "diffuse", GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE)
	};


	/*VBO – Vertex Buffer Object
	Bufor w GPU, który przechowuje dane wierzcho³ków (pozycje, kolory, UV itd.).*/

	/*VAO – Vertex Array Object
	-Obiekt, który zapamiêtuje ustawienia bindowania VBO i atrybutów wierzcho³ków.
	-Dziêki VAO nie musisz ka¿dorazowo ustawiaæ glVertexAttribPointer.*/
	Shader shaderProgram("default.vert", "default.frag");
	Shader robotshader("robot_arm.vert", "robot_arm.frag");

	std::vector <Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector <GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));
	std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
	// Create floor mesh
	Mesh floor(verts, ind, tex);
	Model robotModel((char*)"Ramie_robota_pokolorowane_wiercholki.glb");
	

	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	glEnable(GL_DEPTH_TEST);
	//DO zobaczenia siatki czy model sie zaladowa³
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Rendering window, swaping buffers so window is not flickery
	glfwSwapBuffers(window);
	
	while (!glfwWindowShouldClose(window))
	{
		//Input
		processInput(window);
		glClearColor(0.27f, 0.55f, 0.35f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// macierz 4x4 dodanie lokalizacji modelu i viewportu gdzie ma sie wyswietlac, i dodanie perspective - perspektywa, punkt widzenia na obrazie, w tym przypadku mamy 45 stopni

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);
		view = glm::lookAt(
			glm::vec3(0.0f, 2.0f, -5.0f), // pozycja kamery (wy¿ej i dalej)
			glm::vec3(0.0f, 0.0f, 0.0f), // patrz na œrodek sceny
			glm::vec3(0.0f, 1.0f, 0.0f)  // „góra” sceny
		);

		proj = glm::perspective(glm::radians(45.0f), (float)(WIDTH / HEIGHT), 0.1f, 100.0f);

		shaderProgram.Activate();
		int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
		// floor
		floor.Draw(shaderProgram);

		// Dla robota
		glm::mat4 model_robot = glm::mat4(1.0f);
		model_robot = glm::translate(model_robot, glm::vec3(0.0f, 0.0f, 0.0f));
		model_robot = glm::scale(model_robot, glm::vec3(0.2f));
		robotshader.Activate();
		int modelLoc_robot = glGetUniformLocation(robotshader.ID, "model");
		glUniformMatrix4fv(modelLoc_robot, 1, GL_FALSE, glm::value_ptr(model_robot));
		int viewLoc_robot = glGetUniformLocation(robotshader.ID, "view");
		glUniformMatrix4fv(viewLoc_robot, 1, GL_FALSE, glm::value_ptr(view));
		int projLoc_robot = glGetUniformLocation(robotshader.ID, "proj");
		glUniformMatrix4fv(projLoc_robot, 1, GL_FALSE, glm::value_ptr(proj));
		robotModel.Draw(robotshader);

		glfwSwapBuffers(window);
		glfwPollEvents(); // obs³uga zdarzeñ (klawiatura, mysz, ...)
	}

	//usuwanie obiektow po zakonczeniu programu
	shaderProgram.Delete();
	robotshader.Delete();

	//zamkniecie okna kuniec
	std::cout << "Exiting program" << std::endl;
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
//Resizing window
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
//ESC - close window
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}