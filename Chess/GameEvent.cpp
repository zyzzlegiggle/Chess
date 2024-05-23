#include "GameEvent.h"

GameEvent::GameEvent(Player& player, Board& board, Enemy& enemy)
	: m_player{ player }
	, m_board{ board }
	, m_enemy{ enemy }
{
}

void GameEvent::showPieces(sf::RenderWindow& window)
{
	std::size_t index{};

	if (first_time)
	{
		// player side
		float origin_x{ 32 }; // because origin is in center so add 64/2
		float origin_y{ 416 }; // same here
		for (std::size_t i{ 0 }; i < 2; i++)
		{
			for (std::size_t j{ 0 }; j < 8; j++)
			{
				sf::Sprite& p{ player_owned[index].returnSprite() };
				p.setPosition(origin_x, origin_y);
				p.setOrigin(p.getLocalBounds().width / 2, p.getLocalBounds().height / 2);
				window.draw(player_owned[index++]);
				origin_x += 64;
			}
			origin_y += 64;
			origin_x = 32;
		}

		// enemy side
		// start from right top side
		origin_x = 480;
		origin_y = 32;
		index = enemy_owned.size() - 1;
		for (std::size_t i{ 0 }; i < 2; i++)
		{
			for (std::size_t j{ 0 }; j < 8; j++)
			{
				sf::Sprite& p{ enemy_owned[index].returnSprite() };
				p.setPosition(origin_x, origin_y);
				p.setOrigin(p.getLocalBounds().width / 2, p.getLocalBounds().height / 2);
				window.draw(enemy_owned[index--]);
				origin_x -= 64;
			}
			origin_y += 64;
			origin_x = 480;
		}
		first_time = false;
	}

	else
	{
		for (ChessPiece& p : player_owned)
		{
			window.draw(p);
		}

		for (ChessPiece& p : enemy_owned)
		{
			window.draw(p);
		}
	}

}

void GameEvent::choosePiece(int x, int y)
{
	std::vector<ChessPiece>& current_owned{ (m_playerturn) ? player_owned : enemy_owned };
	for (ChessPiece& piece : current_owned)
	{
		/*
		if (check)
		{
			if (piece.returnSprite().getGlobalBounds().contains(x, y) &&
				piece.returnPieceType() == ChessPiece::PieceType::KING)
			{
				m_chosen = &piece;
				break;
			}
			else
			{
				m_chosen = nullptr;
			}
		}
		*/
		if (piece.returnSprite().getGlobalBounds().contains(x, y))
		{
			m_chosen = &piece;
			break;
		}
		else
		{
			m_chosen = nullptr;
		}
	}
}

bool GameEvent::isChosen()
{
	if (m_chosen == nullptr)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void GameEvent::movePiece(int x, int y)
{
	//bool point_piece{ false };
	
	/*
	for (ChessPiece& piece : player_owned)
	{
		if (piece.returnSprite().getGlobalBounds().contains(x, y))
		{
			point_piece = true;
		}
	}
	*/

	

	// check if piece can be moved (not eaten yet)
	if (!m_chosen->isMovable())
	{
		m_chosen = nullptr;
		return;
	}

	std::vector<ChessPiece>& current_owned{ (m_playerturn) ? player_owned : enemy_owned };
	std::vector<ChessPiece>& rival_owned{ (m_playerturn) ? enemy_owned : player_owned };

	sf::Vector2f loc{ m_chosen->returnSprite().getPosition() };

	// basically find the length from piece to tile
	for (sf::Sprite& s : board_vector)
	{
		if (s.getGlobalBounds().contains(x, y))
		{
			// + 32 because chess origin is in middle of sprite
			// note this
			x = s.getPosition().x - loc.x + 32;
			y = s.getPosition().y - loc.y + 32;
			break;
		}
	}

	/*
	if (checkSeeker()
	{
		if (m_chosen->returnPieceType() == ChessPiece::PieceType::KING)
		{
			moveKing(x, y, current_owned, rival_owned, loc);
		}
	}
	else
	{
		
	}
	*/
	// remember to always return nullptr if wrong move inside particular function
	switch (m_chosen->returnPieceType())
	{
	case ChessPiece::PieceType::PAWN:
		movePawn(x, y, current_owned, rival_owned, loc);
		break;
	case ChessPiece::PieceType::KNIGHT:
		moveKnight(x, y, current_owned, rival_owned, loc);
		break;
	case ChessPiece::PieceType::BISHOP:
		moveBishop(x, y, current_owned, rival_owned, loc);
		break;
	case ChessPiece::PieceType::QUEEN:
		moveQueen(x, y, current_owned, rival_owned, loc);
		break;
	case ChessPiece::PieceType::ROOK:
		moveRook(x, y, current_owned, rival_owned, loc);
		break;
	case ChessPiece::PieceType::KING:
		moveKing(x, y, current_owned, rival_owned, loc);
		break;
	}
	
	// if player do a wrong move (clicking random tiles) 
	// stay at player turn
	// else, enemy turn and reset pointer
	if (!isChosen())
	{
		m_playerturn = m_playerturn;
	}
	else
	{
		m_playerturn = !m_playerturn;
		m_chosen = nullptr;
	}
	

}

void GameEvent::movePawn(int x, int y, std::vector<ChessPiece>& current_owned, 
							std::vector<ChessPiece>& rival_owned, sf::Vector2f& loc)
{
	bool first_time{};
	bool eat_enemy{ false };

	// check if pawn is in default position. if so enable double move skill
	first_time = (loc.y == 416 || loc.y == 96);


	// prevent moving back
	if ((m_playerturn && y >= 0) || (!m_playerturn && y <= 0))
	{
		m_chosen = nullptr;
		return;
	}

	// allow double tile but if chose other than front tile, pointer null
	if (first_time)
	{
		if (y < -160 || y > 160)
		{
			m_chosen = nullptr;
			return;
		}
	}
	else
	{
		if (y < -96 || y > 96)
		{
			m_chosen = nullptr;
			return;
		}
	}

	// diagonal move
	if ((x == -64 || x == 64) && (y != -160 || y != 160))
	{
		// check for enemy
		eat_enemy = eatEnemy(x, y, rival_owned);
	}

	// normal move
	if (!eat_enemy)
	{
		if (x == 0)
		{
			if ((!pieceBlocked(0, -64, current_owned, rival_owned) &&
				!pieceBlocked(0, -128, current_owned, rival_owned) &&
				!pieceBlocked(0, -64, rival_owned, rival_owned) &&
				!pieceBlocked(0, -128, rival_owned, rival_owned)) ||
				(!pieceBlocked(0, 64, current_owned, rival_owned) &&
					!pieceBlocked(0, 128, current_owned, rival_owned) &&
					!pieceBlocked(0, 64, rival_owned, rival_owned) &&
					!pieceBlocked(0, 128, rival_owned, rival_owned)))// yee yee ahh function
			{
				m_chosen->returnSprite().move(x, y);
			}
			else
			{
				m_chosen = nullptr;
			}
			
		}
		else
		{
			m_chosen = nullptr;
		}
	}
	
	
}

// rival_owned here is to check for checkmates purposes. if using rival_owned for owned,
// use rival_owned for rival_owned as well
// use this to check if the block is one tile in front (usually for pawn)
bool GameEvent::pieceBlocked(int x, int y, std::vector<ChessPiece>& owned, 
	std::vector<ChessPiece>& rival_owned)
{
	for (std::size_t i{ 0 }; i < owned.size(); i++)
	{
		if (owned[i].returnSprite().getGlobalBounds().
			contains(x + m_chosen->returnSprite().getPosition().x, y + m_chosen->returnSprite().getPosition().y))
		{
			return true;
		}
	}

	return false;
}

// use only 64 or -64 for dir_x or dir_y here
bool GameEvent::onWayBlocked(int target_x, int target_y, int dir_x, int dir_y, 
	std::vector<ChessPiece>& current_owned, std::vector<ChessPiece>& rival_owned)
{
	std::vector<bool> check_blocked{};
	int x_adder{ dir_x };
	int y_adder{ dir_y };

	while (dir_x != target_x || dir_y != target_y)
	{
		check_blocked.emplace_back(pieceBlocked(dir_x, dir_y, current_owned, rival_owned));
		check_blocked.emplace_back(pieceBlocked(dir_x, dir_y, rival_owned, rival_owned));
		dir_x += x_adder;
		dir_y += y_adder;
	}

	// in case player pointed to player piece because previous loop stops when dir == target
	// no need for enemy because will be checked on caller function (by eating?)
	check_blocked.emplace_back(pieceBlocked(dir_x, dir_y, current_owned, rival_owned)); 
	
	bool way_blocked = std::any_of(check_blocked.begin(), check_blocked.end(),
		[](bool v) {return v; });

	return way_blocked;
}

bool GameEvent::eatEnemy(int x, int y, std::vector<ChessPiece>& rival_owned)
{
	for (std::size_t i{ 0 }; i < rival_owned.size(); i++)
	{
		if (rival_owned[i].returnSprite().getGlobalBounds().
			contains(x + m_chosen->returnSprite().getPosition().x, y + m_chosen->returnSprite().getPosition().y))
		{
			rival_owned[i].returnSprite().setPosition(600, 500);
			rival_owned[i].isMovable() = false;
			m_chosen->returnSprite().move(x, y);
			return true;
		}
	}

	return false;
}

void GameEvent::moveKnight(int x, int y, std::vector<ChessPiece>& current_owned,
							std::vector<ChessPiece>& rival_owned, sf::Vector2f& loc)
{
	if ((y == 128 || y == -128) && (x == 64 || x == -64)) // vertical L
	{
		if (!pieceBlocked(x, y, current_owned, rival_owned))
		{
			if (!eatEnemy(x, y, rival_owned))
			{
				m_chosen->returnSprite().move(x, y);
			}
		}
		else
		{
			m_chosen = nullptr;
		}
	}
	else if ((y == 64 || y == -64) && (x == 128 || x == -128)) // horizontal L
	{
		if (!pieceBlocked(x, y, current_owned, rival_owned))
		{
			if (!eatEnemy(x, y, rival_owned))
			{
				m_chosen->returnSprite().move(x, y);
			}
		}
		else
		{
			m_chosen = nullptr;
		}
	}
	else
	{
		m_chosen = nullptr;
	}
}

void GameEvent::moveBishop(int x, int y, std::vector<ChessPiece>& current_owned,
	std::vector<ChessPiece>& rival_owned, sf::Vector2f& loc)
{
	bool not_blocked{ false };
	for (int i{ 0 }; i <= 512; i += 64)
	{
		int to_down{ 64 + i };
		int to_up{ -64 - i };
		int to_right{ 64 + i };
		int to_left{ -64 - i };

		// based on where its going, check if blocked and if pointed tile is blocked
		if (y == to_down)
		{
			if (x == to_right)
			{
				not_blocked = !onWayBlocked(x, y, 64, 64, current_owned, rival_owned);
					
			}
			else if (x == to_left)
			{
				not_blocked = !onWayBlocked(x, y, -64, 64, current_owned, rival_owned);
			}

			break;
		}
		else if (y == to_up)
		{
			if (x == to_right)
			{
				not_blocked = !onWayBlocked(x, y, 64, -64, current_owned, rival_owned);
				
			}
			else if (x == to_left)
			{
				not_blocked = !onWayBlocked(x, y, -64, -64, current_owned, rival_owned);
			}

			break;
		}
	}

	if (not_blocked)
	{
		if (!eatEnemy(x, y, rival_owned))
		{
			m_chosen->returnSprite().move(x, y);
		}
	}
	else
	{
		m_chosen = nullptr;
	}
}


void GameEvent::moveQueen(int x, int y, std::vector<ChessPiece>& current_owned,
	std::vector<ChessPiece>& rival_owned, sf::Vector2f& loc)
{
	bool not_blocked{ false };
	for (int i{ 0 }; i <= 512; i += 64)
	{
		int to_down{ 64 + i };
		int to_up{ -64 - i };
		int to_right{ 64 + i };
		int to_left{ -64 - i };

		// based on where its going, check if blocked and if pointed tile is blocked
		if (y == to_down)
		{
			if (x == to_right)
			{
				not_blocked = !onWayBlocked(x, y, 64, 64, current_owned, rival_owned);

			}
			else if (x == to_left)
			{
				not_blocked = !onWayBlocked(x, y, -64, 64, current_owned, rival_owned);
			}
			else if (x == 0)
			{
				not_blocked = !onWayBlocked(x, y, 0, 64, current_owned, rival_owned);
			}

			break;
		}
		else if (y == to_up)
		{
			if (x == to_right)
			{
				not_blocked = !onWayBlocked(x, y, 64, -64, current_owned, rival_owned);

			}
			else if (x == to_left)
			{
				not_blocked = !onWayBlocked(x, y, -64, -64, current_owned, rival_owned);
			}
			else if (x == 0)
			{
				not_blocked = !onWayBlocked(x, y, 0, -64, current_owned, rival_owned);
			}
			break;
		}
		else if (y == 0)
		{
			if (x == to_right)
			{
				not_blocked = !onWayBlocked(x, y, 64, 0, current_owned, rival_owned);
			}
			else if (x == to_left)
			{
				not_blocked = !onWayBlocked(x, y, -64, 0, current_owned, rival_owned);
			}
		}
	}

	if (not_blocked)
	{
		if (!eatEnemy(x, y, rival_owned))
		{
			m_chosen->returnSprite().move(x, y);
		}
	}
	else
	{
		m_chosen = nullptr;
	}
}

void GameEvent::moveRook(int x, int y, std::vector<ChessPiece>& current_owned,
	std::vector<ChessPiece>& rival_owned, sf::Vector2f& loc)
{
	bool not_blocked{ false };
	for (int i{ 0 }; i <= 512; i += 64)
	{
		int to_down{ 64 + i };
		int to_up{ -64 - i };
		int to_right{ 64 + i };
		int to_left{ -64 - i };

		// based on where its going, check if blocked and if pointed tile is blocked
		if (y == to_down)
		{
			if (x == 0)
			{
				not_blocked = !onWayBlocked(x, y, 0, 64, current_owned, rival_owned);
			}

			break;
		}
		else if (y == to_up)
		{
			if (x == 0)
			{
				not_blocked = !onWayBlocked(x, y, 0, -64, current_owned, rival_owned);
			}

			break;
		}
		else if (y == 0)
		{
			if (x == to_right)
			{
				not_blocked = !onWayBlocked(x, y, 64, 0, current_owned, rival_owned);
			}
			else if (x == to_left)
			{
				not_blocked = !onWayBlocked(x, y, -64, 0, current_owned, rival_owned);
			}
		}
	}

	if (not_blocked)
	{
		if (!eatEnemy(x, y, rival_owned))
		{
			m_chosen->returnSprite().move(x, y);
		}
	}
	else
	{
		m_chosen = nullptr;
	}
}

void GameEvent::moveKing(int x, int y, std::vector<ChessPiece>& current_owned,
	std::vector<ChessPiece>& rival_owned, sf::Vector2f& loc)
{
	bool not_blocked{ false };
	int to_down{ 64 };
	int to_up{ -64 };
	int to_right{ 64 };
	int to_left{ -64 };

	// castling check (white will always go first)
	
	if (m_playerturn)
	{
		static bool white_firsttime{ true };
		if (loc.x != 352 && loc.y != 480)
		{
			white_firsttime = false;
		}

		if (white_firsttime)
		{
			if (y == 0)
			{
				if ((x == to_right + 64 && !onWayBlocked(x, y, to_right, y, current_owned, rival_owned)))
				{
					if (castlingMove(loc.x + x + 64, loc.y, current_owned, -128))
					{
						m_chosen->returnSprite().move(x, y);
						white_firsttime = false;
						return;
					}

				}
				else if (x == to_left - 64 && !onWayBlocked(x, y, to_left, y, current_owned, rival_owned))
				{
					if (castlingMove(32, loc.y, current_owned, 192))
					{
						m_chosen->returnSprite().move(x, y);
						white_firsttime = false;
						return;
					}
				}
			}
		}
	}
	
	else 
	{
		static bool black_firsttime{ true };
		if (loc.x != 352 && loc.y != 32)
		{
			black_firsttime = false;
		}
		if (black_firsttime)
		{

			if (y == 0)
			{
				if ((x == to_right + 64 && !onWayBlocked(x, y, to_right, y, current_owned, rival_owned)))
				{
					if (castlingMove(loc.x + x + 64, loc.y, current_owned, -128))
					{
						m_chosen->returnSprite().move(x, y);
						black_firsttime = false;
						return;
					}

				}
				else if (x == to_left - 64 && !onWayBlocked(x, y, to_left, y, current_owned, rival_owned))
				{
					if (castlingMove(32, loc.y, current_owned, 192))
					{
						m_chosen->returnSprite().move(x, y);
						black_firsttime = false;
						return;
					}
				}
			}
		}
		
	}

	// based on where its going, check if blocked and if pointed tile is blocked
	if (y == to_down)
	{
		if (x == to_right && !checkSeeker(x + loc.x, y + loc.y, rival_owned, current_owned))
		{
			not_blocked = !onWayBlocked(x, y, to_right, to_down, current_owned, rival_owned);
		}
		else if (x == to_left && !checkSeeker(x + loc.x, y + loc.y, rival_owned, current_owned))
		{
			not_blocked = !onWayBlocked(x, y, to_left, to_down, current_owned, rival_owned);
		}
		else if (x == 0 && !checkSeeker(x + loc.x, y + loc.y, rival_owned, current_owned))
		{
			not_blocked = !onWayBlocked(x, y, x, to_down, current_owned, rival_owned);
		}
	}
	else if (y == to_up)
	{
		if (x == to_right && !checkSeeker(x + loc.x, y + loc.y, rival_owned, current_owned))
		{
			not_blocked = !onWayBlocked(x, y, to_right, to_up, current_owned, rival_owned);

		}
		else if (x == to_left && !checkSeeker(x + loc.x, y + loc.y, rival_owned, current_owned))
		{
			not_blocked = !onWayBlocked(x, y, to_left, to_up, current_owned, rival_owned);
		}
		else if (x == 0 && !checkSeeker(x + loc.x, y + loc.y, rival_owned, current_owned))
		{
			not_blocked = !onWayBlocked(x, y, x, to_up, current_owned, rival_owned);
		}
	}
	else if (y == 0)
	{
		if (x == to_right && !checkSeeker(x + loc.x, y + loc.y, rival_owned, current_owned))
		{
			not_blocked = !onWayBlocked(x, y, to_right, y, current_owned, rival_owned);
		}
		else if (x == to_left && !checkSeeker(x + loc.x, y + loc.y, rival_owned, current_owned))
		{
			not_blocked = !onWayBlocked(x, y, to_left, y, current_owned, rival_owned);
		}
	}

	if (not_blocked)
	{
		if (!eatEnemy(x, y, rival_owned))
		{
			m_chosen->returnSprite().move(x, y);
			// since move is allowed, means escaped from check
			check = false;
		}
	}
	else
	{
		m_chosen = nullptr;
	}
}

bool GameEvent::castlingMove(int rook_x, int rook_y, std::vector<ChessPiece>& current_owned, int offsetX)
{
	for (auto& p : current_owned)
	{
		if (p.returnSprite().getPosition().x == rook_x &&
			p.returnSprite().getPosition().y == rook_y &&
			p.returnPieceType() == ChessPiece::PieceType::ROOK)
		{
			p.returnSprite().move(offsetX, 0);
			return true;
		}
	}

	return false;
}

// function to check for king every turn
void GameEvent::checkSeeker()
{
	// for optimization purposes
	if (!check)
	{
		std::vector<ChessPiece>& current_owned{ (m_playerturn) ? player_owned : enemy_owned };
		std::vector<ChessPiece>& rival_owned{ (m_playerturn) ? enemy_owned : player_owned };
		ChessPiece* king = nullptr;

		for (ChessPiece& k : current_owned)
		{
			if (k.returnPieceType() == ChessPiece::PieceType::KING)
			{
				king = &k;
				break;
			}
		}

		sf::Vector2f loc{ king->returnSprite().getPosition() };

		bool up_check{ false }, down_check{ false }, left_check{ false },
			right_check{ false }, diag_topright{ false }, diag_topleft{ false },
			diag_botright{ false }, diag_botleft{ false };

		// checks here are executed once if a piece exist
		// if there is a piece not rook/queen in front of king, front check wont be done again
		// also check current side as well if blocked
		for (int i{ 0 }; i <= 512; i += 64)
		{
			int to_down{ 64 + i };
			int to_up{ -64 - i };
			int to_right{ 64 + i };
			int to_left{ -64 - i };

			for (std::size_t i{ 0 }; i < rival_owned.size(); i++)
			{
				// vertical

				// up
				if (!up_check)
				{
					if (rival_owned[i].returnSprite().getGlobalBounds().contains(0 + loc.x, to_up + loc.y))
					{
						if ((rival_owned[i].returnPieceType() == ChessPiece::PieceType::ROOK ||
							rival_owned[i].returnPieceType() == ChessPiece::PieceType::QUEEN))
						{
							check = true;
							break;
						}
						up_check = true;
					}
					else if (current_owned[i].returnSprite().getGlobalBounds().contains(0 + loc.x, to_up + loc.y))
					{
						up_check = true;
					}
				}


				// down
				if (!down_check)
				{
					if (rival_owned[i].returnSprite().getGlobalBounds().contains(0 + loc.x, to_down + loc.y))
					{
						if ((rival_owned[i].returnPieceType() == ChessPiece::PieceType::ROOK ||
							rival_owned[i].returnPieceType() == ChessPiece::PieceType::QUEEN))
						{
							check = true;
							break;
						}
						down_check = true;
					}
					else if (current_owned[i].returnSprite().getGlobalBounds().contains(0 + loc.x, to_down + loc.y))
					{
						down_check = true;
					}
				}

				// horizontal

				// left
				if (!left_check)
				{
					if (rival_owned[i].returnSprite().getGlobalBounds().contains(to_left + loc.x, 0 + loc.y))
					{
						if ((rival_owned[i].returnPieceType() == ChessPiece::PieceType::ROOK ||
							rival_owned[i].returnPieceType() == ChessPiece::PieceType::QUEEN))
						{
							check = true;
							break;
						}
						left_check = true;
					}
					else if (current_owned[i].returnSprite().getGlobalBounds().contains(to_left + loc.x, 0 + loc.y))
					{
						left_check = true;
					}
				}

				// right
				if (!right_check)
				{
					if (rival_owned[i].returnSprite().getGlobalBounds().contains(to_right + loc.x, 0 + loc.y))
					{
						if ((rival_owned[i].returnPieceType() == ChessPiece::PieceType::ROOK ||
							rival_owned[i].returnPieceType() == ChessPiece::PieceType::QUEEN))
						{
							check = true;
							break;
						}
						right_check = true;
					}
					else if (current_owned[i].returnSprite().getGlobalBounds().contains(to_right + loc.x, 0 + loc.y))
					{
						right_check = true;
					}
				}


				// diagonal
				// check for enemy pawn first

				// top right
				if (!diag_topright)
				{

					if (rival_owned[i].returnSprite().getGlobalBounds().contains(to_right + loc.x, to_up + loc.y))
					{
						if (rival_owned[i].returnPieceType() == ChessPiece::PieceType::BISHOP ||
							rival_owned[i].returnPieceType() == ChessPiece::PieceType::QUEEN)
						{
							check = true;
							break;
						}
						diag_topright = true;
					}
					else if (current_owned[i].returnSprite().getGlobalBounds().contains(to_right + loc.x, to_up + loc.y))
					{
						diag_topright = true;
					}
				}

				// top left
				if (!diag_topleft)
				{
					if (rival_owned[i].returnSprite().getGlobalBounds().contains(to_left + loc.x, to_up + loc.y))
					{
						if (rival_owned[i].returnPieceType() == ChessPiece::PieceType::BISHOP ||
							rival_owned[i].returnPieceType() == ChessPiece::PieceType::QUEEN)
						{
							check = true;
							break;
						}
						diag_topleft = true;
					}

					else if (current_owned[i].returnSprite().getGlobalBounds().contains(to_left + loc.x, to_up + loc.y))
					{
						diag_topleft = true;
					}
				}


				// bottom right
				if (!diag_botright)
				{
					if (rival_owned[i].returnSprite().getGlobalBounds().contains(to_right + loc.x, to_down + loc.y))
					{
						if (rival_owned[i].returnPieceType() == ChessPiece::PieceType::BISHOP ||
							rival_owned[i].returnPieceType() == ChessPiece::PieceType::QUEEN)
						{
							check = true;
							break;
						}
						diag_botright = true;
					}
					else if (current_owned[i].returnSprite().getGlobalBounds().contains(to_right + loc.x, to_down + loc.y))
					{
						diag_botright = true;
					}
				}


				// bottom left
				if (!diag_botleft)
				{
					if (rival_owned[i].returnSprite().getGlobalBounds().contains(to_left + loc.x, to_down + loc.y))
					{
						if (rival_owned[i].returnPieceType() == ChessPiece::PieceType::BISHOP ||
							rival_owned[i].returnPieceType() == ChessPiece::PieceType::QUEEN)
						{
							check = true;
							break;
						}
						diag_botleft = true;
					}
					else if (current_owned[i].returnSprite().getGlobalBounds().contains(to_left + loc.x, to_down + loc.y))
					{
						diag_botleft = true;
					}
				}
			}

			if (check)
			{
				break;
			}
		}

		// for pawn and knight
		for (ChessPiece& p : rival_owned)
		{
			// player side pawn check
			if (m_playerturn)
			{
				// top right and left
				if ((p.returnSprite().getGlobalBounds().contains(64 + loc.x, -64 + loc.y) &&
					p.returnPieceType() == ChessPiece::PieceType::PAWN) ||
					(p.returnSprite().getGlobalBounds().contains(-64 + loc.x, -64 + loc.y) &&
						p.returnPieceType() == ChessPiece::PieceType::PAWN))
				{
					check = true;
				}
			}
			// enemy side pawn check
			else
			{
				// bottom right and left
				if ((p.returnSprite().getGlobalBounds().contains(64 + loc.x, 64 + loc.y) &&
					p.returnPieceType() == ChessPiece::PieceType::PAWN) ||
					(p.returnSprite().getGlobalBounds().contains(-64 + loc.x, 64 + loc.y) &&
						p.returnPieceType() == ChessPiece::PieceType::PAWN))
				{
					check = true;
				}
			}

			// knight

			// horizontal L
			if ((p.returnSprite().getGlobalBounds().contains(64 + loc.x, 128 + loc.y) ||
				p.returnSprite().getGlobalBounds().contains(-64 + loc.x, 128 + loc.y) ||
				p.returnSprite().getGlobalBounds().contains(64 + loc.x, -128 + loc.y) ||
				p.returnSprite().getGlobalBounds().contains(-64 + loc.x, -128 + loc.y)) &&
				p.returnPieceType() == ChessPiece::PieceType::KNIGHT)
			{
				check = true;
			}

			// vertical L
			else if ((p.returnSprite().getGlobalBounds().contains(128 + loc.x, 64 + loc.y) ||
				p.returnSprite().getGlobalBounds().contains(-128 + loc.x, 64 + loc.y) ||
				p.returnSprite().getGlobalBounds().contains(128 + loc.x, -64 + loc.y) ||
				p.returnSprite().getGlobalBounds().contains(-128 + loc.x, -64 + loc.y)) &&
				p.returnPieceType() == ChessPiece::PieceType::KNIGHT)
			{
				check = true;
			}
		}
	}
}

// function to check any potential threats if move to x, y position (useful for king, and other piece that want to help blocking)
bool GameEvent::checkSeeker(int x, int y, std::vector<ChessPiece>& rival_owned, 
	std::vector<ChessPiece>& current_owned)
{
	bool up_check{ false }, down_check{ false }, left_check{ false },
		right_check{ false }, diag_topright{ false }, diag_topleft{ false },
		diag_botright{ false }, diag_botleft{ false };

	// checks here are executed once if a piece exist
	// if there is a piece not rook/queen in front of king, front check wont be done again
	// also check current side as well if blocked
	for (int i{ 0 }; i <= 512; i += 64)
	{
		int to_down{ 64 + i };
		int to_up{ -64 - i };
		int to_right{ 64 + i };
		int to_left{ -64 - i };

		for (std::size_t i{ 0 }; i < rival_owned.size(); i++)
		{
			// vertical

			// up
			if (!up_check)
			{
				if (rival_owned[i].returnSprite().getGlobalBounds().contains(0 + x, to_up + y))
				{
					if ((rival_owned[i].returnPieceType() == ChessPiece::PieceType::ROOK ||
						rival_owned[i].returnPieceType() == ChessPiece::PieceType::QUEEN))
					{
						return true;

					}
					up_check = true;
				}
				else if (current_owned[i].returnSprite().getGlobalBounds().contains(0 + x, to_up + y))
				{
					up_check = true;
				}
			}


			// down
			if (!down_check)
			{
				if (rival_owned[i].returnSprite().getGlobalBounds().contains(0 + x, to_down + y))
				{
					if ((rival_owned[i].returnPieceType() == ChessPiece::PieceType::ROOK ||
						rival_owned[i].returnPieceType() == ChessPiece::PieceType::QUEEN))
					{
						return true;
						
					}
					down_check = true;
				}
				else if (current_owned[i].returnSprite().getGlobalBounds().contains(0 + x, to_down + y))
				{
					down_check = true;
				}
			}

			// horizontal

			// left
			if (!left_check)
			{
				if (rival_owned[i].returnSprite().getGlobalBounds().contains(to_left + x, 0 + y))
				{
					if ((rival_owned[i].returnPieceType() == ChessPiece::PieceType::ROOK ||
						rival_owned[i].returnPieceType() == ChessPiece::PieceType::QUEEN))
					{
						return true;
						
					}
					left_check = true;
				}
				else if (current_owned[i].returnSprite().getGlobalBounds().contains(to_left + x, 0 + y))
				{
					left_check = true;
				}
			}

			// right
			if (!right_check)
			{
				if (rival_owned[i].returnSprite().getGlobalBounds().contains(to_right + x, 0 + y))
				{
					if ((rival_owned[i].returnPieceType() == ChessPiece::PieceType::ROOK ||
						rival_owned[i].returnPieceType() == ChessPiece::PieceType::QUEEN))
					{
						return true;
						
					}
					right_check = true;
				}
				else if (current_owned[i].returnSprite().getGlobalBounds().contains(to_right + x, 0 + y))
				{
					right_check = true;
				}
			}


			// diagonal
			// check for enemy pawn first

			// top right
			if (!diag_topright)
			{

				if (rival_owned[i].returnSprite().getGlobalBounds().contains(to_right + x, to_up + y))
				{
					if (rival_owned[i].returnPieceType() == ChessPiece::PieceType::BISHOP ||
						rival_owned[i].returnPieceType() == ChessPiece::PieceType::QUEEN)
					{
						return true;
						
					}
					diag_topright = true;
				}
				else if (current_owned[i].returnSprite().getGlobalBounds().contains(to_right + x, to_up + y))
				{
					diag_topright = true;
				}
			}

			// top left
			if (!diag_topleft)
			{
				if (rival_owned[i].returnSprite().getGlobalBounds().contains(to_left + x, to_up + y))
				{
					if (rival_owned[i].returnPieceType() == ChessPiece::PieceType::BISHOP ||
						rival_owned[i].returnPieceType() == ChessPiece::PieceType::QUEEN)
					{
						return true;
						
					}
					diag_topleft = true;
				}

				else if (current_owned[i].returnSprite().getGlobalBounds().contains(to_left + x, to_up + y))
				{
					diag_topleft = true;
				}
			}


			// bottom right
			if (!diag_botright)
			{
				if (rival_owned[i].returnSprite().getGlobalBounds().contains(to_right + x, to_down + y))
				{
					if (rival_owned[i].returnPieceType() == ChessPiece::PieceType::BISHOP ||
						rival_owned[i].returnPieceType() == ChessPiece::PieceType::QUEEN)
					{
						return true;
						
					}
					diag_botright = true;
				}
				else if (current_owned[i].returnSprite().getGlobalBounds().contains(to_right + x, to_down + y))
				{
					diag_botright = true;
				}
			}


			// bottom left
			if (!diag_botleft)
			{
				if (rival_owned[i].returnSprite().getGlobalBounds().contains(to_left + x, to_down + y))
				{
					if (rival_owned[i].returnPieceType() == ChessPiece::PieceType::BISHOP ||
						rival_owned[i].returnPieceType() == ChessPiece::PieceType::QUEEN)
					{
						return true;
						
					}
					diag_botleft = true;
				}
				else if (current_owned[i].returnSprite().getGlobalBounds().contains(to_left + x, to_down + y))
				{
					diag_botleft = true;
				}
			}
		}
	}

	// for pawn and knight
	for (ChessPiece& p : rival_owned)
	{
		// player side pawn check
		if (m_playerturn)
		{
			// top right and left
			if ((p.returnSprite().getGlobalBounds().contains(64 + x, -64 + y) &&
				p.returnPieceType() == ChessPiece::PieceType::PAWN) ||
				(p.returnSprite().getGlobalBounds().contains(-64 + x, -64 + y) &&
					p.returnPieceType() == ChessPiece::PieceType::PAWN))
			{
				return true;
			}
		}
		// enemy side pawn check
		else
		{
			// bottom right and left
			if ((p.returnSprite().getGlobalBounds().contains(64 + x, 64 + y) &&
				p.returnPieceType() == ChessPiece::PieceType::PAWN) ||
				(p.returnSprite().getGlobalBounds().contains(-64 + x, 64 + y) &&
					p.returnPieceType() == ChessPiece::PieceType::PAWN))
			{
				return true;
			}
		}

		// knight

		// horizontal L
		if ((p.returnSprite().getGlobalBounds().contains(64 + x, 128 + y) ||
			p.returnSprite().getGlobalBounds().contains(-64 + x, 128 + y) ||
			p.returnSprite().getGlobalBounds().contains(64 + x, -128 + y) ||
			p.returnSprite().getGlobalBounds().contains(-64 + x, -128 + y)) &&
			p.returnPieceType() == ChessPiece::PieceType::KNIGHT)
		{
			return true;
		}

		// vertical L
		else if ((p.returnSprite().getGlobalBounds().contains(128 + x, 64 + y) ||
			p.returnSprite().getGlobalBounds().contains(-128 + x, 64 + y) ||
			p.returnSprite().getGlobalBounds().contains(128 + x, -64 + y) ||
			p.returnSprite().getGlobalBounds().contains(-128 + x, -64 + y)) &&
			p.returnPieceType() == ChessPiece::PieceType::KNIGHT)
		{
			return true;
		}
	}

	return false;
}