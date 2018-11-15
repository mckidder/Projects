// player.h:	Player for Dao game.

#ifndef PLAYER_H
#define PLAYER_H

// class declaration section for class Player
class Player
{
private:
	bool human;	// Human or computer player?
public:
	Player();
	Move get_move(unsigned short p, board game_board);
	void settype(bool x) { human = x; }
	bool gettype() { return human; }
	~Player() {}
};

// class implementation section for class Player
Player::Player()
{
	char response;

	do {
		cout << "Is player human? (Enter 'y', 'Y', 'n', or 'N'): ";
		cin >> response;
	} while (response != 'y' && response != 'Y' && response != 'n' && response != 'N');
	
	if (response == 'y' || response =='Y')
		human = true;
	else
		human = false;
}

Move Player::get_move(unsigned short p, board game_board)
{
	Move move;
 
	move.player = p;

	if (human) {
		cout << "Enter move (old_x old_y new_x new_y): ";
		cin >> move.src_x;
		cin >> move.src_y;
		cin >> move.dst_x;
		cin >> move.dst_y;
	}
	else {
		move.src_x = 0;
		move.src_y = 0;
		move.dst_x = 0;
		move.dst_y = 0;
	}

	return move;
}

#endif