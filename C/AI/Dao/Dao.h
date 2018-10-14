// Dao.h:  Header file for Dao game.


#include "constants.h"
#include "state.h"
#include "helper.h"
#include "player.h"
#include "player1.h"
#include "player2.h"

// class declaration section for class Game
class Game
{
private:
	State state;
	Player1 player1;
	Player2 player2;
	bool gameover;
	unsigned short winner;
	unsigned short round;
	unsigned short firstplayer;
	unsigned short secondplayer;
	ofstream fileout;	// For creating a gamelog.

public:
	Game();
	bool isover();
	bool validmove(Move move);
	void showstate();
	void playround();
	~Game() { fileout.close(); }	// Close gamelog file.
};

// class implementation section for class Game
Game::Game()
{
	unsigned short response;
	
	gameover = false;
	winner = 0;
	round = 1;

	// Open the gamelog file.
	fileout.open("dao.log");
	if (fileout.fail()) {
		cout << "\nUnable to open dao.log - program terminating.\n";
		exit(1);
	}

	do {
		cout << "Player to move first (1 or 2): ";
		cin >> response;
	} while (response != PLAYER1 && response != PLAYER2);

	if (response == PLAYER1) {
		firstplayer = PLAYER1;
		secondplayer = PLAYER2;
	}
	else {
		firstplayer = PLAYER2;
		secondplayer = PLAYER1;
	}

}

void Game::showstate()
{
	unsigned short x, y;
	board game_board;

	cout << endl;
	fileout << endl;

	if (gameover) {
		cout << "Game over - winner is player: " << winner << "!\n";
		fileout << "Game over - winner is player: " << winner << "!\n";
	}
	else {
		cout << "Round #: " << round << endl;
		fileout << "Round #: " << round << endl;
	}

	game_board = state.get_game_state();

	for (y = 0; y < XYDIM; y++) {
		for (x =0; x < XYDIM; x++) {
			cout << setw(3) << state.getloc(x, y);
			fileout << setw(3) << state.getloc(x, y);
		}

		cout << endl;
		fileout << endl;
	}
	cout << endl;
	fileout << endl;

}

void Game::playround()
{

	bool firsttry;
	Move firstplayermove, secondplayermove;
	board game_board;

	// Display game board.
	showstate();

	// Get game_board to pass to players. Fix due to Bracken Neff (and others).
	game_board = state.get_game_state();

	firsttry = true;
	do {
		if (!firsttry) {
			cout << "Invalid move!\n";
			fileout << "Invalid move!\n";
		}
		
		firsttry = false;

		cout << "Player " << setw(2) << firstplayer << " move:\n";
		fileout << "Player " << setw(2) << firstplayer << " move:\n";

		if (firstplayer == PLAYER1)
			firstplayermove = player1.get_move(firstplayer, game_board);
		else
			firstplayermove = player2.get_move(firstplayer, game_board);

	} while (validmove(firstplayermove) != true);

	// Register first player's move on game board.
	state.setloc(firstplayermove.src_x, firstplayermove.src_y, EMPTY);
	state.setloc(firstplayermove.dst_x, firstplayermove.dst_y, firstplayermove.player);

	// Display game board.
	showstate();

	if (!isover()) {
		// Get game_board to pass to players. Fix due to Jared Short.
		game_board = state.get_game_state();

		firsttry = true;
		do {
			if (!firsttry) {
				cout << "Invalid move!\n";
				fileout << "Invalid move!\n";
			}

			firsttry = false;

			cout << "Player " << setw(2) << secondplayer << " move:\n";
			fileout << "Player " << setw(2) << secondplayer << " move:\n";

			if (firstplayer == PLAYER1)
				secondplayermove = player2.get_move(secondplayer, game_board);
			else
				secondplayermove = player1.get_move(secondplayer, game_board);

		} while (validmove(secondplayermove) != true);

		// Register second player's move on game board.
		state.setloc(secondplayermove.src_x, secondplayermove.src_y, EMPTY);
		state.setloc(secondplayermove.dst_x, secondplayermove.dst_y, secondplayermove.player);

	}
	
	round++;
}

bool Game::validmove(Move move)
{
	short x, y;
	board game_board;
	bool allclear;

	// Get current state of the game.
	game_board = state.get_game_state();

	// First, verify that a player isn't trying to move another's stone.
	if (move.player != game_board.layout[move.src_y][move.src_x])
		return false;

	// Second, verify that a player actually tries to move somewhere...
	if (move.dst_y == move.src_y && move.dst_x == move.src_x)  // Wow - really?
		return false;

	// Check for invalid 'North' move.
	if (move.dst_y < move.src_y && move.dst_x == move.src_x) {
		
		y = move.src_y - 1;
		allclear = true;

		while (allclear && y >= 0)
			if (game_board.layout[y][move.dst_x] != EMPTY)
				allclear = false;
			else
				y--;

		y++;	// Went past the valid move cell, so back it up.

		if (move.dst_y != y)
			return false;
	}

	// Check for invalid 'South' move.
	if (move.dst_y > move.src_y && move.dst_x == move.src_x) {
		
		y = move.src_y + 1;
		allclear = true;

		while (allclear && y < XYDIM)
			if (game_board.layout[y][move.dst_x] != EMPTY)
				allclear = false;
			else
				y++;

		y--;	// Went past the valid move cell, so back it up.

		if (move.dst_y != y)
			return false;
	}

	// Check for invalid 'East' move.
	if (move.dst_y == move.src_y && move.dst_x > move.src_x) {
		
		x = move.src_x + 1;
		allclear = true;

		while (allclear && x < XYDIM)
			if (game_board.layout[move.dst_y][x] != EMPTY)
				allclear = false;
			else
				x++;

		x--;	// Went past the valid move cell, so back it up.

		if (move.dst_x != x)
			return false;
	}

	// Check for invalid 'West' move.
	if (move.dst_y == move.src_y && move.dst_x < move.src_x) {
		
		x = move.src_x - 1;
		allclear = true;

		while (allclear && x >= 0)
			if (game_board.layout[move.dst_y][x] != EMPTY)
				allclear = false;
			else
				x--;

		x++;	// Went past the valid move cell, so back it up.

		if (move.dst_x != x)
			return false;
	}

	// Check for invalid 'Northeast' move.
	if (move.dst_y < move.src_y && move.dst_x > move.src_x) {
		
		x = move.src_x + 1;
		y = move.src_y - 1;
		allclear = true;

		while (allclear && y >= 0 && x < XYDIM)
			if (game_board.layout[y][x] != EMPTY)
				allclear = false;
			else {
				x++;
				y--;
			}

		x--;	// Went past the valid move cell, so back it up.
		y++;	

		if (move.dst_x != x || move.dst_y != y)
			return false;
	}

	// Check for invalid 'Northwest' move.
	if (move.dst_y < move.src_y && move.dst_x < move.src_x) {
		
		x = move.src_x - 1;
		y = move.src_y - 1;
		allclear = true;

		while (allclear && y >= 0 && x >= 0)
			if (game_board.layout[y][x] != EMPTY)
				allclear = false;
			else {
				x--;
				y--;
			}

		x++;	// Went past the valid move cell, so back it up.
		y++;	

		if (move.dst_x != x || move.dst_y != y)
			return false;
	}

	// Check for invalid 'Southeast' move.
	if (move.dst_y > move.src_y && move.dst_x > move.src_x) {
		
		x = move.src_x + 1;
		y = move.src_y + 1;
		allclear = true;

		while (allclear && y < XYDIM && x < XYDIM)
			if (game_board.layout[y][x] != EMPTY)
				allclear = false;
			else {
				x++;
				y++;
			}

		x--;	// Went past the valid move cell, so back it up.
		y--;	

		if (move.dst_x != x || move.dst_y != y)
			return false;
	}

	// Check for invalid 'Southwest' move.
	if (move.dst_y > move.src_y && move.dst_x < move.src_x) {
		
		x = move.src_x - 1;
		y = move.src_y + 1;
		allclear = true;

		while (allclear && y < XYDIM && x >= 0)
			if (game_board.layout[y][x] != EMPTY)
				allclear = false;
			else {
				x--;
				y++;
			}

		x++;	// Went past the valid move cell, so back it up.
		y--;	

		if (move.dst_x != x || move.dst_y != y)
			return false;
	}

	return true;   // Made it past all the checks - Valid move!
}

bool Game::isover()
{
	unsigned short x, y;
	board game_board;

	if (gameover)
		return gameover;
	
	// Determine if game is over. First, get current state of the game.
	game_board = state.get_game_state();

	// Check for a 'four corners' victory.
	if (game_board.layout[0][0] == game_board.layout[0][3] &&
		game_board.layout[0][0] == game_board.layout[3][0] &&
		game_board.layout[0][0] == game_board.layout[3][3] &&
		game_board.layout[0][0] != EMPTY) {

		gameover = true;
		winner = game_board.layout[0][0];

		return gameover;
	}
		
	// Check for a 'backdoor' victory.
	if (game_board.layout[0][0] != EMPTY && game_board.layout[0][1] != EMPTY &&
		game_board.layout[1][0] != EMPTY && game_board.layout[1][1] != EMPTY &&
		game_board.layout[0][0] != game_board.layout[0][1] &&
		game_board.layout[0][0] != game_board.layout[1][0] &&
		game_board.layout[0][0] != game_board.layout[1][1]) {

		gameover = true;
		winner = game_board.layout[0][0];

		return gameover;
	}
	if (game_board.layout[0][3] != EMPTY && game_board.layout[0][2] != EMPTY &&
		game_board.layout[1][3] != EMPTY && game_board.layout[1][2] != EMPTY &&
		game_board.layout[0][3] != game_board.layout[0][2] &&
		game_board.layout[0][3] != game_board.layout[1][3] &&
		game_board.layout[0][3] != game_board.layout[1][2]) {

		gameover = true;
		winner = game_board.layout[0][3];

		return gameover;
	}
	if (game_board.layout[3][0] != EMPTY && game_board.layout[3][1] != EMPTY &&
		game_board.layout[2][0] != EMPTY && game_board.layout[2][1] != EMPTY &&
		game_board.layout[3][0] != game_board.layout[3][1] &&
		game_board.layout[3][0] != game_board.layout[2][0] &&
		game_board.layout[3][0] != game_board.layout[2][1]) {

		gameover = true;
		winner = game_board.layout[3][0];

		return gameover;
	}
	if (game_board.layout[3][3] != EMPTY && game_board.layout[3][2] != EMPTY &&
		game_board.layout[2][3] != EMPTY && game_board.layout[2][2] != EMPTY &&
		game_board.layout[3][3] != game_board.layout[3][2] &&
		game_board.layout[3][3] != game_board.layout[2][3] &&
		game_board.layout[3][3] != game_board.layout[2][2]) {

		gameover = true;
		winner = game_board.layout[3][3];

		return gameover;
	}

	// Check for 'column' victory.
	for (x = 0; x < XYDIM; x++)
		if (game_board.layout[0][x] != EMPTY && game_board.layout[1][x] == game_board.layout[0][x]
											 && game_board.layout[2][x] == game_board.layout[0][x]
											 && game_board.layout[3][x] == game_board.layout[0][x]) {
			gameover = true;
			winner = game_board.layout[0][x];

			return gameover;
		}

	// Check for 'row' victory.
	for (y = 0; y < XYDIM; y++)
		if (game_board.layout[y][0] != EMPTY && game_board.layout[y][1] == game_board.layout[y][0]
											 && game_board.layout[y][2] == game_board.layout[y][0]
											 && game_board.layout[y][3] == game_board.layout[y][0]) {
			gameover = true;
			winner = game_board.layout[y][0];

			return gameover;
		}

	// Check for 'square cluster' victory.
	for (y = 0; y < XYDIM - 1; y++)
		for (x = 0; x < XYDIM - 1; x++)
			if (game_board.layout[y][x] != EMPTY && game_board.layout[y][x] == game_board.layout[y+1][x]
												 && game_board.layout[y][x] == game_board.layout[y][x+1]
												 && game_board.layout[y][x] == game_board.layout[y+1][x+1]) {
				gameover = true;
				winner = game_board.layout[y][x];

				return gameover;
			}

	return gameover;
}





