#pragma once
#include "Player.h"
#include "Board.h"
#include "Enemy.h"
#include "ChessPiece.h"
#include <algorithm>
#include <memory>
#include <SFML/Audio.hpp>

class GameEvent
{
private:
	Player& m_player;
	Board& m_board;
	Enemy& m_enemy;
	static inline ChessPiece* m_chosen = nullptr;
	bool m_playerturn{ true };
	sf::RenderWindow& m_window;

	bool m_promotion{ false };
	bool m_check{ false };
	bool m_firsttime{ true }; // for showing the board the first time
	bool m_checkmate{ false };
	bool m_stale{ false };

	std::vector<ChessPiece> m_choices; // choices for pawn promotion

	// last eaten piece for recovery in movingAction eatEnemy
	// put it in eatEnemy to recover in movingAction to prevent blind eating
	ChessPiece* m_eaten = nullptr;
public:
	GameEvent(Player& player, Board& board, Enemy& enemy, sf::RenderWindow& window);
;
	std::vector<ChessPiece>& player_owned{ m_player.returnVector() };
	std::vector<ChessPiece>& enemy_owned{ m_enemy.returnVector() };
	std::vector<sf::Sprite>& board_vector{ m_board.returnVector() };

	void showPieces();

	void showBoard();

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

	void checkSeeker();

	bool checkSeeker(int x, int y, sf::Vector2f loc = m_chosen->returnSprite().getPosition());

	sf::Vector2f findKingLoc();

	void movingAction(int x, int y, bool not_blocked, std::vector<ChessPiece>& rival_owned,
		bool eat_enemy = false);

	void promotionCheck();

	bool pawnPromotion();

	void choosePromotion(int x, int y);

	void staleCheck();

	const bool isCheck();

	bool findHelper();

	void checkMate();

	bool oneKing();

	const bool isStale();

	const bool isCheckmate();

	void showCheckmate();
};

