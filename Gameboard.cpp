
#include "Gameboard.h"
#include <iostream>

#define DEBUG		// Uncomment to enable printing out of extra debug information


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


/* GAMEBOARD CLASS DEFINITIONS */

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
	if (!isValidCoord(coord)) {
		return nullptr;
	}
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
	if (captured) captured->setPos(-1, -1);	
	return captured;
}

Piece * Gameboard::place(Piece * piece, char file, int rank)
{
	return place(piece, rftoxy(file, rank));
}

Piece * Gameboard::move(Piece * piece, Move to)
{
	// Backup to History Stack
	Hist last;
	last.from = piece->getPos();
	last.to = to.coord;
	last.movedStatus = piece->getStatus();

	int from = xytoi(piece->getPos());
	Piece* captured = place(piece, to.coord);
	m_board[from].set(nullptr);

	last.captured = captured;
	piece->setStatus(Piece::Status::NORMAL);					// Mark moved piece as dirty
	if (captured) {
		last.capturedStatus = captured->getStatus();
		captured->setStatus(Piece::Status::CAPTURED);	// Mark captured piece as captured
	}
	m_history.push(last);
	return captured;
}

Piece * Gameboard::undo()
{
	if (m_history.empty()) return nullptr;	// nothing to undo

	Hist last = m_history.top();
	Piece* moved = nullptr;
	int from = xytoi(last.from);
	int to = xytoi(last.to);	

	// Revert moved piece
	moved = m_board[to].get();
	moved->setPos(last.from.x, last.from.y);		
	moved->setStatus(last.movedStatus);	
	m_board[from].set(moved);

	// Revert captured
	if (last.captured) {
		last.captured->setPos(last.to.x, last.to.y);
		last.captured->setStatus(last.capturedStatus);
	}
	m_board[to].set(last.captured);			

	m_history.pop();		// pop from stack
	return last.captured;	// return captured 
}

std::vector<Gameboard::Playable> Gameboard::genPlayables(std::vector<Piece*>& owned, Piece& king)
{
	Playable op;
	std::vector<Move> moves;		// Moves a single piece can make
	std::vector<Playable> options;	// Options the player has this turn

	for (int i = 0; i < owned.size(); i++) {
#ifdef DEBUG
		std::cout << "Processing ";
		print(owned[i]);
		std::cout << '\n';
#endif
		// Cull pieces that are off the board
		if (owned[i]->isOnBoard()) {
#ifdef DEBUG
			std::cout << "Piece on board. Generating Moves:\n";
#endif
			moves = generateMoves(*(owned[i]), king);	// Generate moves for this piece
#ifdef DEBUG		
			std::cout << "Piece ";
			print(owned[i]);
			std::cout << " Has " << moves.size() << " Generated Moves\n\n";
#endif
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

bool Gameboard::isThreatened(Piece & piece)	// Returns true if passed in piece is under attack
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

std::vector<Gameboard::Move> Gameboard::generateMoves(Piece & piece, Piece& king)
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

#ifdef DEBUG
	std::cout << "Piece is Pawn? : " << isPawn << '\n';
	std::cout << "Moveset is Extendable? : " << moveset.extendable << '\n';
	std::cout << "Orientation : " << piece.orientation() <<'\n';
	std::cout << "Mapping Pieces Base Moveset Vectors :\n";
#endif

	mapped = mapBase(piece);

#ifdef DEBUG
	std::cout << "Processing Mapped Vectors:\n";
#endif
	for (int i = 0; i < mapped.size(); i++) {
#ifdef DEBUG
		std::cout << "Mapped [" << mapped[i].x << "," << mapped[i].y << "]";
#endif
		if (!isValidCoord(mapped[i])) {
#ifdef DEBUG
			std::cout << " : Invalid Coord [Off Board]\n";
#endif
			continue;	// if mapped is off board, continue with next mapped coord
		}
#ifdef DEBUG
		std::cout << '\n';
#endif
		checkpos = mapped[i];
		atPos = check(checkpos);
		extend = moveset.extendable;

		do {
			move.coord = checkpos;
#ifdef DEBUG
			std::cout << "Checking Pos [" << checkpos.x << "," << checkpos.y << "]\n";
#endif
			if (atPos) {
				if (atPos->team() != team) {
#ifdef DEBUG
					std::cout << "Enemy Piece Found at Position : ";
					print(atPos);
					std::cout << '\n';
#endif
				}
				else {
#ifdef DEBUG
					std::cout << "Friendly Piece Found at Position : ";
					print(atPos);
					std::cout << '\n';
#endif
				}
				extend = false;
				if (!isPawn && atPos->team() != team) {
#ifdef DEBUG
					std::cout << "Position Added To Move List\n";
#endif
					moves.push_back(move);
				}
			}
			else {
#ifdef DEBUG
				std::cout << "Position Free!\n";
#endif
				if (!isValidCoord(checkpos)) {
#ifdef DEBUG
					std::cout << "Invalid Coord [Off Board]\n";
#endif
					break;	// Out of bounds -> continue with next mapped
				}
#ifdef DEBUG
				std::cout << "Position Added to move list\n";
#endif
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
		glm::vec2 l_diag(1 * orientation, -1 * orientation);
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

	// print moves list
#ifdef DEBUG
	std::cout << "Unsanitized Move List :\n";
	for (int i = 0; i < moves.size(); i++) {
		std::cout << "\t[" << moves[i].coord.x << "," << moves[i].coord.y << "]\n";
	}
#endif

	moves = sanitize(moves, piece, king);

	return moves;
}

std::vector<glm::vec2> Gameboard::mapBase(Piece & piece)
{
	glm::vec2 map;
	std::vector<glm::vec2> mapped;
	std::vector<glm::vec2> base = piece.moveset().base;
	int orientation = piece.orientation();

	for (int i = 0; i < base.size(); i++) {
		map.x = base[i].x;
		map.y = base[i].y;
		map.x *= orientation;
		map.y *= orientation;
		map.x = map.x + piece.getPos().x;
		map.y = map.y + piece.getPos().y;
		mapped.push_back(map);
#ifdef DEBUG
		std::cout << "Base [" << base[i].x << "," << base[i].y << "] Mapped to [" << map.x << "," << map.y << "]\n";
#endif
	}

	return mapped;
}

/*
	Cull threats Function - Removes threats from move list
*/
std::vector<Gameboard::Move> Gameboard::sanitize(std::vector<Move>& moves, Piece& piece, Piece& king)
{
	std::vector<Move> safe;
#ifdef DEBUG
	std::cout << "Sanitizing Moves\n";
#endif

	for (int i = 0; i < moves.size(); i++) {
		// 1) Make Move
#ifdef DEBUG
		std::cout << "Before Move : "; print(&piece); std::cout << '\n';
#endif
		move(&piece, moves[i]);
#ifdef DEBUG
		std::cout << "\tAfter Move : "; print(&piece); std::cout << '\n';
#endif
		// 2) Check if move is safe
		if (!isThreatened(king)) {
			safe.push_back(moves[i]);
		}

		// 3) Undo Move
		undo();
#ifdef DEBUG
		std::cout << "\tAfter Undo : "; print(&piece); std::cout << '\n';
#endif
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

void Gameboard::print(Piece* piece)
{
	std::cout << piece->ID() << " | " << piece->teamStr() << " | " << piece->typeStr() << " | " << piece->statusStr() << " | [" << piece->getPos().x << "," << piece->getPos().y << "]";
}