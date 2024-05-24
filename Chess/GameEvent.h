#pragma once
#include "Player.h"
#include "Board.h"
#include "Enemy.h"
#include "ChessPiece.h"
#include <algorithm>
#include <memory>

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
	static inline bool check{ false };
	static inline sf::Vector2f up, down, left, right, topright, topleft, botright, botleft;

	void showPieces(sf::RenderWindow& window);

	void choosePiece(int x, int y);

	bool isChosen();

	void movePiece(int x, int y);

	void movePawn(int x, int y, std::vector<ChessPiece>& current_owned, 
					std::vector<ChessPiece>& rival_owned, sf::Vector2f& loc);
	
	bool pieceBlocked(int x, int y, std::vector<ChessPiece>& current_owned);

	bool pieceBlocked(int x, int y, std::vector<ChessPiece>& current_owned,
		std::vector<ChessPiece>& rival_owned);

	bool eatEnemy(int x, int y, std::vector<ChessPiece>& rival_owned);

	void moveKnight(int x, int y, std::vector<ChessPiece>& current_owned, 
		std::vector<ChessPiece>& rival_owned, sf::Vector2f& loc);

	void moveBishop(int x, int y, std::vector<ChessPiece>& current_owned,
		std::vector<ChessPiece>& rival_owned, sf::Vector2f& loc);

	void moveQueen(int x, int y, std::vector<ChessPiece>& current_owned,
		std::vector<ChessPiece>& rival_owned, sf::Vector2f& loc);
	
	bool onWayBlocked(int target_x, int target_y, int dir_x, int dir_y,
		std::vector<ChessPiece>& current_owned, std::vector<ChessPiece>& rival_owned);

	void moveRook(int x, int y, std::vector<ChessPiece>& current_owned,
		std::vector<ChessPiece>& rival_owned, sf::Vector2f& loc);

	void moveKing(int x, int y, std::vector<ChessPiece>& current_owned,
		std::vector<ChessPiece>& rival_owned, sf::Vector2f& loc);

	bool castlingMove(int rook_x, int rook_y, 
		std::vector<ChessPiece>& current_owned, int offsetX);

	bool checkSeeker();

	bool checkSeeker(int x, int y);

	bool helperKing(int x, int y, );
};

