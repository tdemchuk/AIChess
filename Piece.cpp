
#include "Piece.h"


unsigned int Piece::glob_ID = 0;

// Constructor
Piece::Piece(Type type, Color team)
	: m_ID(glob_ID++), m_type(type), m_team(team), m_orientation(m_team == Color::BLACK ? -1 : 1), m_status(Piece::Status::PRISTINE)
{
	setMoveset();

	m_pos.x = -1;
	m_pos.y = -1;
}

unsigned int Piece::ID() const
{
	return this->m_ID;
}

Piece::Type Piece::type() const
{
	return m_type;
}

std::string Piece::typeStr() const
{
	switch (m_type) {
		case KING:		return "King";
		case QUEEN:		return "Queen";
		case BISHOP:	return "Bishop";
		case KNIGHT:	return "Knight";
		case ROOK:		return "Rook";
		case PAWN:		return "Pawn";
	}
	return "NO TYPE";
}

Color Piece::team() const
{
	return m_team;
}

std::string Piece::teamStr() const
{
	switch (m_team) {
		case BLACK: return "Black";
		case WHITE: return "White";
	}
	return "NO TEAM";
}

const Piece::Moveset& Piece::moveset() const
{
	return m_moveset;
}

int Piece::orientation() const
{
	return m_orientation;
}

Piece::Status Piece::getStatus() const
{
	return m_status;
}

void Piece::setStatus(Piece::Status status)
{
	m_status = status;
}

std::string Piece::statusStr() const
{
	switch (m_status) {
		case CAPTURED:	return "Captured";
		case NORMAL:	return "Normal";
		case PRISTINE:	return "Pristine";
		case PRISTINE_CHECK: return "Pristine | In Check";
		case CHECK:		return "In Check";
	}
	return "NO STATUS";
}

bool Piece::isOnBoard() const
{
	if (m_pos.x < 8 && m_pos.x >= 0 && m_pos.y < 8 && m_pos.y >= 0)
		return true;
	else return false;
}

char Piece::file() const
{
	return m_pos.y + 97;	// Start at lowercase 'a'
}

int Piece::rank() const
{
	return m_pos.x + 1;		// position is 0-indexed, add 1 to get proper rank
}

int Piece::relativeRank() const
{
	if (m_orientation == 1) return rank();
	else return (9 - rank());
}

glm::vec2 Piece::getPos() const
{
	return m_pos;
}

void Piece::setPos(int x, int y)
{
	m_pos.x = x;
	m_pos.y = y;
}

bool Piece::canPromote() const
{
	// Only pawns on their 8th (relative) rank can be promoted
	if (m_type == PAWN) {
		if (m_orientation == -1) {
			if (rank() == 1) return true;
			else return false;
		}
		else if (m_orientation == 1) {
			if (rank() == 8) return true;
			else return false;
		}
	}
	return false;
}

bool Piece::promote(Piece::Type type)
{
	// TODO
	// Assumes this piece is valid for promotion
	bool goodtype = false;

	switch (type) {		// Check to make sure promoting to a valid type
	case QUEEN:
		goodtype = true;
		break;
	case BISHOP:
		goodtype = true;
		break;
	case KNIGHT:
		goodtype = true;
		break;
	case ROOK:
		goodtype = true;
		break;
	}
	if (!goodtype) return false;

	m_type = type;	// update piece type
	setMoveset();	// update moveset based on new piece type

	return true;
}

void Piece::setMoveset()
{
	bool extend = false;

	switch (m_type) {
	case KING:
		m_moveset.base = std::vector<glm::vec2>{
			glm::vec2(0,1), glm::vec2(1,1), glm::vec2(1,0), glm::vec2(1,-1),
			glm::vec2(0,-1), glm::vec2(-1,-1), glm::vec2(-1,0), glm::vec2(-1,1)
		};
		extend = false;
		break;
	case QUEEN:
		m_moveset.base = std::vector<glm::vec2>{
			glm::vec2(0,1), glm::vec2(1,1), glm::vec2(1,0), glm::vec2(1,-1),
			glm::vec2(0,-1), glm::vec2(-1,-1), glm::vec2(-1,0), glm::vec2(-1,1)
		};
		extend = true;
		break;
	case BISHOP:
		m_moveset.base = std::vector<glm::vec2>{
			glm::vec2(1,1), glm::vec2(1,-1),
			glm::vec2(-1,-1), glm::vec2(-1,1)
		};
		extend = true;
		break;
	case KNIGHT:
		m_moveset.base = std::vector<glm::vec2>{
			glm::vec2(2,1), glm::vec2(1,2), glm::vec2(1,-2), glm::vec2(2,-1),
			glm::vec2(-2,-1), glm::vec2(-1,-2), glm::vec2(-2,1), glm::vec2(-1,2)
		};
		extend = false;
		break;
	case ROOK:
		m_moveset.base = std::vector<glm::vec2>{
			glm::vec2(0,1), glm::vec2(1,0),
			glm::vec2(0,-1), glm::vec2(-1,0)
		};
		extend = true;
		break;
	case PAWN:
		m_moveset.base = std::vector<glm::vec2>{
			glm::vec2(1,0)
		};
		extend = false;
		break;
	}

	m_moveset.extendable = extend;
}