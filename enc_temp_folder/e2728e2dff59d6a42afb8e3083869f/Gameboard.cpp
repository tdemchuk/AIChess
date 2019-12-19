
#include "Gameboard.h"
#include <iostream>

//#define DEBUG		// Uncomment to enable printing out of extra debug information


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
	// Local Vars
	Hist last;
	int from;
	Piece* captured = nullptr;

	// Handle Castling Move
	if (to.special && piece->type() == Piece::Type::KING) {		// Castling Move
		Piece* rook = nullptr;
		glm::vec2 kingpos = piece->getPos();
		glm::vec2 rook_to(kingpos.x,-1);
		if (kingpos.y < to.coord.y) {		// Kingside castling
			rook = check(glm::vec2(kingpos.x,7));
			rook_to.y = to.coord.y - 1;
		}	
		else if (kingpos.y > to.coord.y) {	// Queenside Castling
			rook = check(glm::vec2(kingpos.x,0));
			rook_to.y = to.coord.y + 1;
		}
		last.special = false;
		last.from = rook->getPos();
		last.to = rook_to;
		last.movedStatus = rook->getStatus();
		from = xytoi(rook->getPos());
		captured = place(rook, rook_to);
		m_board[from].set(nullptr);
		last.captured = captured;
		rook->setStatus(Piece::Status::NORMAL);
		if (captured) {
			last.capturedStatus = captured->getStatus();
			captured->setStatus(Piece::Status::CAPTURED);	// Mark captured piece as captured
		}
		m_history.push(last);
	}

	bool enPassant = to.special && piece->type() == Piece::Type::PAWN ? true : false;

	// Backup to History Stack
	last.special = to.special;
	last.from = piece->getPos();
	last.to = to.coord;
	last.movedStatus = piece->getStatus();

#ifdef DEBUG
	if (last.special) std::cout << "Special Move!\n";
	if (enPassant) {
		std::cout << "En PAssant Move\n\tPiece : ";
		print(piece);
		std::cout << "\n";
	}
#endif

	from = xytoi(piece->getPos());
	captured = place(piece, to.coord);	
	if (enPassant) {	// Handle En Passant Move
		glm::vec2 capLoc = to.coord - glm::vec2(piece->orientation(),0);
		captured = check(capLoc);
		m_board[xytoi(capLoc)].set(nullptr);
		std::cout << "\tCaptured : ";
		print(captured);
	}
	m_board[from].set(nullptr);

	last.captured = captured;
	piece->setStatus(Piece::Status::NORMAL);					// Mark moved piece as dirty
	if (captured) {
		last.capturedStatus = captured->getStatus();
		captured->setStatus(Piece::Status::CAPTURED);	// Mark captured piece as captured
		captured->setPos(-1,-1);
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

	bool enPassant = last.special && moved->type() == Piece::Type::PAWN ? true : false;

	// Revert captured
	if (last.captured) {
		if (enPassant) {
			last.captured->setPos(last.to.x - moved->orientation(), last.to.y);
		}
		else last.captured->setPos(last.to.x, last.to.y);
		last.captured->setStatus(last.capturedStatus);
	}
	if (enPassant) {
		glm::vec2 capLoc = last.to - glm::vec2(moved->orientation(), 0);
		m_board[xytoi(capLoc)].set(last.captured);
		m_board[to].set(nullptr);
	}
	else m_board[to].set(last.captured);			

	m_history.pop();		// pop from stack
	if (last.special && moved->type() == Piece::Type::KING) undo();	// Castling move -> composite move requiring an additional undo
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
			if (!isValidCoord(checkpos)) {
				break;
			}
			atPos = check(checkpos);
			range = sqrt(pow(abs(checkpos.x - piece.getPos().x), 2) + pow(abs(checkpos.y - piece.getPos().y), 2));

			if (atPos) {	// Checkpos contains a piece
				extend = false;
				if (atPos->team() != piece.team()) {	// Enemy piece encountered
					switch (atPos->type()) {
					case Piece::Type::BISHOP:
						if (diag) return true;
						break;
					case Piece::Type::KING:
						if (range == 1) return true;
						break;
					case Piece::Type::PAWN:
						if (diag && range == 1) {
							if (atPos->rank() < piece.rank() && atPos->orientation() == 1)			return true;
							else if (atPos->rank() > piece.rank() && atPos->orientation() == -1)	return true;
						}
						break;
					case Piece::Type::QUEEN:
						return true;
						break;
					case Piece::Type::ROOK:
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
	const bool isKing = (piece.type() == Piece::Type::KING ? true : false);
	Hist last;


#ifdef DEBUG
	std::cout << "Piece is Pawn? : " << isPawn << '\n';
	std::cout << "Moveset is Extendable? : " << moveset.extendable << '\n';
	std::cout << "Orientation : " << piece.orientation() <<'\n';
	std::cout << "Mapping Pieces Base Moveset Vectors :\n";
#endif

	move.special = false;
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

	// Handle Pawn Attack & En Passant 
	if (isPawn) {	
		bool firstTurn = m_history.empty();
		if (!firstTurn) last = m_history.top();
		glm::vec2 l_diag(1 * orientation, -1 * orientation);
		glm::vec2 r_diag(1 * orientation, 1 * orientation);
		move.special = false;
		move.coord = piece.getPos() + l_diag;
		atPos = check(move.coord);
		if (atPos) {
			if (atPos->team() != team) moves.push_back(move);
		}
		else if (piece.relativeRank() == 5 && !firstTurn) {	// check en passant
			atPos = check(last.to);
			if (atPos) {
				if (atPos->type() == Piece::Type::PAWN && abs(last.to.x - last.from.x) > 1 && last.to.y == move.coord.y) {	// if opponent moved a pawn 2 spaces last turn in an adjacent file
					move.special = true;
					moves.push_back(move);
				}
			}
		}
		move.special = false;
		move.coord = piece.getPos() + r_diag;
		atPos = check(move.coord);
		if (atPos) {
			if (atPos->team() != team) moves.push_back(move);
		}
		else if (piece.relativeRank() == 5 && !firstTurn) {	// check en passant
			atPos = check(last.to);
			if (atPos) {
				if (atPos->type() == Piece::Type::PAWN && abs(last.to.x - last.from.x) > 1 && last.to.y == move.coord.y) {	// if opponent moved a pawn 2 spaces last turn in an adjacent file
					move.special = true;
					moves.push_back(move);
				}
			}
		}
		move.special = false;
	}

	// Check for Castling
	if (isKing && king.getStatus() == Piece::Status::PRISTINE) {	// King hasn 't been moved and is not in check
		
		int x = king.getPos().x;
		std::vector<Move> temp;

		// Check Kingside
		atPos = check(glm::vec2(x, 7));
		if (atPos) {
			if (atPos->type() == Piece::Type::ROOK && atPos->getStatus() == Piece::Status::PRISTINE) {	// Rook is in place and untouched
				if (!check(glm::vec2(x, 5)) && !check(glm::vec2(x, 6))) {		// Intermediary Cells are empty 
					// Ensure Passing Through [x,5] doesnt place king in check
					move.coord.x = x;
					move.coord.y = 5;
					move.special = false;
					temp.push_back(move);
					temp = sanitize(temp, piece, king);
					if (temp.size() > 0) {
						move.coord.x = x;
						move.coord.y = 6;
						move.special = true;
						moves.push_back(move);
					}
				}
			}
		}

		// Check Queenside
		atPos = check(glm::vec2(x, 0));
		if (atPos) {
			if (atPos->type() == Piece::Type::ROOK && atPos->getStatus() == Piece::Status::PRISTINE) {	// Rook is in place and untouched
				if (!check(glm::vec2(x, 1)) && !check(glm::vec2(x, 2)) && !check(glm::vec2(x,3))) {		// Intermediary Cells are empty 
					// Ensure Passing Through [x,3] doesnt place king in check
					move.coord.x = x;
					move.coord.y = 3;
					move.special = false;
					temp.push_back(move);
					temp = sanitize(temp, piece, king);
					if (temp.size() > 0) {
						move.coord.x = x;
						move.coord.y = 2;
						move.special = true;
						moves.push_back(move);
					}
				}
			}
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
	std::cout << piece->ID() << " | " << piece->teamStr() << " | " << piece->typeStr() << " | " << piece->statusStr() << " | [" << piece->rank() << "," << (char)piece->file() << "]";
}