#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "Color.h"
#include "Piece.h"
#include <stack>

/*
	Represents a single cell on a chess gameboard
*/
class Cell
{
public:
	Cell() = delete;
	Cell(Color& c);
	~Cell() = default;

	Color color() const;		// Returns the color of this cell
	Piece* get() const;			// Returns the piece on this Cell
	void set(Piece* p);			// Sets the piece on this Cell

private:
	Piece* m_piece = nullptr;		// The piece that occupies this cell on the gameboard
	const Color m_color;				// This cell's color
};

/*
	Represents a chess gameboard
*/
class Gameboard
{
public:
	struct Move {
		glm::vec2 coord;
		bool special = false;		
	};

	struct Playable {
		Piece* piece = nullptr;		// The piece that can make the following moves
		std::vector<Move> moves;	// All board coordinates that the piece can move to
	};

	struct Hist {			// Move History Object
		bool special;
		glm::vec2 from;				
		glm::vec2 to;
		Piece* captured;
		Piece::Status capturedStatus;
		Piece::Status movedStatus;
	};


	static const int ROW = 8;
	static const int COL = 8;

	const int rows() const { return ROW; }	// Get the number of rows on this board
	const int cols() const { return COL; }	// Get the number of columns on this board

	Gameboard();
	~Gameboard() = default;

	Piece* check(glm::vec2 coord) const;			// Gets the piece at the given coordinate
	Piece* check(char file, int rank) const;		// Gets the piece at the given rank and file on the board
	Piece* place(Piece* piece, glm::vec2 coord);	// Places the given piece at the specified coordinate
	Piece* place(Piece* piece, char file, int rank);// Places the given piece at the specified rank and file
	Piece* move(Piece* piece, Move to);				// Moves the given piece to the given location and returns the captured piece, if any
	Piece* undo();									// Undoes the changes made by the previous call to "move". Returns the pointer to the previously captured piece
	std::vector<Playable> genPlayables(std::vector<Piece*>& owned, Piece& king);	// Generates playable pieces and corresponding moves that the given player can make on the current boardstate
	bool isThreatened(Piece& piece);				// Returns true if the given piece is under attack in the given boardstate
	bool isValidCoord(glm::vec2 coord) const;		// Returns true if the coordinate is on the board

private:
	std::vector<Move> generateMoves(Piece& piece, Piece& king);	// Returns a list of moves the given piece can legally make in the given boardstate
	std::vector<glm::vec2> mapBase(Piece& piece);	// Returns a list of base moves mapped to the board, with all illegal moves filtered out
													// Returns all moves in the given list that doesnt result in the players king in check
	std::vector<Move> sanitize(std::vector<Move>& moves, Piece& piece, Piece& king);
	int xytoi(glm::vec2 coord) const;				// Converts a 2d coordinate to 1D array index
	int rftoi(char file, int rank) const;			// Converts a rank and file to a 1D array index
	glm::vec2 rftoxy(char file, int rank) const;	// Converts a rank and file to a 2D board coordinate
	int ftox(char file) const;						// Converts file to the x-component of a board coordinate
	int rtoy(int rank) const;						// Converts rank to the y-component of a board coordinate 
	
	void print(Piece* piece);
	

	std::vector<Cell> m_board;	// board has 64 cells
	std::stack<Hist> m_history;	// move history stack
};

#endif