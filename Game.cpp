
#include "Game.h"
#include "Player.h"
#include "AI.h" //Daniel Edit

/* GAME CLASS DEFINITIONS */

void Game::play(UI* ui, Mode mode) {

	// Local Vars
	Gameboard	board;
	AI*			ai			= nullptr;
	int			curPlayer	= 0;				// current player in game
	bool		done		= false;			// If Game has finished [win/loss/draw occurred]
	std::vector<Gameboard::Playable> playables;	// List of moves current player can make
	bool		inCheck		= false;			// If the current players King is in check at the start of their turn
	glm::vec2	input;							// User Input 
	Piece*		captured	= nullptr;			// captured piece placeholder
	bool		aiEnabled	= mode.numPlayers == 1 ? true : false;		// if the AI is standing in for player 2 or not

	// 1) Init Players, Pieces
	Player players[] = {Player(Color::WHITE), Player(Color::BLACK)};
	Piece* kings[2];

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
	players[1].create(Piece::Type::KNIGHT, glm::vec2(7 ,1), board);
	players[1].create(Piece::Type::BISHOP, glm::vec2(7, 2), board);
	players[1].create(Piece::Type::QUEEN, glm::vec2(7, 3), board);
	kings[1] = players[1].create(Piece::Type::KING, glm::vec2(7, 4), board);		// store created king
	players[1].create(Piece::Type::BISHOP, glm::vec2(7, 5), board);
	players[1].create(Piece::Type::KNIGHT, glm::vec2(7, 6), board);
	players[1].create(Piece::Type::ROOK, glm::vec2(7, 7), board);
	for (int i = 0; i < board.COL; i++) {
		players[1].create(Piece::Type::PAWN, glm::vec2(6, i), board);
	}

	ui->drawBoard(board);
	
	// Daniel Edit - Initialize AI here
	if (aiEnabled) {

		//TODO - Integrate Mode into the constructor
		ai = new AI(2);

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


		// Daniel Edit - Check if it's the AI's turn
		if (aiEnabled && curPlayer == 1) {

			//Perform AB Prune if so
			input = ai->ABPrune(board, players, kings, INT_MIN, INT_MAX, curPlayer, 0);

			//Generate playables for AI
			playables = board.genPlayables(players[curPlayer].getOwned(), *(kings[curPlayer]));

			std::cout << "\n Piece chosen: " << input[0];
			std::cout << "\n Move chosen: " << input[1] << "\n";

		}

		// Daniel Edit - Human Player Turn

		else {
			// 2) Generate list of moves for current player
			playables = board.genPlayables(players[curPlayer].getOwned(), *(kings[curPlayer]));		// TODO - debug write access violation error - nullptr issue
			ui->drawBoard(board);
			// 3) Test to see if king is in check
			inCheck = board.isThreatened(*(kings[curPlayer]));

			// 4) Check end conditions
			if (playables.size() == 0) {
				if (inCheck) {	// Other player won
					ui->drawMessage("Player __ Won!");	// TODO - make non-current player number print here
				}
				else {			// draw
					ui->drawMessage("Draw!");
				}
				done = true;
				continue;
			}

			// 5) Prompt Current Player to make a move
			input = ui->promptMove(playables);	// TODO - make function return user selected move
		}


		// Daniel Edit - After AI or Human Player's Turn

		// 6) Update Game State, Update UI
			// TODO - update game state based on user selected move
		captured = board.move(playables[input.x].piece,playables[input.x].moves[input.y]);
		if (captured) players[curPlayer].capture(captured);	// Add captured piece [if there is one] to current players captured list

		// 7) Handle Pawn Promotion
		if (playables[input.x].piece->type() == Piece::Type::PAWN && playables[input.x].piece->canPromote()) {
			playables[input.x].piece->promote(ui->promptPromote());
		}

		ui->drawBoard(board);

		// 8) Swap Current Player and Repeat
		curPlayer = 1 - curPlayer;

		//if (curPlayer == 0) done = true;		// DEBUG
		//done = true;
	}

	// Cleanup
	if (aiEnabled) delete ai;
}