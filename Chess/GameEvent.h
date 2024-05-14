#pragma once
#include "Player.h"
#include "Board.h"
#include "Enemy.h"
#include "ChessPiece.h"

class GameEvent
{
private:
	Player& m_player;
	Board& m_board;
	Enemy& m_enemy;
	ChessPiece* m_chosen = nullptr;
public:
	GameEvent(Player& player, Board& board, Enemy& enemy);

	bool first_time{ true };
	std::vector<ChessPiece>& player_owned{ m_player.returnVector() };
	std::vector<ChessPiece>& enemy_owned{ m_enemy.returnVector() };

	void showPieces(sf::RenderWindow& window);

	void choosePiece(int x, int y);

	bool returnChosen();

	void movePiece(int x, int y);
};

