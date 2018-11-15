// player2.h: player 2 agent.
// Author:    Matt Kidder

// Original framework code provided by ActiveState Code Recipes
// downloaded on 4/19/2018 from 
// https://github.com/ActiveState/code/tree/master/recipes/C%2B%2B/577457_Astar_Shortest_Path_Algorithm


// ****************************************************************************
// Code heavily modified by Matt Kidder
// to make an intelligent Dao playing agent for using A* searching
// with a priority queue and a heuristic that uses
// weighted goal states 
// ****************************************************************************


// Key concepts for the A star algorithm was researched from
// http://heyes-jones.com/astar.php and pseudo code was used from
// http://heyes-jones.com/pseudocode.php with the key concepts of
// the corresponding 8-puzzle A star solution from
// https://github.com/justinhj/astar-algorithm-cpp

#ifndef PLAYER2_H
#define PLAYER2_H

#include "state.h"
#include "constants.h"
#include <queue>
#include <math.h>
#include <ctime>

using namespace std;

// Convert pre-existing constants into a namespace to avoid duplication issues
namespace player2 {
	const int n = 4; // horizontal size of the map
	const int m = 4; // vertical size size of the map
	static int map[n][m];
	static int closed_nodes_map[n][m]; // map of closed (tried-out) nodes
	static int open_nodes_map[n][m]; // map of open (not-yet-tried) nodes
	static int dir_map[n][m]; // map of directions
	const int dir = 8; // number of possible directions to go at any position
	static int dx[dir] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	static int dy[dir] = { 0, 1, 1, 1, 0, -1, -1, -1 };
}
// Class to keep track of various player statistics
class MKplayerstats2 {
	// for easy tracking of player number
	int player_num;
	// setup heuristic value array to track all 19 victories
	// 4 column, 4 row, 1 corners, 10 clusters
	// arr[0] is for horizontal hvalue, arr[1] is for vertical
	// arr[2] is for corners, arr[3] is for clusters
	double hvalue[4 + 1];
	// track each stone's yx coords
	// first encountered stone is at arr[1] = y, arr[2] = x,
	// second encountered stone is at arr[3] = y, arr[4] = x, etc.
	int player_loc[8 + 1];
public:
	MKplayerstats2() { player_num = 2; };
	int winner = 0;
	int getplayerNum() const { return player_num; };
	double gethvalue(int i) const { return hvalue[i]; };
	void sethvalue(double h, int i) { hvalue[i] = hvalue[i] + h; };
	int getplayerloc(int &i) { return player_loc[++i]; };
	void setplayerloc(int y, int x, int &i) { player_loc[++i] = y; player_loc[++i] = x; };
};

// create a temporary test board using MKState2 class
// Class declaration section for class State2.
class MKState2
{
public:
	MKState2();
	board game_state;  // current state of the game board
	void setloc(unsigned short x, unsigned short y, unsigned short val) { game_state.layout[y][x] = val; }
	unsigned short getloc(unsigned short x, unsigned short y) { return game_state.layout[y][x]; }
};

// Class implementation section for class State2.
MKState2::MKState2()
{
	// Initialize game board.
	for (int row = 0; row < XYDIM; row++)
		for (int col = 0; col < XYDIM; col++)
			game_state.layout[row][col] = 0;

	// Place player 1 stones.
	game_state.layout[0][0] = game_state.layout[1][1] = game_state.layout[2][2]
		= game_state.layout[3][3] = 1;
	// Place player 2 stones.
	game_state.layout[0][3] = game_state.layout[1][2] = game_state.layout[2][1]
		= game_state.layout[3][0] = 2;
}

class MKnode2
{
	// current position
	int xPos;
	int yPos;
	// total distance already travelled to reach the MKnode2
	int level;
	// priority=level+heuristic
	int priority;
public:
	MKnode2(int xp, int yp, int d, int p)
	{
		xPos = xp; yPos = yp; level = d; priority = p;
	}

	int getxPos() const { return xPos; }
	int getyPos() const { return yPos; }
	int getLevel() const { return level; }
	int getPriority() const { return priority; }

	void updatePriority(board game_board)
	{
		//Truncate priority for pqueue
		priority = level + heuristic(game_board) * 10; //A*
	}

	// update level using pre-existing method
	void nextLevel(const int & i) // i: direction
	{
		level += (player2::dir == 8 ? (i % 2 == 0 ? 10 : 14) : 10);
	}

	// Heuristic algorithm to count amount of moves from victory. This implementation represents this
	// by counting total stones for the player in a particular configuraion for a 
	// goal state and multipying that count by a weight (currently weights are arbitrary)
	// to develop a total heuristic value for all possible victory conditions
	double heuristic(board game_board)
	{
		//Set weight values 
		double VERT = .2;
		double HORZ = .2;
		double CLUSTER = .2;
		double FOURC = .4;

		int x, y;
		double htotal[4 + 1] = {};
		double hsum = 0.0;
		MKplayerstats2 ps;
		int player_num = ps.getplayerNum();

		//Scan game board to see what four coordinate configuration can result in a win
		//and find out what is missing to do so (total amount of moves from a goal state)
		// 1 0 0 0
		// 0 1 0 0	This is only one move away for a win and should be highly valued
		// 1 0 0 0  3 stones are in place in one column so the heuristic value will be
		// 1 0 0 0  higher than if there were only 2 stones or 1 stone for this goal state


		// Column count
		for (x = 0; x < XYDIM; x++) {
			if (game_board.layout[0][x] != EMPTY && game_board.layout[0][x] == player_num)
				htotal[x] = htotal[x] + 1.0;
			if (game_board.layout[1][x] != EMPTY && game_board.layout[1][x] == player_num)
				htotal[x] = htotal[x] + 1.0;
			if (game_board.layout[2][x] != EMPTY && game_board.layout[2][x] == player_num)
				htotal[x] = htotal[x] + 1.0;
			if (game_board.layout[3][x] != EMPTY && game_board.layout[3][x] == player_num)
				htotal[x] = htotal[x] + 1.0;
			// Count done, now set actual heuristic value
			htotal[x] = htotal[x] * VERT;
		}
		// Add all heuristic totals for this goal state to overall heuristic sum
		// and reset goal state heuristic total/save heuristic total
		for (x = 0; x < XYDIM; x++) {
			hsum = hsum + htotal[x];
			ps.sethvalue(htotal[x], 0);
			htotal[x] = 0;
		}
		// Row count
		for (x = 0; x < XYDIM; x++) {
			if (game_board.layout[x][0] != EMPTY && game_board.layout[x][0] == player_num)
				htotal[x] = htotal[x] + 1.0;
			if (game_board.layout[x][1] != EMPTY && game_board.layout[x][1] == player_num)
				htotal[x] = htotal[x] + 1.0;
			if (game_board.layout[x][2] != EMPTY && game_board.layout[x][2] == player_num)
				htotal[x] = htotal[x] + 1.0;
			if (game_board.layout[x][3] != EMPTY && game_board.layout[x][3] == player_num)
				htotal[x] = htotal[x] + 1.0;

			htotal[x] = htotal[x] * HORZ;
		}
		for (x = 0; x < XYDIM; x++) {
			hsum = hsum + htotal[x];
			ps.sethvalue(htotal[x], 1);
			htotal[x] = 0;
		}

		// Four corners (only 1 goal state to check so magic numbers abound)
		if (game_board.layout[0][0] != EMPTY && game_board.layout[0][0] == player_num)
			htotal[0] = htotal[0] + 1.0;
		if (game_board.layout[0][3] != EMPTY && game_board.layout[0][3] == player_num)
			htotal[0] = htotal[0] + 1.0;
		if (game_board.layout[3][0] != EMPTY && game_board.layout[3][0] == player_num)
			htotal[0] = htotal[0] + 1.0;
		if (game_board.layout[3][3] != EMPTY && game_board.layout[3][3] == player_num)
			htotal[0] = htotal[0] + 1.0;

		htotal[0] = htotal[0] * FOURC;
		hsum = hsum + htotal[0];
		ps.sethvalue(htotal[0], 2);
		htotal[0] = 0;

		// Cluster
		for (y = 0; y < XYDIM - 1; y++) {
			for (x = 0; x < XYDIM - 1; x++) {
				if (game_board.layout[y][x] != EMPTY &&
					game_board.layout[y][x] == player_num)
					htotal[y] = htotal[y] + 1.0;
				if (game_board.layout[y][x] != EMPTY &&
					game_board.layout[y + 1][x] == player_num)
					htotal[y] = htotal[y] + 1.0;
				if (game_board.layout[y][x] != EMPTY &&
					game_board.layout[y][x + 1] == player_num)
					htotal[y] = htotal[y] + 1.0;
				if (game_board.layout[y][x] != EMPTY &&
					game_board.layout[y + 1][x + 1] == player_num)
					htotal[y] = htotal[y] + 1.0;
			}
			htotal[y] = htotal[y] * CLUSTER;
		}
		for (x = 0; x < XYDIM; x++) {
			hsum = hsum + htotal[x];
			ps.sethvalue(htotal[x], 3);
			htotal[x] = 0;
		}

		return hsum;

		//Left original distance estimations for reference

		//static int xd, yd, d;
		//xd = xDest - xPos;
		//yd = yDest - yPos;

		// Euclidian Distance
		//d = static_cast<int>(sqrt(xd*xd + yd * yd));

		// Manhattan distance
		//d=abs(xd)+abs(yd);

		// Chebyshev distance
		//d=max(abs(xd), abs(yd));

		//return(d);
	}
};

// Determine priority (in the priority queue)
bool operator<(const MKnode2 & a, const MKnode2 & b)
{
	return a.getPriority() > b.getPriority();
}

class Player2 : public Player
{
public:
	Move get_move(unsigned short p, board game_board);
	void rand_gen(Move &move, board game_board, MKplayerstats2 &ps, MKState2 &state);
	bool isoverP(Move move, board game_board, MKplayerstats2 &ps, MKState2 &state);
};

Move Player2::get_move(unsigned short p, board game_board)
{
	Move move;
	MKState2 state;
	int winner = 0;
	move.player = p;

	if (gettype()) {
		cout << "Enter move (old_x old_y new_x new_y): ";
		cin >> move.src_x;
		cin >> move.src_y;
		cin >> move.dst_x;
		cin >> move.dst_y;
	}
	else {
		// A-star algorithm.
		static priority_queue<MKnode2> pq[2]; // list of open (not-yet-tried) nodes
		static int pqi; // pq index
		static MKnode2* n0;
		static MKnode2* m0;
		int xStart, yStart;
		MKplayerstats2 ps;
		int playernum = ps.getplayerNum();
		int iter = 0;
		static int i, j, x, y, xdx, ydy;
		static char c;
		pqi = 0;

		//cout << "I belong to Matt Kidder and I am in " << playernum << "'s seat!\n\n";
		// reset the MKnode2 maps
		for (y = 0; y < player2::m; y++)
		{
			for (x = 0; x < player2::n; x++)
			{
				player2::closed_nodes_map[x][y] = 0;
				player2::open_nodes_map[x][y] = 0;
				// get current yx values for our player and save in array 
				if (game_board.layout[y][x] == playernum) {
					ps.setplayerloc(y, x, iter);
				}
			}
		}

		iter = 0;

		for (int tot_stone = 0; tot_stone < 4; tot_stone++) {
			// create the start MKnode2 and push into list of open nodes
			move.src_y = yStart = ps.getplayerloc(iter);
			move.src_x = xStart = ps.getplayerloc(iter);
			n0 = new MKnode2(xStart, yStart, 0, 0);
			n0->updatePriority(game_board);
			pq[pqi].push(*n0);
			player2::open_nodes_map[x][y] = n0->getPriority(); // mark it on the open nodes map

															   // A* search
			while (!pq[pqi].empty())
			{
				// get the current MKnode2 w/ the highest priority
				// from the list of open nodes
				n0 = new MKnode2(pq[pqi].top().getxPos(), pq[pqi].top().getyPos(),
					pq[pqi].top().getLevel(), pq[pqi].top().getPriority());

				x = n0->getxPos(); y = n0->getyPos();

				pq[pqi].pop(); // remove the MKnode2 from the open list
				player2::open_nodes_map[x][y] = 0;
				// mark it on the closed nodes map
				player2::closed_nodes_map[x][y] = 1;

				// set test coords
				move.dst_y = y;
				move.dst_x = x;
				// quit searching when the goal state is reached
				if (validmove(move, game_board) 
					&& isoverP(move, game_board, ps, state) 
					&& ps.winner == ps.getplayerNum())
				{
					// garbage collection
					delete n0;
					// empty the leftover nodes
					while (!pq[pqi].empty()) pq[pqi].pop();
					return move;
				}

				// generate moves (child nodes) in all possible directions
				for (i = 0; i < player2::dir; i++)
				{
					xdx = x + player2::dx[i]; ydy = y + player2::dy[i];

					if (!(xdx<0 || xdx>player2::n - 1 || ydy<0 || ydy>player2::m - 1 || player2::map[xdx][ydy] == 1
						|| player2::closed_nodes_map[xdx][ydy] == 1))
					{
						// generate a child MKnode2
						m0 = new MKnode2(xdx, ydy, n0->getLevel(),
							n0->getPriority());
						m0->nextLevel(i);
						m0->updatePriority(game_board);

						// if it is not in the open list then add into that
						if (player2::open_nodes_map[xdx][ydy] == 0)
						{
							player2::open_nodes_map[xdx][ydy] = m0->getPriority();
							pq[pqi].push(*m0);
							// mark its parent MKnode2 direction
							player2::dir_map[xdx][ydy] = (i + player2::dir / 2) % player2::dir;
						}
						else if (player2::open_nodes_map[xdx][ydy] > m0->getPriority())
						{
							// update the priority info
							player2::open_nodes_map[xdx][ydy] = m0->getPriority();
							// update the parent direction info
							player2::dir_map[xdx][ydy] = (i + player2::dir / 2) % player2::dir;

							// replace the MKnode2
							// by emptying one pq to the other one
							// except the MKnode2 to be replaced will be ignored
							// and the new MKnode2 will be pushed in instead
							while (!(pq[pqi].top().getxPos() == xdx &&
								pq[pqi].top().getyPos() == ydy))
							{
								pq[1 - pqi].push(pq[pqi].top());
								pq[pqi].pop();
							}
							pq[pqi].pop(); // remove the wanted MKnode2

										   // empty the larger size pq to the smaller one
							if (pq[pqi].size() > pq[1 - pqi].size()) pqi = 1 - pqi;
							while (!pq[pqi].empty())
							{
								pq[1 - pqi].push(pq[pqi].top());
								pq[pqi].pop();
							}
							pqi = 1 - pqi;
							pq[pqi].push(*m0); // add the better MKnode2 instead
						}
						else delete m0; // garbage collection
					}
				}
				delete n0; // garbage collection
			}
		}

		// generate random move if we got here without one
		if (!validmove(move, game_board))
			rand_gen(move, game_board, ps, state);

	}

	return move;
}

bool Player2::isoverP(Move move, board game_board, MKplayerstats2 &ps, MKState2 &state)
{
	unsigned short x, y;
	bool gameover = false;
	board test_board;
	// Copy current real game board to test game
	state.game_state = game_board;
	// Make test move
	state.setloc(move.src_x, move.src_y, EMPTY);
	state.setloc(move.dst_x, move.dst_y, ps.getplayerNum());
	// Setup test board state to be checked
	test_board = state.game_state;

	// Check for a 'four corners' victory.
	if (test_board.layout[0][0] == test_board.layout[0][3] &&
		test_board.layout[0][0] == test_board.layout[3][0] &&
		test_board.layout[0][0] == test_board.layout[3][3] &&
		test_board.layout[0][0] != EMPTY) {

		gameover = true;
		ps.winner = test_board.layout[0][0];

		return gameover;
	}

	// Check for a 'backdoor' victory.
	if (test_board.layout[0][0] != EMPTY && test_board.layout[0][1] != EMPTY &&
		test_board.layout[1][0] != EMPTY && test_board.layout[1][1] != EMPTY &&
		test_board.layout[0][0] != test_board.layout[0][1] &&
		test_board.layout[0][0] != test_board.layout[1][0] &&
		test_board.layout[0][0] != test_board.layout[1][1]) {

		gameover = true;
		ps.winner = test_board.layout[0][0];

		return gameover;
	}
	if (test_board.layout[0][3] != EMPTY && test_board.layout[0][2] != EMPTY &&
		test_board.layout[1][3] != EMPTY && test_board.layout[1][2] != EMPTY &&
		test_board.layout[0][3] != test_board.layout[0][2] &&
		test_board.layout[0][3] != test_board.layout[1][3] &&
		test_board.layout[0][3] != test_board.layout[1][2]) {

		gameover = true;
		ps.winner = test_board.layout[0][3];

		return gameover;
	}
	if (test_board.layout[3][0] != EMPTY && test_board.layout[3][1] != EMPTY &&
		test_board.layout[2][0] != EMPTY && test_board.layout[2][1] != EMPTY &&
		test_board.layout[3][0] != test_board.layout[3][1] &&
		test_board.layout[3][0] != test_board.layout[2][0] &&
		test_board.layout[3][0] != test_board.layout[2][1]) {

		gameover = true;
		ps.winner = test_board.layout[3][0];

		return gameover;
	}
	if (test_board.layout[3][3] != EMPTY && test_board.layout[3][2] != EMPTY &&
		test_board.layout[2][3] != EMPTY && test_board.layout[2][2] != EMPTY &&
		test_board.layout[3][3] != test_board.layout[3][2] &&
		test_board.layout[3][3] != test_board.layout[2][3] &&
		test_board.layout[3][3] != test_board.layout[2][2]) {

		gameover = true;
		ps.winner = test_board.layout[3][3];

		return gameover;
	}

	// Check for 'column' victory.
	for (x = 0; x < XYDIM; x++)
		if (test_board.layout[0][x] != EMPTY && test_board.layout[1][x] == test_board.layout[0][x]
			&& test_board.layout[2][x] == test_board.layout[0][x]
			&& test_board.layout[3][x] == test_board.layout[0][x]) {
			gameover = true;
			ps.winner = test_board.layout[0][x];

			return gameover;
		}

	// Check for 'row' victory.
	for (y = 0; y < XYDIM; y++)
		if (test_board.layout[y][0] != EMPTY && test_board.layout[y][1] == test_board.layout[y][0]
			&& test_board.layout[y][2] == test_board.layout[y][0]
			&& test_board.layout[y][3] == test_board.layout[y][0]) {
			gameover = true;
			ps.winner = test_board.layout[y][0];

			return gameover;
		}

	// Check for 'square cluster' victory.
	for (y = 0; y < XYDIM - 1; y++)
		for (x = 0; x < XYDIM - 1; x++)
			if (test_board.layout[y][x] != EMPTY && test_board.layout[y][x] == test_board.layout[y + 1][x]
				&& test_board.layout[y][x] == test_board.layout[y][x + 1]
				&& test_board.layout[y][x] == test_board.layout[y + 1][x + 1]) {
				gameover = true;
				ps.winner = test_board.layout[y][x];

				return gameover;
			}

	return gameover;
}
// Just-in-case we need to provide a move in desperate times
void Player2::rand_gen(Move &move, board game_board, MKplayerstats2 &ps, MKState2 &state) {
	srand(time(NULL));
	bool check = false;
	int iter = 0;

	do {
		move.src_x = rand() % 4;
		move.src_y = rand() % 4;
		move.dst_x = rand() % 4;
		move.dst_y = rand() % 4;

	} while (!validmove(move, game_board));
	
	ps.winner = 0;
	check = isoverP(move, game_board, ps, state);
	// Rando found winner, return
	if (check  && ps.winner == ps.getplayerNum())
		return;
	// Rando found non-loser, return
	if (!check)
		return;
	// Call again to make sure the rando move is not a suicide move
	// keep calling until a non-suicide move is generated
	if ( check && ps.winner != ps.getplayerNum())
		rand_gen(move, game_board, ps, state);
}
#endif
