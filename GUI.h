/*
#ifndef GUI_H
#define GUI_H

#include "UI.h"
#include "GL.h"

/*
	Some code from www.learnopengl.com
*/
/*
class GUI : public UI
{
public:

	GUI();		// Constructor
	~GUI();		// Destructor

	// Draws the current gameboard to the screen
	void drawBoard(const Gameboard& board) const override;

	// Prompts the current player to make a move
	glm::vec2 promptMove(std::vector<Gameboard::Playable>& moves) override;

	// Prompts the current player to choose what to promote a piece to 
	Piece::Type promptPromote() override;

	// Prompts the user to select a mode of play (2-Player vs 1P & AI --> Difficulty selection)
	Mode promptGameMode() override;

	// Draws the provided message string to the screen (use for win/loss/draw messages)
	void drawMessage(std::string msg) override;

private:

	const char* m_Title;	// Window title
	const int m_Width;		// initial window width
	const int m_Height;		// initial window height
	GLFWwindow* window;		// window object 
	//Shader* shader;			// shader object
	//float* vertices;			// array containing gameboard vertices
};

#endif
*/