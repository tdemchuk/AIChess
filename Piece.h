#ifndef PIECE_H
#define PIECE_H

#include "Color.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>

/*
	Abstract chess piece superclass
*/
class Piece
{
public:

	struct Moveset {
		std::vector<glm::vec2> base;	// The base moves that the piece can make
		bool extendable;				// Whether or not the base move can be extended to a length longer than 1
	};

	enum Type {
		KING,		// 0 
		QUEEN,		// 1
		BISHOP,		// 2
		KNIGHT,		// 3
		ROOK,		// 4
		PAWN		// 5
	};

	enum Status {
		CAPTURED,	// Piece has been captured by the opposing Player
		NORMAL,		// Piece is on the board and has been moved
		PRISTINE	// Piece hasn't been touched --> still in initial position
	};

	static unsigned int glob_ID;

	Piece() = delete;
	Piece(Type type, Color team);
	~Piece() = default;

	unsigned int ID() const;			// Returns this pieces ID
	Type type() const;					// Returns the type of this chess piece
	std::string typeStr() const;		// Returns a string describing the type of this piece
	Color team() const;					// Returns the team this piece is on
	std::string teamStr() const;		// Returns a string decsribing which team this piece is on [its color]
	const Moveset& moveset() const;		// Returns the Moveset for this piece
	int orientation() const;			// Returns the orientation of thios piece on the board
	Status getStatus() const;			// Returns this piece's status
	std::string statusStr() const;		// Returns a string describing this piece's status 
	void setStatus(Status status);		// Sets this piece's status
	bool isOnBoard() const;				// Returns whether or not this piece is on the gameboard (has a valid rank and file)
	char file() const;					// Returns the file of this piece on the board [NUL if not on board], [a-h used]
	int rank() const;					// Returns the rank of this piece on the board [-1 if not on board], [1-8 used]
	glm::vec2 getPos() const;			// Returns the coordinates of this piece on the gameboard
	void setPos(int x, int y);			// Sets the coordinates of this piece on the board	
	bool canPromote() const;			// If this piece is eligible for promotion
	bool promote(Type type);			// Promote this piece to the provided type [if valid type (no King, no Pawn)] --> modifies the Type and Moveset for this Piece

private:
	void setMoveset();					// Sets the moveset of this piece based on its type

	const unsigned int	m_ID;			// Unique piece ID
	mutable Type		m_type;			// What type of chess piece
	const Color			m_team;			// Team that piece is on
	Moveset				m_moveset;		// This piece's moveset
	const int			m_orientation;	// This piece's orientation
	Status				m_status;		// This piece's status
	glm::vec2			m_pos;			// The pieces position on the board [rank(Y) and file(x)]. (-1,-1) if not on board
};

#endif