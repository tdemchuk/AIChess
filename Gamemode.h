#ifndef GAMEMODE_H
#define GAMEMODE_H

enum Difficulty {
	EASY,
	MEDIUM,
	HARD
};

struct Mode {
	int numPlayers;
	Difficulty diff;
};

#endif