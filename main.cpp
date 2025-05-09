#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>

#include"EBO.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"Texture.h"

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
	GLfloat vertices[] =
	{ //     COORDINATES       /     COLORS     //
		-0.5f, -0.5f, 0.0f,     0.0f, 0.0f,  0.0f,	1.0f, 0.0f,			// Lower left corner
		 0.5f, -0.5f, 0.0f,     0.0f, 0.0f,  0.0f,  0.0f, 0.0f,// Lower right corner
		 0.5f,  0.5f, 0.0f,    0.0f, 0.0f,  0.0f,	0.0f, 1.0f,// Upper corner
		-0.5f, 0.5f, 0.0f,     0.0f, 0.0f, 0.0f,	1.0f, 1.0f// Inner left
		 
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
	glViewport(0,0,800,800);


	/*VBO – Vertex Buffer Object
	Bufor w GPU, który przechowuje dane wierzcho³ków (pozycje, kolory, UV itd.).*/

	/*VAO – Vertex Array Object
	-Obiekt, który zapamiêtuje ustawienia bindowania VBO i atrybutów wierzcho³ków.
	-Dziêki VAO nie musisz ka¿dorazowo ustawiaæ glVertexAttribPointer.*/
	Shader shaderProgram("default.vert", "default.frag");
	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(vertices, sizeof(vertices));
	EBO EBO1(indices, sizeof(indices));

	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3*sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");


	//Texture
	Texture zaloga("zaloga.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	zaloga.texUnit(shaderProgram,"tex0", 0);


	glfwSwapBuffers(window);
	
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.043f, 0.239f, 0.239f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		shaderProgram.Activate();
		glUniform1f(uniID, 0.5f);
		zaloga.Bind();
		VAO1.Bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
		glfwPollEvents(); // obs³uga zdarzeñ (klawiatura, mysz, ...)
	}

	//usuwanie obiektow po zakonczeniu programu
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	zaloga.Delete();
	shaderProgram.Delete();

	//zamkniecie okna kuniec
	std::cout << "Exiting program" << std::endl;
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}