
#include "Gameboard.h"
#include <iostream>

/* CELL CLASS DEFINITIONS */

Cell::Cell(Color& c)
	: m_color(c)
{
	// this cell's piece component is initially nullptr
}

Color Cell::color() const
{
	return m_color;
}

Piece* Cell::get() const
{
	return m_piece;
}

void Cell::set(Piece* p)
{
	m_piece = p;
}


/* GAMEBOARD CLASS DEFINTIONS */

Gameboard::Gameboard()
{
	int index = 0;
	Color color = Color::WHITE;

	for (int r = 0; r < ROW; r++) {

		if (color == Color::BLACK) color = Color::WHITE;
		else color = Color::BLACK;

		for (int c = 0; c < COL; c++)
		{
			m_board.push_back(Cell(color));

			// Swap Color
			if (color == Color::BLACK) color = Color::WHITE;
			else color = Color::BLACK;
		}
	}
}

Piece * Gameboard::check(glm::vec2 coord) const
{
	if (!isValidCoord(coord)) return nullptr;
	else return m_board[xytoi(coord)].get();
}

Piece * Gameboard::check(char file, int rank) const
{
	return check(rftoxy(file, rank));
}

Piece * Gameboard::place(Piece * piece, glm::vec2 coord)
{
	if (!isValidCoord(coord)) return nullptr;
	int index = xytoi(coord);
	Piece* captured = m_board[index].get();	// backup captured piece
	m_board[index].set(piece);
	piece->setPos(coord.x, coord.y);	// Update board coordinate [but NOT status]
	captured->setPos(-1, -1);	
	return captured;
}

Piece * Gameboard::place(Piece * piece, char file, int rank)
{
	return place(piece, rftoxy(file, rank));
}

Piece * Gameboard::move(Piece * piece, Move to)
{
	int from = xytoi(piece->getPos());
	Piece* captured = place(piece, to.coord);
	piece->setStatus(Piece::Status::NORMAL);					// Mark moved piece as dirty
	if (captured) captured->setStatus(Piece::Status::CAPTURED);	// Mark captured piece as captured
	return captured;
}

void Gameboard::undo() 
{
	if (m_history.empty()) return;	// nothing to undo


}

std::vector<Gameboard::Option> Gameboard::generateOptions(std::vector<Piece*>& owned, Piece& king)
{
	Option op;				
	std::vector<Move> moves;		// Moves a single piece can make
	std::vector<Option> options;	// Options the player has this turn

	for (int i = 0; i < owned.size(); i++) {
		// Cull pieces that are off the board
		if (owned[i]->isOnBoard()) {
			moves = generateMoves(*owned[i], king.getPos());	// Generate moves for this piece
			if (moves.size() > 0) {
				// If piece has moves to make, wrap in Option and add to list
				op.piece = owned[i];
				op.moves = moves;
				options.push_back(op);
			}
		}
	}

	return options;
}

bool Gameboard::threatAssess(Piece & piece)	// Returns true if threat assessment passes, false if fails
{
	static const std::vector<glm::vec2> los = {	// line of sight directions from piece to check
		glm::vec2(1,0), glm::vec2(1,1), glm::vec2(0,1), glm::vec2(-1,1),
		glm::vec2(-1,0), glm::vec2(-1,-1), glm::vec2(0,-1), glm::vec2(1,-1)
	};
	static const std::vector<glm::vec2> kpos = {	// Knight positions 
		glm::vec2(2,1), glm::vec2(1,2), glm::vec2(-1,2), glm::vec2(-2,1),
		glm::vec2(-2,-1), glm::vec2(-1,-2), glm::vec2(1,-2), glm::vec2(2,-1)
	};

	bool diag = false;
	bool extend = false;
	int range = 0;
	glm::vec2 checkpos;
	Piece* atPos = nullptr;

	// Check line of sight to enemy pieces
	for (int i = 0; i < los.size(); i++) {
		diag = (i % 2 == 0 ? false : true);
		checkpos = piece.getPos() + los[i];
		extend = true;

		do {
			if (!isValidCoord(checkpos)) break;
			atPos = check(checkpos);
			range = sqrt(pow(abs(checkpos.x), 2) + pow(abs(checkpos.y), 2));

			if (atPos) {	// Checkpos contains a piece
				extend = false;
				if (atPos->team() != piece.team()) {	// Enemy piece encountered
					switch (atPos->type()) {
					case Piece::Type::BISHOP :
						if (diag) return true;
						break;
					case Piece::Type::KING :
						if (range == 1) return true;
						break;
					case Piece::Type::PAWN :
						if (diag && range == 1) return true;
						break;
					case Piece::Type::QUEEN :
						return true;
						break;
					case Piece::Type::ROOK :
						if (!diag) return true;
						break;
					}
				}
			}
			checkpos += los[i];

		} while (extend);
	}

	// Check Knight positions
	for (int i = 0; i < kpos.size(); i++) {
		checkpos = piece.getPos() + kpos[i];
		atPos = check(checkpos);
		if (atPos) {
			if (atPos->team() != piece.team() && atPos->type() == Piece::Type::KNIGHT)
				return true;
		}
	}

	return false;
}

bool Gameboard::isValidCoord(glm::vec2 coord) const
{
	int x = coord.x;
	int y = coord.y;

	if (x < 0 || x >= COL || y < 0 || y >= ROW) return false;
	else return true;
}

std::vector<Gameboard::Move> Gameboard::generateMoves(Piece & piece, const glm::vec2 kingpos)
{
	Move move;
	std::vector<Move> moves;
	std::vector<glm::vec2> mapped;	// Mapped base vectors for a single piece 
	Piece::Moveset moveset = piece.moveset();
	int orientation = piece.orientation();
	Color team = piece.team();
	Piece* atPos = nullptr;
	glm::vec2 checkpos;
	bool extend;
	const bool isPawn = (piece.type() == Piece::Type::PAWN ? true : false);

	mapped = mapBase(piece);

	for (int i = 0; i < mapped.size(); i++) {
		if (!isValidCoord(mapped[i])) continue;	// if mapped is off board, continue with next mapped coord
		checkpos = mapped[i];
		atPos = check(checkpos);
		extend = moveset.extendable;

		do {
			move.coord = checkpos;
			if (atPos) {
				extend = false;
				if (!isPawn && atPos->team() != team) moves.push_back(move);
			}
			else {
				if (!isValidCoord(checkpos)) break;	// Out of bounds -> continue with next mapped
				moves.push_back(move);
				if (isPawn && piece.getStatus() == Piece::Status::PRISTINE) {	// Pawns can "rush" on their first turn
					move.coord.x += (moveset.base[i].x * orientation);
					move.coord.y += (moveset.base[i].y * orientation);
					atPos = check(move.coord);
					if (!atPos) moves.push_back(move);
				}
			}

			checkpos.x += (moveset.base[i].x * orientation);
			checkpos.y += (moveset.base[i].y * orientation);
			atPos = check(checkpos);

		} while (extend);	// Extend along direction of extendable
	}

	if (isPawn) {	// Pawns can attack diagonally
		glm::vec2 l_diag(-1 * orientation, 1 * orientation);
		glm::vec2 r_diag(1 * orientation, 1 * orientation);
		move.coord = piece.getPos() + l_diag;
		atPos = check(move.coord);
		if (atPos) {
			if (atPos->team() != team) moves.push_back(move);
		}
		move.coord = piece.getPos() + r_diag;
		atPos = check(move.coord);
		if (atPos) {
			if (atPos->team() != team) moves.push_back(move);
		}
	}

	moves = cullThreats(moves, piece, kingpos);

	return moves;
}

std::vector<glm::vec2> Gameboard::mapBase(Piece & piece)
{
	glm::vec2 map;
	std::vector<glm::vec2> mapped;
	std::vector<glm::vec2> base = piece.moveset().base;
	int orientation = piece.orientation();

	for (int i = 0; i < base.size(); i++) {
		map = base[i] + piece.getPos();
		map.x *= orientation;
		map.y *= orientation;
		mapped.push_back(map);
	}

	return mapped;
}


std::vector<Gameboard::Move> Gameboard::cullThreats(std::vector<Move>& moves, Piece& piece, const glm::vec2 kingpos)
{
	std::vector<Move> safe;
	glm::vec2 from = piece.getPos();
	Piece* captured = nullptr;
	const Piece::Status pStatus = piece.getStatus();
	Piece::Status cStatus = Piece::Status::NORMAL;

	for (int i = 0; i < moves.size(); i++) {
		captured = check(moves[i].coord);
		if(captured) cStatus = captured->getStatus();
		captured = move(&piece, moves[i]);
		if (!threatAssess(piece)) {
			safe.push_back(moves[i]);
		}
		place(&piece, from);
		piece.setStatus(pStatus);
		place(captured, moves[i].coord);
		if (captured) captured->setStatus(cStatus);
	}

	return safe;
}

int Gameboard::xytoi(glm::vec2 coord) const
{
	return (coord.y * COL) + coord.x;
}

int Gameboard::rftoi(char file, int rank) const
{
	return xytoi(rftoxy(file, rank));
}

// Convert rank and file to board coordinates
glm::vec2 Gameboard::rftoxy(char file, int rank) const
{
	return glm::vec2(ftox(file), rtoy(rank));
}

int Gameboard::ftox(char file) const
{
	return file - 97;	// lowercase a [ASCII val 97] corresponds to 0
}

int Gameboard::rtoy(int rank) const
{
	return rank - 1;		// Rank is 1-indexed, coordinates are 0-indexed
}