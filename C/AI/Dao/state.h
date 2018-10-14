// state.h:	.h file for class State.

#ifndef STATE_H
#define STATE_H

// Data structure used by class State.

struct board
{
	unsigned short layout[XYDIM][XYDIM];
};

// Class declaration section for class State.
class State
{
private:
	board game_state;  // current state of the game board
public:
	State();
	board get_game_state();   // players can get state of the game board
	void setloc(unsigned short x, unsigned short y, unsigned short val) { game_state.layout[y][x] = val; }
	unsigned short getloc(unsigned short x, unsigned short y) { return game_state.layout[y][x]; }
};

// Class implementation section for class State.
State::State()
{
	// Initialize game board.
	for (int row = 0; row < XYDIM; row++)
		for (int col = 0; col < XYDIM; col++)
			game_state.layout[row][col] = EMPTY;
	
	// Place player 1 stones.
	game_state.layout[0][0] = game_state.layout[1][1] = game_state.layout[2][2] 
							= game_state.layout[3][3] = PLAYER1;
	// Place player 2 stones.
	game_state.layout[0][3] = game_state.layout[1][2] = game_state.layout[2][1] 
							= game_state.layout[3][0] = PLAYER2;
}

board State::get_game_state()
{
	return game_state;
}

#endif