#include "GameEvent.h"

GameEvent::GameEvent(Player& player, Board& board, Enemy& enemy, sf::RenderWindow& window)
	: m_player{ player }
	, m_board{ board }
	, m_enemy{ enemy }
	, m_window{ window }
{
}

void GameEvent::showBoard()
{
	std::size_t m_boardsize{ m_board.returnBoardSize() };
	sf::Sprite& m_boxW{ m_board.returnTileW()};
	sf::Sprite& m_boxB{ m_board.returnTileB() };
	// put it inside the vector
	std::size_t current_tile;
	bool white_turn{ true };

	float origin_x{};
	float origin_y{};
	for (std::size_t row{ 0 }; row < m_boardsize; row++)
	{
		if (row % 2)
		{
			white_turn = true;
		}
		else
		{
			white_turn = false;
		}
		for (std::size_t col{ 0 }; col < m_boardsize; col++)
		{
			current_tile = (row * m_boardsize) + col;

			if (white_turn)
			{
				board_vector[current_tile] = m_boxW;
				white_turn = false;
			}
			else
			{
				board_vector[current_tile] = m_boxB;
				white_turn = true;
			}

			// draw tile on the app
			board_vector[current_tile].setPosition(origin_x, origin_y);
			board_vector[current_tile].scale(0.5f, 0.5f);
			m_window.draw(board_vector[current_tile]);
			origin_x += 64;
		}

		// move to new row and reset column position
		origin_y += 64;
		origin_x = 0;
	}
}

// show the pieces on window
void GameEvent::showPieces()
{
	std::size_t index{};

	if (m_firsttime)
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
				m_window.draw(player_owned[index++]);
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
				m_window.draw(enemy_owned[index--]);
				origin_x -= 64;
			}
			origin_y += 64;
			origin_x = 480;
		}
		m_firsttime = false;
	}

	else
	{
		for (std::size_t i{ 0 }; i < player_owned.size(); i++)
		{
			m_window.draw(player_owned[i]);
			m_window.draw(enemy_owned[i]);
		}

	}

}

void GameEvent::choosePiece(int x, int y)
{
	std::vector<ChessPiece>& current_owned{ (m_playerturn) ? player_owned : enemy_owned };
	for (ChessPiece& piece : current_owned)
	{
		if (piece.returnSprite().getGlobalBounds().contains(x, y))
		{
			m_chosen = &piece;
			m_chosen->setActive(true);
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

// piece movements
void GameEvent::movePiece(int x, int y)
{
	if (m_chosen == nullptr)
	{
		return;
	}

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
	// or if promotion, halt the turn
	if (!isChosen() || m_promotion)
	{
		m_playerturn = m_playerturn;
	}
	else
	{
		m_playerturn = !m_playerturn;

		// reset the chosen sprite to be inactive
		m_chosen->setActive(false);
		m_chosen = nullptr;
	}
	

}

void GameEvent::movePawn(int x, int y, std::vector<ChessPiece>& current_owned, 
							std::vector<ChessPiece>& rival_owned, sf::Vector2f& loc)
{
	bool first_time{};
	bool not_blocked{};
	bool eat_enemy{};

	if (m_playerturn)
	{
		first_time = (loc.y == 416);
		if (x == 0)
		{
			if (y == -128 && first_time) // double move
			{
				not_blocked = !pieceBlocked(x, -64, current_owned, rival_owned); // to check one tile back
				not_blocked = !pieceBlocked(x, y, current_owned, rival_owned);
			}
			else if (y == -64)
			{
				not_blocked = !pieceBlocked(x, y, current_owned, rival_owned);
			}
		}
		else if (x == -64 || x == 64)
		{
			if (y == -64)
			{
				eat_enemy = eatEnemy(x, y, rival_owned);
				not_blocked = eat_enemy;
			}
		}
	}
	else
	{
		first_time = (loc.y == 96);
		if (x == 0)
		{
			if (y == 128 && first_time) // double move
			{
				not_blocked = !pieceBlocked(x, 64, current_owned, rival_owned); // to check one tile back
				not_blocked = !pieceBlocked(x, y, current_owned, rival_owned);
			}
			else if (y == 64)
			{
				not_blocked = !pieceBlocked(x, y, current_owned, rival_owned);
			}
		}
		else if (x == -64 || x == 64)
		{
			if (y == 64)
			{
				eat_enemy = eatEnemy(x, y, rival_owned);
				not_blocked = eat_enemy;
			}
		}
	}

	movingAction(x, y, not_blocked, rival_owned, eat_enemy);

	// if pawn reached end of board
	if ((m_playerturn && loc.y == 96) || 
		(!m_playerturn && loc.y == 416))
	{
		m_promotion = true;
	}
	
}

void GameEvent::moveKnight(int x, int y, std::vector<ChessPiece>& current_owned,
	std::vector<ChessPiece>& rival_owned, sf::Vector2f& loc)
{
	bool not_blocked{}, eat_enemy{};

	if (((y == 128 || y == -128) && (x == 64 || x == -64)) || // vertical L
		((y == 64 || y == -64) && (x == 128 || x == -128)))  // horizontal L
	{
		not_blocked = !pieceBlocked(x, y, current_owned);
	}

	movingAction(x, y, not_blocked, rival_owned);
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

	movingAction(x, y, not_blocked, rival_owned);
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

	movingAction(x, y, not_blocked, rival_owned);
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

	movingAction(x, y, not_blocked, rival_owned);
}

void GameEvent::moveKing(int x, int y, std::vector<ChessPiece>& current_owned,
	std::vector<ChessPiece>& rival_owned, sf::Vector2f& loc)
{
	static bool white_firsttime{ true };
	static bool black_firsttime{ true };

	bool not_blocked{ false };
	int to_down{ 64 };
	int to_up{ -64 };
	int to_right{ 64 };
	int to_left{ -64 };
	// castling check (white will always go first)

	if (m_playerturn)
	{
		
		if (loc.x != 288 || loc.y != 480)
		{
			white_firsttime = false;
		}

		if (white_firsttime)
		{
			if (y == 0)
			{
				// if castle left or right, not blocked, and has rook on that side, move
				// x + 64 if go right and x - 64 if left
				if (x == to_right + 64 && !onWayBlocked(x, y, to_right, y, current_owned, rival_owned) &&
					!checkSeeker(to_right + 64, y))
				{
					if (castlingMove(loc.x + x + 64, loc.y, current_owned, -128))
					{
						m_chosen->returnSprite().move(x, y);
						black_firsttime = false;
						return;
					}
				}
				else if (x == to_left - 64 && !onWayBlocked(x, y, to_left, y, current_owned, rival_owned) &&
					!checkSeeker(to_left - 64, y))
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
	else
	{
		if (loc.x != 288 || loc.y != 32)
		{
			black_firsttime = false;
		}

		if (black_firsttime)
		{
			if (y == 0)
			{
				// if castle left or right, not blocked, and has rook on that side, move
				// x + 64 if go right and x - 64 if left
				if (x == to_right + 64 && !onWayBlocked(x, y, to_right, y, current_owned, rival_owned) &&
					!checkSeeker(to_right + 64, y))
				{
					if (castlingMove(loc.x + x + 64, loc.y, current_owned, -128))
					{
						m_chosen->returnSprite().move(x, y);
						black_firsttime = false;
						return;
					}
				}
				else if (x == to_left - 64 && !onWayBlocked(x, y, to_left, y, current_owned, rival_owned) &&
					!checkSeeker(to_left - 64, y))
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
		if (x == to_right && !checkSeeker(x, y))
		{
			not_blocked = !onWayBlocked(x, y, to_right, to_down, current_owned, rival_owned);
		}
		else if (x == to_left && !checkSeeker(x, y))
		{
			not_blocked = !onWayBlocked(x, y, to_left, to_down, current_owned, rival_owned);
		}
		else if (x == 0 && !checkSeeker(x, y))
		{
			not_blocked = !onWayBlocked(x, y, x, to_down, current_owned, rival_owned);
		}
	}
	else if (y == to_up)
	{
		if (x == to_right && !checkSeeker(x, y))
		{
			not_blocked = !onWayBlocked(x, y, to_right, to_up, current_owned, rival_owned);

		}
		else if (x == to_left && !checkSeeker(x, y))
		{
			not_blocked = !onWayBlocked(x, y, to_left, to_up, current_owned, rival_owned);
		}
		else if (x == 0 && !checkSeeker(x, y))
		{
			not_blocked = !onWayBlocked(x, y, x, to_up, current_owned, rival_owned);
		}
	}
	else if (y == 0)
	{
		if (x == to_right && !checkSeeker(x, y))
		{
			not_blocked = !onWayBlocked(x, y, to_right, y, current_owned, rival_owned);
		}
		else if (x == to_left && !checkSeeker(x, y))
		{
			not_blocked = !onWayBlocked(x, y, to_left, y, current_owned, rival_owned);
		}
	}

	movingAction(x, y, not_blocked, rival_owned);
}

// for onWayBlocked and pawn
bool GameEvent::pieceBlocked(int x, int y, std::vector<ChessPiece>& current_owned, 
	std::vector<ChessPiece>& rival_owned)
{
	for (std::size_t i{ 0 }; i < current_owned.size(); i++)
	{
		if (current_owned[i].returnSprite().getGlobalBounds().
			contains(x + m_chosen->returnSprite().getPosition().x, y + m_chosen->returnSprite().getPosition().y) ||
			rival_owned[i].returnSprite().getGlobalBounds().
			contains(x + m_chosen->returnSprite().getPosition().x, y + m_chosen->returnSprite().getPosition().y))
		{
			return true;
		}
	}

	return false;
}

// for knight
bool GameEvent::pieceBlocked(int x, int y, std::vector<ChessPiece>& current_owned)
{
	for (ChessPiece& p : current_owned)
	{
		if (p.returnSprite().getGlobalBounds().
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
		/*
		check_blocked.emplace_back(pieceBlocked(dir_x, dir_y, current_owned));
		check_blocked.emplace_back(pieceBlocked(dir_x, dir_y, rival_owned));
		*/
		check_blocked.emplace_back(pieceBlocked(dir_x, dir_y, current_owned, rival_owned));
		dir_x += x_adder;
		dir_y += y_adder;
	}

	// in case player pointed to player piece because previous loop stops when dir == target
	// no need for enemy because will be checked on caller function (by eating?)
	check_blocked.emplace_back(pieceBlocked(dir_x, dir_y, current_owned, current_owned)); 
	
	bool way_blocked = std::any_of(check_blocked.begin(), check_blocked.end(),
		[](bool v) {return v; });

	return way_blocked;
}

// this function will move the chosen piece too
bool GameEvent::eatEnemy(int x, int y, std::vector<ChessPiece>& rival_owned)
{
	m_eaten = nullptr;
	for (std::size_t i{ 0 }; i < rival_owned.size(); i++)
	{
		if (rival_owned[i].returnSprite().getGlobalBounds().
			contains(x + m_chosen->returnSprite().getPosition().x, y + m_chosen->returnSprite().getPosition().y))
		{
			rival_owned[i].returnSprite().setPosition(600, 500);
			rival_owned[i].isMovable() = false;
			m_eaten = &rival_owned[i];
			m_chosen->returnSprite().move(x, y);
			return true;
		}
	}

	return false;
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

// function to check current king condition every turn
void GameEvent::checkSeeker()
{
	m_check = false;

	std::vector<ChessPiece>& current_owned{ (m_playerturn) ? player_owned : enemy_owned };
	std::vector<ChessPiece>& rival_owned{ (m_playerturn) ? enemy_owned : player_owned };

	sf::Vector2f loc{ findKingLoc() };

	// set this bools to true if checked that there is a piece or not
	bool up_check{ false }, down_check{ false }, left_check{ false },
		right_check{ false }, diag_topright{ false }, diag_topleft{ false },
		diag_botright{ false }, diag_botleft{ false }, horizontal_L{ false }, vertical_L{ false },
		pawn_check{ false };

	// checks here are executed once if a piece exist
	// if there is a piece not rook/queen in front of king, front check wont be done again
	// also check current side as well if blocked
	for (int i{ 0 }; i <= 512; i += 64)
	{
		int to_down{ 64 + i };
		int to_up{ -64 - i };
		int to_right{ 64 + i };
		int to_left{ -64 - i };

		for (std::size_t j{ 0 }; j < rival_owned.size(); j++)
		{
			// vertical

			// up for queen and rook check
			if (!up_check)
			{
				if (rival_owned[j].returnSprite().getGlobalBounds().contains(0 + loc.x, to_up + loc.y))
				{
					if ((rival_owned[j].returnPieceType() == ChessPiece::PieceType::ROOK ||
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::QUEEN))
					{
						m_check = true;
						break;
					}
					up_check = true;
					
				}
				else if (current_owned[j].returnSprite().getGlobalBounds().contains(0 + loc.x, to_up + loc.y))
				{
					up_check = true;
					
				}
			}


			// down for queen and rook check
			if (!down_check)
			{
				if (rival_owned[j].returnSprite().getGlobalBounds().contains(0 + loc.x, to_down + loc.y))
				{
					if ((rival_owned[j].returnPieceType() == ChessPiece::PieceType::ROOK ||
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::QUEEN))
					{
						m_check = true;
						break;
					}
					down_check = true;
					
				}
				else if (current_owned[j].returnSprite().getGlobalBounds().contains(0 + loc.x, to_down + loc.y))
				{
					down_check = true;
					
				}
			}

			// horizontal

			// left for queen and rook check
			if (!left_check)
			{
				if (rival_owned[j].returnSprite().getGlobalBounds().contains(to_left + loc.x, 0 + loc.y))
				{
					if ((rival_owned[j].returnPieceType() == ChessPiece::PieceType::ROOK ||
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::QUEEN))
					{
						m_check = true;
						break;
					}
					left_check = true;
					
				}
				else if (current_owned[j].returnSprite().getGlobalBounds().contains(to_left + loc.x, 0 + loc.y))
				{
					left_check = true;
					
				}
			}

			// right for queen and rook check
			if (!right_check)
			{
				if (rival_owned[j].returnSprite().getGlobalBounds().contains(to_right + loc.x, 0 + loc.y))
				{
					if ((rival_owned[j].returnPieceType() == ChessPiece::PieceType::ROOK ||
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::QUEEN))
					{
						m_check = true;
						break;
					}
					right_check = true;
					
				}
				else if (current_owned[j].returnSprite().getGlobalBounds().contains(to_right + loc.x, 0 + loc.y))
				{
					right_check = true;
					
				}
			}


			// diagonal

			// top right for bishop and queen check
			if (!diag_topright)
			{
				if (rival_owned[j].returnSprite().getGlobalBounds().contains(to_right + loc.x, to_up + loc.y))
				{
					if (rival_owned[j].returnPieceType() == ChessPiece::PieceType::BISHOP ||
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::QUEEN)
					{
						m_check = true;
						break;
					}
					diag_topright = true;
					
				}
				else if (current_owned[j].returnSprite().getGlobalBounds().contains(to_right + loc.x, to_up + loc.y))
				{
					diag_topright = true;
					
				}
			}

			// top left for bishop and queen check
			if (!diag_topleft)
			{
				if (rival_owned[j].returnSprite().getGlobalBounds().contains(to_left + loc.x, to_up + loc.y))
				{
					if (rival_owned[j].returnPieceType() == ChessPiece::PieceType::BISHOP ||
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::QUEEN)
					{
						m_check = true;
						break;
					}
					diag_topleft = true;
					
				}

				else if (current_owned[j].returnSprite().getGlobalBounds().contains(to_left + loc.x, to_up + loc.y))
				{
					diag_topleft = true;
					
				}
			}
			

			// bottom right for bishop and queen check
			if (!diag_botright)
			{
				if (rival_owned[j].returnSprite().getGlobalBounds().contains(to_right + loc.x, to_down + loc.y))
				{
					if (rival_owned[j].returnPieceType() == ChessPiece::PieceType::BISHOP ||
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::QUEEN)
					{
						m_check = true;
						break;
					}
					diag_botright = true;
					
				}
				else if (current_owned[j].returnSprite().getGlobalBounds().contains(to_right + loc.x, to_down + loc.y))
				{
					diag_botright = true;
					
				}
			}


			// bottom left for bishop and queen check
			if (!diag_botleft)
			{
				if (rival_owned[j].returnSprite().getGlobalBounds().contains(to_left + loc.x, to_down + loc.y))
				{
					if (rival_owned[j].returnPieceType() == ChessPiece::PieceType::BISHOP ||
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::QUEEN)
					{
						m_check = true;
						break;
					}
					diag_botleft = true;
					
				}
				else if (current_owned[j].returnSprite().getGlobalBounds().contains(to_left + loc.x, to_down + loc.y))
				{
					diag_botleft = true;
					
				}
			}

			// knight check
			if (!horizontal_L)
			{
				if (rival_owned[j].returnPieceType() == ChessPiece::PieceType::KNIGHT &&
					(rival_owned[j].returnSprite().getGlobalBounds().contains(64 + loc.x, 128 + loc.y) ||
						rival_owned[j].returnSprite().getGlobalBounds().contains(-64 + loc.x, 128 + loc.y) ||
						rival_owned[j].returnSprite().getGlobalBounds().contains(64 + loc.x, -128 + loc.y) ||
						rival_owned[j].returnSprite().getGlobalBounds().contains(-64 + loc.x, -128 + loc.y)))
				{
					m_check = true;
					horizontal_L = true;
					break;
				}
				
			}

			if (!vertical_L)
			{
				if ((rival_owned[j].returnPieceType() == ChessPiece::PieceType::KNIGHT) &&
					(rival_owned[j].returnSprite().getGlobalBounds().contains(128 + loc.x, 64 + loc.y) ||
						rival_owned[j].returnSprite().getGlobalBounds().contains(-128 + loc.x, 64 + loc.y) ||
						rival_owned[j].returnSprite().getGlobalBounds().contains(128 + loc.x, -64 + loc.y) ||
						rival_owned[j].returnSprite().getGlobalBounds().contains(-128 + loc.x, -64 + loc.y)))
				{
					m_check = true;
					vertical_L = true;
					break;
				}
			}

			// pawn check
			if (!pawn_check)
			{
				if (m_playerturn)
				{
					// top right and left
					if ((rival_owned[j].returnSprite().getGlobalBounds().contains(64 + loc.x, -64 + loc.y) &&
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::PAWN) ||
						(rival_owned[j].returnSprite().getGlobalBounds().contains(-64 + loc.x, -64 + loc.y) &&
							rival_owned[j].returnPieceType() == ChessPiece::PieceType::PAWN))
					{
						m_check = true;
						pawn_check = true;
						break;
					}
				}
					
				else
				{
					// bottom right and left
					if ((rival_owned[j].returnSprite().getGlobalBounds().contains(64 + loc.x, 64 + loc.y) &&
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::PAWN) ||
						(rival_owned[j].returnSprite().getGlobalBounds().contains(-64 + loc.x, 64 + loc.y) &&
							rival_owned[j].returnPieceType() == ChessPiece::PieceType::PAWN))
					{
						m_check = true;
						pawn_check = true;
						break;
					}
					
				}
			}
		}
	}
}

// function to check any potential threats if move to x, y position
// loc is used for king purposes only
bool GameEvent::checkSeeker(int x, int y, sf::Vector2f loc /*=m_chosen->returnSprite().getPosition();*/)
{
	
	std::vector<ChessPiece>& current_owned{ (m_playerturn) ? player_owned : enemy_owned };
	std::vector<ChessPiece>& rival_owned{ (m_playerturn) ? enemy_owned : player_owned };

	x += loc.x;
	y += loc.y;
	bool up_check{ false }, down_check{ false }, left_check{ false },
		right_check{ false }, diag_topright{ false }, diag_topleft{ false },
		diag_botright{ false }, diag_botleft{ false }, horizontal_L{ false }, vertical_L{ false },
		pawn_check{ false }, king_check{ false };

	// check blocking pieces in front, back, left, right, diagonal

	// checks here are executed once if a piece exist
	// if there is a piece not rook/queen in front of king, front check wont be done again
	// also check current side as well if blocked
	for (int i{ 0 }; i <= 512; i += 64)
	{
		int to_down{ 64 + i };
		int to_up{ -64 - i };
		int to_right{ 64 + i };
		int to_left{ -64 - i };

		for (std::size_t j{ 0 }; j < rival_owned.size(); j++)
		{

			// default movement if there is piece blocking

			if (current_owned[j].returnSprite().getGlobalBounds().contains(0 + x, 0 + y))
			{
				return true;
			}

			// king check
			if (!king_check)
			{
				if (rival_owned[j].returnPieceType() == ChessPiece::PieceType::KING)
				{
					if (rival_owned[j].returnSprite().
						getGlobalBounds().contains(0 + x, -64 + y))
					{
						return true;
					}
					else if (rival_owned[j].returnSprite().
						getGlobalBounds().contains(0 + x, 64 + y))
					{
						return true;
					}
					else if (rival_owned[j].returnSprite().
						getGlobalBounds().contains(64 + x, 0 + y))
					{
						return true;
					}
					else if (rival_owned[j].returnSprite().
						getGlobalBounds().contains(-64 + x, 0 + y))
					{
						return true;
					}
					else if (rival_owned[j].returnSprite().
						getGlobalBounds().contains(64 + x, 64 + y))
					{
						return true;
					}
					else if (rival_owned[j].returnSprite().
						getGlobalBounds().contains(64 + x, -64 + y))
					{
						return true;
					}
					else if (rival_owned[j].returnSprite().
						getGlobalBounds().contains(-64 + x, 64 + y))
					{
						return true;
					}
					else if (rival_owned[j].returnSprite().
						getGlobalBounds().contains(-64 + x, -64 + y))
					{
						return true;
					}
					else
					{
						king_check = true;
					}
				}
			}

			// vertical

			// up
			if (!up_check)
			{
				if (rival_owned[j].returnSprite().getGlobalBounds().contains(0 + x, to_up + y))
				{
					if ((rival_owned[j].returnPieceType() == ChessPiece::PieceType::ROOK ||
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::QUEEN))
					{
						return true;
						
					}
					up_check = true;
				}
				
				else if (current_owned[j].returnSprite().getGlobalBounds().contains(0 + x, to_up + y))
				{
					// since this only simulates and not actual movement, the chosen piece might get checked
					// check it if the sprite location is the same as chosen
					if (!(current_owned[j].returnSprite().getPosition() == loc))
					{
						up_check = true;
					}
				}
			}


			// down
			if (!down_check)
			{
				if (rival_owned[j].returnSprite().getGlobalBounds().contains(0 + x, to_down + y))
				{
					if ((rival_owned[j].returnPieceType() == ChessPiece::PieceType::ROOK ||
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::QUEEN))
					{
						return true;
						
					}
					down_check = true;
				}

				else if (current_owned[j].returnSprite().getGlobalBounds().contains(0 + x, to_down + y))
				{
					// since this only simulates and not actual movement, the chosen piece might get checked
					// check it if the sprite location is the same as chosen
					if (!(current_owned[j].returnSprite().getPosition() == loc))
					{
						down_check = true;
					}
				}
			}

			// horizontal

			// left
			if (!left_check)
			{
				if (rival_owned[j].returnSprite().getGlobalBounds().contains(to_left + x, 0 + y))
				{
					if ((rival_owned[j].returnPieceType() == ChessPiece::PieceType::ROOK ||
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::QUEEN))
					{
						return true;
						
					}
					left_check = true;
				}
				else if (current_owned[j].returnSprite().getGlobalBounds().contains(to_left + x, 0 + y))
				{
					// since this only simulates and not actual movement, the chosen piece might get checked
					// check it if the sprite location is the same as chosen
					if (!(current_owned[j].returnSprite().getPosition() == loc))
					{
						left_check = true;
					}
				}
			}

			// right
			if (!right_check)
			{
				if (rival_owned[j].returnSprite().getGlobalBounds().contains(to_right + x, 0 + y))
				{
					if ((rival_owned[j].returnPieceType() == ChessPiece::PieceType::ROOK ||
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::QUEEN))
					{
						return true;
					}
					right_check = true;
				}
				else if (current_owned[j].returnSprite().getGlobalBounds().contains(to_right + x, 0 + y))
				{
					// since this only simulates and not actual movement, the chosen piece might get checked
					// check it if the sprite location is the same as chosen
					if (!(current_owned[j].returnSprite().getPosition() == loc))
					{
						right_check = true;
					}
				}
			}


			// diagonal
			// check for enemy pawn first

			// top right
			if (!diag_topright)
			{

				if (rival_owned[j].returnSprite().getGlobalBounds().contains(to_right + x, to_up + y))
				{
					if (rival_owned[j].returnPieceType() == ChessPiece::PieceType::BISHOP ||
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::QUEEN)
					{
						return true;
						
					}
					diag_topright = true;
				}
				else if (current_owned[j].returnSprite().getGlobalBounds().contains(to_right + x, to_up + y))
				{
					// since this only simulates and not actual movement, the chosen piece might get checked
					// check it if the sprite location is the same as chosen
					if (!(current_owned[j].returnSprite().getPosition() == loc))
					{
						diag_topright = true;
					}
				}
			}

			// top left
			if (!diag_topleft)
			{
				if (rival_owned[j].returnSprite().getGlobalBounds().contains(to_left + x, to_up + y))
				{
					if (rival_owned[j].returnPieceType() == ChessPiece::PieceType::BISHOP ||
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::QUEEN)
					{
						return true;
						
					}
					diag_topleft = true;
				}

				else if (current_owned[j].returnSprite().getGlobalBounds().contains(to_left + x, to_up + y))
				{
					// since this only simulates and not actual movement, the chosen piece might get checked
					// check it if the sprite location is the same as chosen
					if (!(current_owned[j].returnSprite().getPosition() == loc))
					{
						diag_topleft = true;
					}
				}
			}


			// bottom right
			if (!diag_botright)
			{
				if (rival_owned[j].returnSprite().getGlobalBounds().contains(to_right + x, to_down + y))
				{
					if (rival_owned[j].returnPieceType() == ChessPiece::PieceType::BISHOP ||
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::QUEEN)
					{
						return true;
						
					}
					diag_botright = true;
				}
				else if (current_owned[j].returnSprite().getGlobalBounds().contains(to_right + x, to_down + y))
				{
					// since this only simulates and not actual movement, the chosen piece might get checked
					// check it if the sprite location is the same as chosen
					if (!(current_owned[j].returnSprite().getPosition() == loc))
					{
						diag_botright = true;
					}
				}
			}


			// bottom left
			if (!diag_botleft)
			{
				if (rival_owned[j].returnSprite().getGlobalBounds().contains(to_left + x, to_down + y))
				{
					if (rival_owned[j].returnPieceType() == ChessPiece::PieceType::BISHOP ||
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::QUEEN)
					{
						return true;
						
					}
					diag_botleft = true;
				}
				else if (current_owned[j].returnSprite().getGlobalBounds().contains(to_left + x, to_down + y))
				{
					// since this only simulates and not actual movement, the chosen piece might get checked
					// check it if the sprite location is the same as chosen
					if (!(current_owned[j].returnSprite().getPosition() == loc))
					{
						diag_botleft = true;
					}
				}
			}

			// knight check
			if (!horizontal_L)
			{
				if (rival_owned[j].returnPieceType() == ChessPiece::PieceType::KNIGHT &&
					(rival_owned[j].returnSprite().getGlobalBounds().contains(64 + x, 128 + y) ||
						rival_owned[j].returnSprite().getGlobalBounds().contains(-64 + x, 128 + y) ||
						rival_owned[j].returnSprite().getGlobalBounds().contains(64 + x, -128 + y) ||
						rival_owned[j].returnSprite().getGlobalBounds().contains(-64 + x, -128 + y)))
				{
					return true;
				}

			}

			if (!vertical_L)
			{
				if ((rival_owned[j].returnPieceType() == ChessPiece::PieceType::KNIGHT) &&
					(rival_owned[j].returnSprite().getGlobalBounds().contains(128 + x, 64 + y) ||
						rival_owned[j].returnSprite().getGlobalBounds().contains(-128 + x, 64 + y) ||
						rival_owned[j].returnSprite().getGlobalBounds().contains(128 + x, -64 + y) ||
						rival_owned[j].returnSprite().getGlobalBounds().contains(-128 + x, -64 + y)))
				{
					return true;
				}
			}

			// pawn check
			if (!pawn_check)
			{
				if (m_playerturn)
				{
					// top right and left
					if ((rival_owned[j].returnSprite().getGlobalBounds().contains(64 + x, -64 + y) &&
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::PAWN) ||
						(rival_owned[j].returnSprite().getGlobalBounds().contains(-64 + x, -64 + y) &&
							rival_owned[j].returnPieceType() == ChessPiece::PieceType::PAWN))
					{
						return true;
					}
				}

				else
				{
					// bottom right and left
					if ((rival_owned[j].returnSprite().getGlobalBounds().contains(64 + x, 64 + y) &&
						rival_owned[j].returnPieceType() == ChessPiece::PieceType::PAWN) ||
						(rival_owned[j].returnSprite().getGlobalBounds().contains(-64 + x, 64 + y) &&
							rival_owned[j].returnPieceType() == ChessPiece::PieceType::PAWN))
					{
						return true;
					}

				}
			}
		}
	}

	return false;
}

sf::Vector2f GameEvent::findKingLoc()
{
	std::vector<ChessPiece>& current_owned{ (m_playerturn) ? player_owned : enemy_owned };

	for (ChessPiece& k : current_owned)
	{
		if (k.returnPieceType() == ChessPiece::PieceType::KING)
		{
			return k.returnSprite().getPosition();
		}
	}
}

// leave eat_enemy empty if variable doesnt exist inside caller
// if check and move not helping king, m_chosen = nullptr
void GameEvent::movingAction(int x, int y, bool not_blocked, std::vector<ChessPiece>& rival_owned,
	bool eat_enemy /*= false*/)
{
	bool is_king{ m_chosen->returnPieceType() == ChessPiece::PieceType::KING };

	if (not_blocked) // movements allowed
	{
		if (m_check && !is_king)
		{
			// use eating enemy first as condition to prevent enemy eating double
			// if help king by eating
			if (eat_enemy) // pawn only
			{
				checkSeeker(); // check current king condition

				if (m_check)
				{
					m_chosen->returnSprite().move(-x, -y); // revert if king is still in check
					m_chosen->setActive(false);
					m_chosen = nullptr;
				}
			}
			else if (eatEnemy(x, y, rival_owned)) // other pieces eat
			{
				checkSeeker(); // check current king condition

				if (m_check)
				{
					m_chosen->returnSprite().move(-x, -y); // revert if king is still in check

					// enemy recovery
					sf::Vector2f loc{ m_chosen->returnSprite().getPosition() };
					m_eaten->returnSprite().setPosition(x + loc.x, y + loc.y);
					m_eaten->isMovable() = true;

					m_eaten = nullptr;

					m_chosen->setActive(false);
					m_chosen = nullptr;
				}
			}
			else if (checkSeeker(x, y)) // check if piece helps king by blocking threat
			{
				m_chosen->returnSprite().move(x, y);

				checkSeeker(); // check current king condition

				if (m_check)
				{
					m_chosen->returnSprite().move(-x, -y); // revert if king is still in check
					m_chosen->setActive(false);
					m_chosen = nullptr;
				}
			}
			else
			{
				m_chosen->setActive(false);
				m_chosen = nullptr;
			}
		}
		else
		{
			// put eat_enemy here so it wont be nullptr and stayed in same turn
			if (!eat_enemy && !eatEnemy(x, y, rival_owned)) 
			{
				m_chosen->returnSprite().move(x, y);

				if (is_king) // king can move so not check
				{
					m_check = false;
				}

				checkSeeker(); // check current king condition

				if (m_check)
				{
					m_chosen->returnSprite().move(-x, -y);
					m_chosen->setActive(false);
					m_chosen = nullptr;
				}
			}
		}
	}
	else
	{
		if (m_chosen != nullptr)
		{
			m_chosen->setActive(false);
			m_chosen = nullptr;
		}
		
	}
	
}

void GameEvent::promotionCheck()
{
	sf::Texture t;
	sf::Sprite text;

	t.loadFromFile("chessimages/promotion.png");

	text.setTexture(t);
	text.setPosition(-95, 150);
	m_window.draw(text);
	ChessPiece rook, bishop, queen, knight, pawn;
	ChessPiece::ColorType color{};


	if (m_playerturn)
	{
		color = ChessPiece::ColorType::WHITE;
	}
	else
	{
		color = ChessPiece::ColorType::BLACK;
	}

	rook.updateSprite(color, ChessPiece::PieceType::ROOK);
	bishop.updateSprite(color, ChessPiece::PieceType::BISHOP);
	queen.updateSprite(color, ChessPiece::PieceType::QUEEN);
	knight.updateSprite(color, ChessPiece::PieceType::KNIGHT);
	pawn.updateSprite(color, ChessPiece::PieceType::PAWN);

	m_choices = { rook, bishop, queen, knight, pawn };

	sf::Texture box;
	sf::Sprite promobox;

	box.loadFromFile("chessimages/promobox.png");
	promobox.setTexture(box);
	promobox.setOrigin(promobox.getLocalBounds().width / 2, promobox.getLocalBounds().height / 2);

	int pos_x{ 60 };
	int pos_y{ 275 };
	for (ChessPiece& s : m_choices)
	{
		promobox.setPosition(pos_x, pos_y);
		s.returnSprite().setPosition(pos_x, pos_y);
		m_window.draw(promobox);
		m_window.draw(s);
		pos_x += 96;
	}

}

bool GameEvent::pawnPromotion()
{
	return m_promotion;
}

void GameEvent::choosePromotion(int x, int y)
{
	for (ChessPiece& s : m_choices)
	{
		if (s.returnSprite().getGlobalBounds().contains(x, y))
		{
			m_chosen->updateSprite(s.returnColorType(), s.returnPieceType());
			// because updateSprite scales the image even smaller, manually rescale back
			m_chosen->resetSprite();
			m_promotion = false;

			// change turn and reset chosen
			m_playerturn = !m_playerturn;
			m_chosen->setActive(false);
			m_chosen = nullptr;
			break;
		}
	}
}

// if check, perform king checkseeker function to see if theres any tile king can be moved
// use loop for x and y
void GameEvent::staleCheck()
{
	sf::Vector2f loc{ findKingLoc() };

	bool first_time{};
	bool stale{};

	int max_top{ 32 };
	int max_bottom{ 480 };
	int max_right{ 480 };
	int max_left{ 32 };

	// check if king on top or bottom on board or even on end of right or left side
	// if king on bottom of board, dont checkseeker on y = 64
	if (loc.y == max_bottom) 
	{
		if (loc.x == max_right)
		{
			stale = (checkSeeker(0, -64, loc) && checkSeeker(-64, 0, loc) &&
				checkSeeker(-64, -64, loc));
		}
		else if (loc.x == max_left)
		{
			stale = (checkSeeker(0, -64, loc) && checkSeeker(64, 0, loc) &&
				checkSeeker(64, -64, loc));
		}
		else
		{
			stale = (checkSeeker(0, -64, loc) && checkSeeker(64, 0, loc) &&
				checkSeeker(-64, 0, loc) && checkSeeker(64, -64, loc) &&
				checkSeeker(-64, -64, loc));
		}
	}
	// if king on top of board, dont checkseeker on y = -64
	else if (loc.y == max_top) 
	{
		if (loc.x == max_right)
		{
			stale = (checkSeeker(0, 64, loc) && checkSeeker(-64, 0, loc) && 
				checkSeeker(-64, 64, loc));
		}
		else if (loc.x == max_left)
		{
			stale = (checkSeeker(0, 64, loc) && checkSeeker(64, 0, loc) && 
				checkSeeker(64, 64, loc));
		}
		else
		{
			stale = (checkSeeker(0, 64, loc) && checkSeeker(64, 0, loc) && 
				checkSeeker(-64, 0, loc) && checkSeeker(64, 64, loc) && 
				checkSeeker(-64, 64, loc));
		}
	}
	// if king on middle of board
	else
	{
		if (loc.x == max_right)
		{
			stale = (checkSeeker(0, -64, loc) && checkSeeker(0, 64, loc) &&
				checkSeeker(-64, 0, loc) && checkSeeker(-64, -64, loc) &&
				checkSeeker(-64, 64, loc));
		}
		else if (loc.x == max_left)
		{
			stale = (checkSeeker(0, -64, loc) && checkSeeker(0, 64, loc) &&
				checkSeeker(64, 0, loc) && checkSeeker(64, -64, loc) && 
				checkSeeker(64, 64, loc));
		}
		else
		{
			stale = (checkSeeker(0, -64, loc) && checkSeeker(0, 64, loc) &&
				checkSeeker(64, 0, loc) && checkSeeker(-64, 0, loc) &&
				checkSeeker(64, -64, loc) && checkSeeker(-64, -64, loc) &&
				checkSeeker(64, 64, loc) && checkSeeker(-64, 64, loc));
		}
	}

	m_stale = stale;
}

const bool GameEvent::isCheck()
{
	return m_check;
}

// this function makes m_chosen null
bool GameEvent::findHelper()
{
	std::vector<ChessPiece>& current_owned{ (m_playerturn) ? player_owned : enemy_owned };
	std::vector<ChessPiece>& rival_owned{ (m_playerturn) ? enemy_owned : player_owned };

	sf::Vector2f loc;
	int x{}, y{}; // to reverse the movement at end of function
	for (ChessPiece& p : current_owned)
	{
		m_chosen = &p;
		loc = m_chosen->returnSprite().getPosition();

		if (!m_chosen->isMovable())
		{
			continue;
		}

		switch (m_chosen->returnPieceType())
		{
		case ChessPiece::PieceType::PAWN:
		{
			int onetile{ 64 };
			int twotile{ 128 };

			if (m_playerturn)
			{
				onetile = -onetile;
				twotile = -twotile;
			}

			movePawn(0, onetile, current_owned, rival_owned, loc); 
			x = 0, y = onetile;
			if (m_chosen == nullptr)
			{
				m_chosen = &p;
				movePawn(0, twotile, current_owned, rival_owned, loc);
				x = 0, y = twotile;
			}
			break;
		}
		case ChessPiece::PieceType::KNIGHT:
		{
			bool L1, L2, L3, L4, L5, L6, L7, L8;
			L1 = L2 = L3 = L4 = L5 = L6 = L7 = L8 = true;
			for (int i{ 0 }; i < 8; i++)
			{
				m_chosen = &p;
				if (L1)
				{
					moveKnight(64, 128, current_owned, rival_owned, loc);
					L1 = (m_chosen != nullptr);
					x = 64, y = 128;
				}
				else if (L2)
				{
					moveKnight(64, -128, current_owned, rival_owned, loc);
					L2 = (m_chosen != nullptr);
					x = 64, y = -128;
				}
				else if (L3)
				{
					moveKnight(-64, 128, current_owned, rival_owned, loc);
					L3 = (m_chosen != nullptr);
					x = -64, y = 128;
				}
				else if (L4)
				{
					moveKnight(-64, -128, current_owned, rival_owned, loc);
					L4 = (m_chosen != nullptr);
					x = -64, y = -128;
				}
				else if (L5)
				{
					moveKnight(128, 64, current_owned, rival_owned, loc);
					L5 = (m_chosen != nullptr);
					x = 128, y = 64;
				}
				else if (L6)
				{
					moveKnight(128, -64, current_owned, rival_owned, loc);
					L6 = (m_chosen != nullptr);
					x = 128, y = -64;
				}
				else if (L7)
				{
					moveKnight(-128, 64, current_owned, rival_owned, loc);
					L7 = (m_chosen != nullptr);
					x = -128, y = 64;
				}
				else if (L8)
				{
					moveKnight(-128, -64, current_owned, rival_owned, loc);
					L8 = (m_chosen != nullptr);
					x = -128, y = -64;
				}

				if (!m_check)
				{
					break;
				}
			}

			break;
		}

		case ChessPiece::PieceType::BISHOP:
		{
			for (int i{ 0 }; i <= 512; i += 64)
			{
				int to_down{ 64 + i };
				int to_up{ -64 - i };
				int to_right{ 64 + i };
				int to_left{ -64 - i };
				bool diag_topright, diag_topleft, diag_botright, diag_botleft;
				diag_topright = diag_topleft = diag_botright = diag_botleft = true;
				m_chosen = &p;

				// make sure to move to every possible way before next iteration
				for (int j{ 0 }; j < 4; j++)
				{
					m_chosen = &p;

					if (diag_topright)
					{
						moveBishop(to_right, to_up, current_owned, rival_owned, loc);
						diag_topright = (m_chosen != nullptr);
						x = to_right, y = to_up;
					}
					else if (diag_topleft)
					{
						moveBishop(to_left, to_up, current_owned, rival_owned, loc);
						diag_topleft = (m_chosen != nullptr);
						x = to_left, y = to_up;
					}
					else if (diag_botleft)
					{
						moveBishop(to_left, to_down, current_owned, rival_owned, loc);
						diag_botleft = (m_chosen != nullptr);
						x = to_left, y = to_down;
					}
					else if (diag_botright)
					{
						moveBishop(to_right, to_down, current_owned, rival_owned, loc);
						diag_botright = (m_chosen != nullptr);
					}

					if (!m_check)
					{
						break;
					}
				}

				if (!m_check)
				{
					break;
				}
			}
			break;
		}

		case ChessPiece::PieceType::QUEEN:
		{
			for (int i{ 0 }; i <= 512; i += 64)
			{
				int to_down{ 64 + i };
				int to_up{ -64 - i };
				int to_right{ 64 + i };
				int to_left{ -64 - i };
				bool up, down, left, right, diag_topright,
					diag_topleft, diag_botright, diag_botleft;
				up = down = left = right =
					diag_topright = diag_topleft =
					diag_botright = diag_botleft = true;
				m_chosen = &p;

				// make sure to move to every possible way before next iteration
				for (int j{ 0 }; j < 8; j++)
				{
					m_chosen = &p;

					if (up)
					{
						moveQueen(0, to_up, current_owned, rival_owned, loc);
						up = (m_chosen != nullptr);
						x = 0, y = to_up;
					}
					else if (down)
					{
						moveQueen(0, to_down, current_owned, rival_owned, loc);
						down = (m_chosen != nullptr);
						x = 0, y = to_down;
					}
					else if (left)
					{
						moveQueen(to_left, 0, current_owned, rival_owned, loc);
						left = (m_chosen != nullptr);
						x = to_left, y = 0;
					}
					else if (right)
					{
						moveQueen(to_right, 0, current_owned, rival_owned, loc);
						right = (m_chosen != nullptr);
						x = to_right, y = 0;
					}
					else if (diag_topright)
					{
						moveQueen(to_right, to_up, current_owned, rival_owned, loc);
						diag_topright = (m_chosen != nullptr);
						x = to_right, y = to_up;
					}
					else if (diag_topleft)
					{
						moveQueen(to_left, to_up, current_owned, rival_owned, loc);
						diag_topleft = (m_chosen != nullptr);
						x = to_left, y = to_up;
					}
					else if (diag_botleft)
					{
						moveQueen(to_left, to_down, current_owned, rival_owned, loc);
						diag_botleft = (m_chosen != nullptr);
						x = to_left, y = to_down;
					}
					else if (diag_botright)
					{
						moveQueen(to_right, to_down, current_owned, rival_owned, loc);
						diag_botright = (m_chosen != nullptr);
						x = to_right, y = to_down;
					}

					if (!m_check)
					{
						break;
					}
				}

				if (!m_check)
				{
					break;
				}
			}
			break;
		}
		case ChessPiece::PieceType::ROOK:
		{
			for (int i{ 0 }; i <= 512; i += 64)
			{
				int to_down{ 64 + i };
				int to_up{ -64 - i };
				int to_right{ 64 + i };
				int to_left{ -64 - i };
				bool up, down, left, right;
				up = down = left = right = true;
				m_chosen = &p;

				// make sure to move to every possible way before next iteration
				for (int j{ 0 }; j < 4; j++)
				{
					m_chosen = &p;

					if (up)
					{
						moveRook(0, to_up, current_owned, rival_owned, loc);
						up = (m_chosen != nullptr);
						x = 0, y = to_up;
					}
					else if (down)
					{
						moveRook(0, to_down, current_owned, rival_owned, loc);
						down = (m_chosen != nullptr);
						x = 0, y = to_down;
					}
					else if (left)
					{
						moveRook(to_left, 0, current_owned, rival_owned, loc);
						left = (m_chosen != nullptr);
						x = to_left, y = 0;
					}
					else if (right)
					{
						moveRook(to_right, 0, current_owned, rival_owned, loc);
						right = (m_chosen != nullptr);
						x = to_right, y = 0;
					}

					if (!m_check)
					{
						break;
					}
				}

				if (!m_check)
				{
					break;
				}
			}
			break;
		}
		}

		if (!m_check)
		{
			break;
		}
	}

	if (m_chosen == nullptr)
	{
		return false;
	}
	else
	{
		// in case if king isnt checked, but stale
		if (m_chosen != nullptr)
		{
			m_chosen->returnSprite().move(-x, -y);
			m_chosen->setActive(false);
			m_chosen = nullptr;
		}
		
		return true;
	}
}

void GameEvent::checkMate()
{
	m_checkmate = true;

	// because it will always change turn after movement, reverse the condition
	if (m_playerturn)
	{
		std::cout << "enemy win\n";
	}
	else
	{
		std::cout << "player win\n";
	}
}

bool GameEvent::oneKing()
{
	std::vector<ChessPiece>& current_owned{ (m_playerturn) ? player_owned : enemy_owned };
	
	int piece_count{};

	for (ChessPiece& k : current_owned)
	{
		if (k.isMovable())
		{
			piece_count++;
		}
	}

	if (piece_count == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

const bool GameEvent::isStale()
{
	return m_stale;
}

const bool GameEvent::isCheckmate()
{
	return m_checkmate;
}

void GameEvent::showCheckmate()
{
	sf::Texture t;
	sf::Sprite s;

	if (!t.loadFromFile("chessimages/CHECKMATE.png"))
	{
		std::cout << "failed load checkmate";
	}

	s.setTexture(t);

	s.setPosition(-85, 215);

	m_window.draw(s);
}