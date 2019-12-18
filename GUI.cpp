/*
#include "GUI.h"
#include "stb_image.h"
#include "Shader.h"


//	GUI Class Implementation
//	By Tennyson Demchuk
//	Various pieces of code borrowed from www.learnopengl.com


void framebuffer_size_callback(GLFWwindow* window, int width, int height);	// Handle window resize
void mouse_callback(GLFWwindow* window, double xpos, double ypos);			// Handle mouse click
void processInput(GLFWwindow* window);										// Handle keyboard input

// Constructor
GUI::GUI()
	: m_Title("COSC 3P71 - Term Project - Chess"), m_Width(600), m_Height(600)
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// Using OpenGL version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create Window
	window = glfwCreateWindow(m_Width, m_Height, m_Title, NULL, NULL);
	if (window == NULL) {
		std::cout << "Window Creation Failed\n";
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	// Load OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "GLAD Failed to Initialize\n";
		return;
	}

	// Create Shader Object
	shader = new Shader("shader.vert", "shader.frag");

	// Init Vertices of gameboard
	vertices = new float[] {
		-0.8f, 0.8f, 0.0f,
		-0.6f, 0.8f, 0.0f,
		-0.4f, 0.8f, 0.0f,
		-0.2f, 0.8f, 0.0f,
		0.0f, 0.8f, 0.0f,
		0.2f, 0.8f, 0.0f,
		0.4f, 0.8f, 0.0f,
		0.6f, 0.8f, 0.0f,
		0.8f, 0.8f, 0.0f,
		-0.8f, 0.6f, 0.0f,
		-0.6f, 0.6f, 0.0f,
		-0.4f, 0.6f, 0.0f,
		-0.2f, 0.6f, 0.0f,
		0.0f, 0.6f, 0.0f,
		0.2f, 0.6f, 0.0f,
		0.4f, 0.6f, 0.0f,
		0.6f, 0.6f, 0.0f,
		0.8f, 0.6f, 0.0f,
		-0.8f, 0.4f, 0.0f,
		-0.6f, 0.4f, 0.0f,
		-0.4f, 0.4f, 0.0f,
		-0.2f, 0.4f, 0.0f,
		0.0f, 0.4f, 0.0f,
		0.2f, 0.4f, 0.0f,
		0.4f, 0.4f, 0.0f,
		0.6f, 0.4f, 0.0f,
		0.8f, 0.4f, 0.0f,
		-0.8f, 0.2f, 0.0f,
		-0.6f, 0.2f, 0.0f,
		-0.4f, 0.2f, 0.0f,
		-0.2f, 0.2f, 0.0f,
		0.0f, 0.2f, 0.0f,
		0.2f, 0.2f, 0.0f,
		0.4f, 0.2f, 0.0f,
		0.6f, 0.2f, 0.0f,
		0.8f, 0.2f, 0.0f,
		-0.8f, 0.0f, 0.0f,
		-0.6f, 0.0f, 0.0f,
		-0.4f, 0.0f, 0.0f,
		-0.2f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.2f, 0.0f, 0.0f,
		0.4f, 0.0f, 0.0f,
		0.6f, 0.0f, 0.0f,
		0.8f, 0.0f, 0.0f,
		-0.8f, -0.2f, 0.0f,
		-0.6f, -0.2f, 0.0f,
		-0.4f, -0.2f, 0.0f,
		-0.2f, -0.2f, 0.0f,
		0.0f, -0.2f, 0.0f,
		0.2f, -0.2f, 0.0f,
		0.4f, -0.2f, 0.0f,
		0.6f, -0.2f, 0.0f,
		0.8f, -0.2f, 0.0f,
		-0.8f, -0.4f, 0.0f,
		-0.6f, -0.4f, 0.0f,
		-0.4f, -0.4f, 0.0f,
		-0.2f, -0.4f, 0.0f,
		0.0f, -0.4f, 0.0f,
		0.2f, -0.4f, 0.0f,
		0.4f, -0.4f, 0.0f,
		0.6f, -0.4f, 0.0f,
		0.8f, -0.4f, 0.0f,
		-0.8f, -0.6f, 0.0f,
		-0.6f, -0.6f, 0.0f,
		-0.4f, -0.6f, 0.0f,
		-0.2f, -0.6f, 0.0f,
		0.0f, -0.6f, 0.0f,
		0.2f, -0.6f, 0.0f,
		0.4f, -0.6f, 0.0f,
		0.6f, -0.6f, 0.0f,
		0.8f, -0.6f, 0.0f,
		-0.8f, -0.8f, 0.0f,
		-0.6f, -0.8f, 0.0f,
		-0.4f, -0.8f, 0.0f,
		-0.2f, -0.8f, 0.0f,
		0.0f, -0.8f, 0.0f,
		0.2f, -0.8f, 0.0f,
		0.4f, -0.8f, 0.0f,
		0.6f, -0.8f, 0.0f,
		0.8f, -0.8f, 0.0f
	};
}

// Destructor
GUI::~GUI()
{
	//delete shader;
	//delete vertices;
	glfwTerminate();
}

// Draws the current gameboard to the screen
void GUI::drawBoard(const Gameboard& board) const
{

}

// Prompts the current player to make a move
glm::vec2 GUI::promptMove(std::vector<Gameboard::Playable>& moves)
{

}

// Prompts the current player to choose what to promote a piece to 
Piece::Type GUI::promptPromote()
{

}

// Prompts the user to select a mode of play (2-Player vs 1P & AI --> Difficulty selection)
Mode GUI::promptGameMode()
{

}

// Draws the provided message string to the screen (use for win/loss/draw messages)
void GUI::drawMessage(std::string msg)
{

}

// -------------------------------------------------------------

void processInput(GLFWwindow* window)
{
	// Termination
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);	// may need to call glfwTerminate | figure out a way to terminate application
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

}
*/