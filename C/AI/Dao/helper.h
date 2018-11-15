#pragma once
// validmove: 'helper' function for helping a player determine whether a move is valid.
// Inputs: a move and a game board.
// Output: true if move is valid, false otherwise.
bool validmove(Move move, board game_board)
{
	short x, y;
	bool allclear;

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

// isover: Checks whether the game is over.
// Inputs: a game board and a reference parameter for passing backa winner if any.
// Output: true if game is over, false otherwise.
bool isover(board game_board, int &winner)
{
	unsigned short x, y;
	bool gameover = false;

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
			if (game_board.layout[y][x] != EMPTY && game_board.layout[y][x] == game_board.layout[y + 1][x]
				&& game_board.layout[y][x] == game_board.layout[y][x + 1]
				&& game_board.layout[y][x] == game_board.layout[y + 1][x + 1]) {
				gameover = true;
				winner = game_board.layout[y][x];

				return gameover;
			}

	return gameover;
}