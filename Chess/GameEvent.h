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
	bool m_playerturn{ true };
public:
	GameEvent(Player& player, Board& board, Enemy& enemy);

	bool first_time{ true };
	std::vector<ChessPiece>& player_owned{ m_player.returnVector() };
	std::vector<ChessPiece>& enemy_owned{ m_enemy.returnVector() };
	std::vector<sf::Sprite>& board_vector{ m_board.returnVector() };

	void showPieces(sf::RenderWindow& window);

	void choosePiece(int x, int y);

	bool isChosen();

	void movePiece(int x, int y);

	void movePawn(int x, int y, std::vector<ChessPiece>& current_owned, 
					std::vector<ChessPiece>& rival_owned, sf::Vector2f& loc);
	
	bool pieceBlocked(int x, int y, std::vector<ChessPiece>& rival_owned);

	bool eatEnemy(int x, int y, std::vector<ChessPiece>& rival_owned);

	void moveKnight(int x, int y, std::vector<ChessPiece>& current_owned, 
					std::vector<ChessPiece>& rival_owned, sf::Vector2f& loc);

	void moveBishop(int x, int y, std::vector<ChessPiece>& current_owned,
		std::vector<ChessPiece>& rival_owned, sf::Vector2f& loc);
};

