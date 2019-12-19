
#include "Game.h"
#include "Player.h"
#include "AI.h" //Daniel Edit
#include <fstream>	// for input file

/* GAME CLASS DEFINITIONS */
/*
	Main Game Loop
*/

void Game::play(UI* ui, Mode mode) {

	// Local Vars
	Gameboard	board;
	AI*			ai = nullptr;
	int			curPlayer	= 0;				// current player in game [default = 0]
	bool		done		= false;			// If Game has finished [win/loss/draw occurred]
	std::vector<Gameboard::Playable> playables;	// List of moves current player can make
	bool		inCheck		= false;			// If the current players King is in check at the start of their turn
	glm::vec2	input;							// User Input 
	Piece*		captured	= nullptr;			// captured piece placeholder
	bool		aiEnabled	= mode.numPlayers == 1 ? true : false;		// if the AI is standing in for player 2 or not
	std::string msg;

	// 1) Init Players, Pieces
	Player players[] = {Player(Color::WHITE), Player(Color::BLACK)};
	Piece* kings[2];

	// Attempt to read board override file [if it exists]
	std::ifstream file("board.bof");
	std::string ln;
	char bofBoard[8][16];	// 8 lines - 8 dual char clusters [16 chars]
	int numlines = 0;	// 8 line requirement
	int numkings_w = 0;	// 1 King tolerance for white team
	int numkings_b = 0;	// 1 king tolerance for black team
	char tc, pc;		// Team Code & Piece Code
	bool isOpen = file.is_open();
	bool goodFormat = true;		// If BOF is of proper format
	if (isOpen) ui->drawMessage("Board Override File [BOF] Detected. Loading...\n");
	while (std::getline(file, ln)) {
		// Process line of BOF
		if (ln.size() < 23) {
			goodFormat = false;
			break;
		}
		for (int i = 0; i < 8; i++) {	// Process up to 8 char clusters
			tc = ln[(i*3)];			// retrieve team code
			pc = ln[(i*3) + 1];		// retrieve piece code
			if (tc == '#' || pc == '#') {
				if (tc != '#' || pc != '#') {
					goodFormat = false;
					break;
				}
			}
			else {
				// Validate Team Codes
				if (tc != 'B' && tc != 'W') {
					ui->drawMessage("Invalid Team Code Found in BOF at line" + std::to_string(numlines+1) + "\n");
					goodFormat = false;
					break;
				}
				// Validate Piece Codes
				if (pc != 'R' && pc != 'N' && pc != 'B' && pc != 'Q' && pc != 'K' && pc != 'P') {
					ui->drawMessage("Invalid Piece Code Found in BOF at line" + std::to_string(numlines + 1) + "\n");
					goodFormat = false;
					break;
				}
				// Check Kings
				if (pc == 'K') {
					if (tc == 'W')			numkings_w++;
					else if (tc == 'B')		numkings_b++;
				}
			}
			// Store tc and pc
			bofBoard[numlines][(i * 2)] = tc;
			bofBoard[numlines][(i * 2) + 1] = pc;
		}
		numlines++;
	}
	if (numlines < 7) goodFormat = false;
	if (numkings_b != 1) {
		ui->drawMessage("BLACK Does Not Meet The King Requirement\n");
		goodFormat = false;
	}
	if (numkings_w != 1) {
		ui->drawMessage("WHITE Does Not Meet The King Requirement\n");
		goodFormat = false;
	}
	if (goodFormat) {
		int pl;
		Piece::Type pt;
		int x, y;	// board placement coords [r|f]
		// process bof config and generate initial boardstate
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 16; j++) {
				tc = bofBoard[i][j];
				pc = bofBoard[i][++j];
				if (tc == '#') continue;	// skip empty cells
				x = abs(i-7);
				y = j / 2;
				pl = tc == 'W' ? 0 : 1;
				switch (pc) {
				case 'R': {
					pt = Piece::Type::ROOK; 
					break;
				}
				case 'N': {
					pt = Piece::Type::KNIGHT;
					break;
				}
				case 'B': {
					pt = Piece::Type::BISHOP;
					break;
				}
				case 'Q': {
					pt = Piece::Type::QUEEN;
					break;
				}
				case 'K': {
					pt = Piece::Type::KING;
					break;
				}
				case 'P': {
					pt = Piece::Type::PAWN;
					break;
				}
				}
				if (pc == 'K') kings[pl] = players[pl].create(pt, glm::vec2(x, y), board);
				else players[pl].create(pt, glm::vec2(x,y), board);
			}
		}
	}
	else {	// Draw Default Board
		if (isOpen) ui->drawMessage("Invalid BOF Format. Loading Default Board Configuration\n");

		players[0].create(Piece::Type::ROOK, glm::vec2(0, 0), board);
		players[0].create(Piece::Type::KNIGHT, glm::vec2(0, 1), board);
		players[0].create(Piece::Type::BISHOP, glm::vec2(0, 2), board);
		players[0].create(Piece::Type::QUEEN, glm::vec2(0, 3), board);
		kings[0] = players[0].create(Piece::Type::KING, glm::vec2(0, 4), board);		// store created king
		players[0].create(Piece::Type::BISHOP, glm::vec2(0, 5), board);
		players[0].create(Piece::Type::KNIGHT, glm::vec2(0, 6), board);
		players[0].create(Piece::Type::ROOK, glm::vec2(0, 7), board);
		for (int i = 0; i < board.COL; i++) {
			players[0].create(Piece::Type::PAWN, glm::vec2(1, i), board);
		}

		players[1].create(Piece::Type::ROOK, glm::vec2(7, 0), board);
		players[1].create(Piece::Type::KNIGHT, glm::vec2(7, 1), board);
		players[1].create(Piece::Type::BISHOP, glm::vec2(7, 2), board);
		players[1].create(Piece::Type::QUEEN, glm::vec2(7, 3), board);
		kings[1] = players[1].create(Piece::Type::KING, glm::vec2(7, 4), board);		// store created king
		players[1].create(Piece::Type::BISHOP, glm::vec2(7, 5), board);
		players[1].create(Piece::Type::KNIGHT, glm::vec2(7, 6), board);
		players[1].create(Piece::Type::ROOK, glm::vec2(7, 7), board);
		for (int i = 0; i < board.COL; i++) {
			players[1].create(Piece::Type::PAWN, glm::vec2(6, i), board);
		}
	}

	ui->drawBoard(board);
	
	// Initialize AI
	if (aiEnabled) {

		int ply;

		//Apply ply based off difficulty selected
		switch (mode.diff) {

			case 0: ply = 4;
				break;
			case 1: ply = 5;
				break;
			case 2: ply = 6;

		}

		ai = new AI(ply);

	}

	// Print all pieces owned by current player - DEBUG ONLY
#ifdef DEBUG
	std::vector<Piece*> owned = players[curPlayer].getOwned();
	for (int i = 0; i < owned.size(); i++) {
		std::cout << owned[i]->ID() << " | " << owned[i]->teamStr() << " | " << owned[i]->typeStr() << " | " << owned[i]->statusStr() << " | [" << owned[i]->getPos().x << "," << owned[i]->getPos().y << "]\n";
	}
#endif

	// Begin Game Loop
	while (!done) {

		if (players[0].getOwned().size() == 0 || players[1].getOwned().size() == 0) {
			ui->drawMessage("No Pieces On The Board? That Doesn't Seem Right.\nHow About You Find Some Pieces and Try Again?\nGAME OVER.\n");
			done = true;
			continue;
		}

		inCheck = false;

		// Check if it's the AI's turn
		if (aiEnabled && curPlayer == 1) {

			msg = "\nAI's Turn...\n";
			ui->drawMessage(msg);

			// 2) Test to see if King is in Check
			if (board.isThreatened(*kings[curPlayer])) {
				inCheck = true;
				if (kings[curPlayer]->getStatus() == Piece::Status::NORMAL) kings[curPlayer]->setStatus(Piece::Status::CHECK);
				else kings[curPlayer]->setStatus(Piece::Status::PRISTINE_CHECK);
			}
			else if (kings[curPlayer]->getStatus() == Piece::Status::PRISTINE_CHECK) kings[curPlayer]->setStatus(Piece::Status::PRISTINE);
			else if (kings[curPlayer]->getStatus() == Piece::Status::CHECK) kings[curPlayer]->setStatus(Piece::Status::NORMAL);

			// 3) Generate playables for AI
			playables = board.genPlayables(players[curPlayer].getOwned(), *(kings[curPlayer]));

			// 4) Check end conditions
			if (playables.size() == 0) {
				if (inCheck) {	// Other player won
					ui->drawMessage("You Won!");	// AI Lost --> Human Player Won
				}
				else {			// draw
					ui->drawMessage("Draw!");
				}
				done = true;
				continue;
			}
			else {	// Check for only kings left
				bool onlyKings = true;
				for (int i = 0; i < players[0].getOwned().size(); i++) {
					if (players[0].getOwned()[i]->type() != Piece::Type::KING && players[0].getOwned()[i]->isOnBoard() == true) {
						onlyKings = false;
						break;
					}
				}
				for (int i = 0; i < players[1].getOwned().size(); i++) {
					if (players[1].getOwned()[i]->type() != Piece::Type::KING && players[1].getOwned()[i]->isOnBoard() == true) {
						onlyKings = false;
						break;
					}
				}
				if (onlyKings) {
					ui->drawMessage("Draw!");
					done = true;
					continue;
				}
			}

			// 5) Perform AB Prune to make move
			input = ai->ABPrune(board, players, kings, INT_MIN, INT_MAX, curPlayer, 0);

			msg = playables[input.x].piece->typeStr() + " Moved From [" + std::to_string(playables[input.x].piece->rank()) + "," + (char)(playables[input.x].piece->file()) + "] To [" + std::to_string((int)(playables[input.x].moves[input.y].coord.x + 1)) + "," + (char)(playables[input.x].moves[input.y].coord.y + 97) + "]\n";
			ui->drawMessage(msg);

		}

		// Human Player Turn

		else {

			msg = "\nPlayer " + std::to_string((curPlayer + 1)) + "'s Turn.\n";
			ui->drawMessage(msg);
			ui->drawCaptured(players);

			// 2) Test to see if King is in Check
			if (board.isThreatened(*kings[curPlayer])) {
				inCheck = true;
				if (kings[curPlayer]->getStatus() == Piece::Status::NORMAL) kings[curPlayer]->setStatus(Piece::Status::CHECK);
				else kings[curPlayer]->setStatus(Piece::Status::PRISTINE_CHECK);
			}
			else if (kings[curPlayer]->getStatus() == Piece::Status::PRISTINE_CHECK) kings[curPlayer]->setStatus(Piece::Status::PRISTINE);
			else if (kings[curPlayer]->getStatus() == Piece::Status::CHECK) kings[curPlayer]->setStatus(Piece::Status::NORMAL);

			// 3) Generate list of moves for current player
			playables = board.genPlayables(players[curPlayer].getOwned(), *(kings[curPlayer]));

			// 4) Check end conditions
			if (playables.size() == 0) {
				if (inCheck) {	// Other player won
					ui->drawMessage("Player " + std::to_string((1 - curPlayer) + 1) + " Won!");	// TODO - make non-current player number print here
				}
				else {			// draw
					ui->drawMessage("Draw!");
				}
				done = true;
				continue;
			}
			else {	// Check for only kings left
				bool onlyKings = true;
				for (int i = 0; i < players[0].getOwned().size(); i++) {
					if (players[0].getOwned()[i]->type() != Piece::Type::KING && players[0].getOwned()[i]->isOnBoard() == true) {
						onlyKings = false;
						break;
					}
				}
				for (int i = 0; i < players[1].getOwned().size(); i++) {
					if (players[1].getOwned()[i]->type() != Piece::Type::KING && players[1].getOwned()[i]->isOnBoard() == true) {
						onlyKings = false;
						break;
					}
				}
				if (onlyKings) {
					ui->drawMessage("Draw!");
					done = true;
					continue;
				}
			}

			// 5) Prompt Current Player to make a move
			input = ui->promptMove(playables);	

			// Check for Forfeiture
			if (input.x == -1 || input.y == -1) {
				msg = "\nPlayer " + std::to_string(curPlayer+1) + " Forfeits.\n";
				ui->drawMessage(msg);
				msg = "Player " + std::to_string((1 - curPlayer) + 1) + " Wins!\n";
				ui->drawMessage(msg);

				done = true;
				continue;
			}
		}


		// After AI or Human Player's Turn

		// 6) Update Game State, Update UI
		captured = board.move(playables[input.x].piece,playables[input.x].moves[input.y]);
		if (captured) {
			players[curPlayer].capture(captured);	// Add captured piece [if there is one] to current players captured list
			msg = "Captured : " + captured->teamStr() + "," +  captured->typeStr();
			ui->drawMessage(msg);
		}

		// 7) Handle Pawn Promotion
		if (playables[input.x].piece->type() == Piece::Type::PAWN && playables[input.x].piece->canPromote()) {
			if (aiEnabled && curPlayer == 1) {
				playables[input.x].piece->promote(Piece::Type::QUEEN);
			}
			else {
				playables[input.x].piece->promote(ui->promptPromote());
			}
		}

		ui->drawBoard(board);

		// 8) Swap Current Player and Repeat
		curPlayer = 1 - curPlayer;
	}

	// Cleanup
	if (aiEnabled) delete ai;
}