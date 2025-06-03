#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <cmath>

#include"EBO.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"Texture.h"
#include"Mesh.h"
#include"Model.h"
#include"Camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
bool check_area_xz_axis(float grabberlink_location_x, float grabberlink_location_z);

const GLuint WIDTH = 800, HEIGHT = 800;
//zainicjowanie kamery
Camera camera(glm::vec3(0.0f, 1.0f, 5.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT/ 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

//rotation for arms
float rotationBaseAngle = 0.0f;
float rotationArm2Angle = 0.0f;
float rotationArm3Angle = 0.0f;
float grabber_movement = 0.0f;

//location for grabber
float grabber_location = 0.0f;
float grabberlink_location_x = 0.0f;
float grabberlink_location_y = 0.0f;
float grabberlink_location_z = 0.0f;
float arm3_location_x = 0.0f;
float arm3_location_y = 0.0f;
float arm3_location_z = 0.0f;

// Rotation limits (in degrees)
const float BASE_ROT_MIN = -180.0f;
const float BASE_ROT_MAX = 180.0f;

const float ARM2_ROT_MIN = -60.0f;
const float ARM2_ROT_MAX = 30.0f;

const float ARM3_ROT_MIN = -30.0f;
const float ARM3_ROT_MAX = 90.0f;


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
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
	Model robotModel((char*)"Ramie_robota_poprawa.glb");

	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	glEnable(GL_DEPTH_TEST);
	//DO zobaczenia siatki czy model sie zaladowa³
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Rendering window, swaping buffers so window is not flickery
	glfwSwapBuffers(window);

	//Setting for model_robot
	Node* BaseNode = robotModel.findNodeByName(robotModel.rootNode, "Base");
	if (BaseNode) {
		BaseNode->transformation = glm::mat4(1.0f);
		BaseNode->transformation = glm::scale(
			BaseNode->transformation,
			glm::vec3(0.2f)
		);
	}

	//Pozycja x i z 0.74
	float count = 0.0f;
	glm::vec3 grabberlink_position;
	glm::vec3 grabberlink_position_example;
	glm::vec3 arm3_position;



	while (!glfwWindowShouldClose(window))
	{
		//Camera settings (calculating each frame to get smoother camera)
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
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

		proj = glm::perspective(glm::radians(camera.Zoom), (float)(WIDTH / HEIGHT), 0.1f, 100.0f);
		view = camera.GetViewMatrix();

		shaderProgram.Activate();
		int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
		// floor
		floor.Draw(shaderProgram);

		//robot

		//Calculating positionas after 10000 frames
		//if (count > 10000)
		//{
		//	//robotModel.printAllGlobalPositions(robotModel.rootNode, glm::mat4(1.0f));
		//	grabberlink_position_example = robotModel.getGlobalPosition(robotModel.rootNode, glm::mat4(1.0f), "Grabber_link");
		//	std::cout << "Pozycja grabbera x: " << grabberlink_position_example.x << std::endl << "Pozycja grabbera y: " << grabberlink_position_example.y << std::endl << "Pozycja grabbera z: " << grabberlink_position_example.z << std::endl;
		//	count = 0;
		//}
		//else
		//	count++;
		
		grabberlink_position = robotModel.getGlobalPosition(robotModel.rootNode, glm::mat4(1.0f), "Grabber_link");
		grabberlink_location_x = grabberlink_position.x;
		grabberlink_location_y = grabberlink_position.y;
		grabberlink_location_z = grabberlink_position.z;
		arm3_position = robotModel.getGlobalPosition(robotModel.rootNode, glm::mat4(1.0f), "Arm3");
		arm3_location_x = grabberlink_position.x;
		arm3_location_y = grabberlink_position.y;
		arm3_location_z = grabberlink_position.z;
		std::cout << arm3_location_x << " " << arm3_location_y << " " << arm3_location_z << std::endl;

		//Baserotator movement
		Node* Baserotator = robotModel.findNodeByName(robotModel.rootNode, "Base_rotator");
		if (Baserotator) {
			Baserotator->transformation = glm::mat4(1.0f);
			Baserotator->transformation = glm::rotate(
				Baserotator->transformation,
				glm::radians(rotationBaseAngle),
				glm::vec3(0.0f,1.0f,0.0f)
			);
		}
		//arm2 movement
		Node* Arm2 = robotModel.findNodeByName(robotModel.rootNode, "Arm2");
		if (Arm2) {
			Arm2->transformation = glm::mat4(1.0f);
			Arm2->transformation = glm::rotate(
				Arm2->transformation,
				glm::radians(rotationArm2Angle),
				glm::vec3(0.0f, 0.0f, 1.0f)
			);
		}
		//arm3 movement
		Node* Arm3 = robotModel.findNodeByName(robotModel.rootNode, "Arm3");
		if (Arm3) {
			Arm3->transformation = glm::mat4(1.0f);
			Arm3->transformation = glm::rotate(
				Arm3->transformation,
				glm::radians(rotationArm3Angle),
				glm::vec3(0.0f, 0.0f, 1.0f)
			);
		}

		//grabber movement
		Node* grabber1 = robotModel.findNodeByName(robotModel.rootNode, "Grabber1");
		if (grabber1) {
			glm::vec3 localPosition = glm::vec3(grabber1->transformation[3]);
			grabber_location = localPosition.z;
			grabber1->transformation = glm::mat4(1.0f);
			grabber1->transformation = glm::translate(
				grabber1->transformation,
				glm::vec3(0.0f, 0.0f, grabber_movement)
			);
		}
		Node* grabber2 = robotModel.findNodeByName(robotModel.rootNode, "Grabber2");
		if (grabber2) {
			grabber2->transformation = glm::mat4(1.0f);
			grabber2->transformation = glm::translate(
				grabber2->transformation,
				glm::vec3(0.0f, 0.0f, -2.0f * grabber_movement)  // ruch przeciwny
			);
		}

		robotshader.Activate();
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
	//Camera position input
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	//Robot positions
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && rotationBaseAngle < BASE_ROT_MAX)
		rotationBaseAngle += 25.0f*deltaTime;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && rotationBaseAngle > BASE_ROT_MIN)
		rotationBaseAngle -= 25.0f*deltaTime;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && rotationArm2Angle < ARM2_ROT_MAX)
		rotationArm2Angle += 25.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && check_area_xz_axis(grabberlink_location_x,grabberlink_location_z) && grabberlink_location_y >=0.213f && rotationArm2Angle > ARM2_ROT_MIN)
		rotationArm2Angle -= 25.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && rotationArm3Angle < ARM3_ROT_MAX)
		rotationArm3Angle += 25.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && check_area_xz_axis(grabberlink_location_x,grabberlink_location_z) && grabberlink_location_y >= 0.213f && rotationArm3Angle > ARM3_ROT_MIN)
		rotationArm3Angle -= 25.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && grabber_location <=0.5f)
		grabber_movement += 0.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS && grabber_location >=0.0f)
		grabber_movement -= 0.5f * deltaTime;
		
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

bool check_area_xz_axis(float grabberlink_location_x, float grabberlink_location_z)
{
	if (std::abs(grabberlink_location_x) >= 0.8f || std::abs(grabberlink_location_z) >= 0.8f)
		return true;
	return false;
}