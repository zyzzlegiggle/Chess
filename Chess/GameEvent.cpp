#include "GameEvent.h"

GameEvent::GameEvent(Player& player, Board& board, Enemy& enemy, sf::RenderWindow& window)
	: m_player{ player }
	, m_board{ board }
	, m_enemy{ enemy }
	, m_window{ window }
{
	if (!moving_buffer.loadFromFile("sound/moving.mp3"))
	{
		std::cout << "moving";
	}

	moving.setBuffer(moving_buffer);
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

	// for showing the board the first time
	static bool firsttime{ true }; 

	if (firsttime)
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
		firsttime = false;
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

// if movement not succeeded, stay at current turn
// else change turns, m_chosen became nullptr
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

	sf::Vector2f loc{ m_chosen->getPosition() };

	
	if (!((x % 64 == 0) && (y % 64 == 0)))
	{
		// this one for player click the tile
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
	}

	// boundaries check
	if (outOfBoundaries(x, y, loc))
	{
		m_chosen = nullptr;
		return;
	}
	
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
		moving.play();
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
				if (not_blocked)
				{
					not_blocked = !pieceBlocked(x, y, current_owned, rival_owned);
				}
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
				if (not_blocked)
				{
					not_blocked = !pieceBlocked(x, y, current_owned, rival_owned);
				}
				
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

	// if pawn reached end of board (assuming it actually moved
	if (m_chosen != nullptr)
	{
		// put 96 and 416 instead of 32 and 480 because the time it was moved, the updated position is not drawn yet
		if ((m_playerturn && loc.y == 96) ||
			(!m_playerturn && loc.y == 416))
		{
			m_promotion = true;
		}
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
					!checkSeeker(to_left - 64, y) && !checkSeeker(to_left - 128, y))
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
					!checkSeeker(to_left - 64, y) && !checkSeeker(to_left - 128, y))
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
			contains(x + m_chosen->getPosition().x, y + m_chosen->getPosition().y) ||
			rival_owned[i].returnSprite().getGlobalBounds().
			contains(x + m_chosen->getPosition().x, y + m_chosen->getPosition().y))
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
			contains(x + m_chosen->getPosition().x, y + m_chosen->getPosition().y))
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
			contains(x + m_chosen->getPosition().x, y + m_chosen->getPosition().y))
		{
			rival_owned[i].returnSprite().setPosition(1500, 1500);
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
		if (p.getPosition().x == rook_x &&
			p.getPosition().y == rook_y &&
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
bool GameEvent::checkSeeker(int x, int y, sf::Vector2f loc /*=m_chosen->getPosition();*/)
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

			// calcPoints use x and y as 0 and other function is using the if condition inside
			if (!(x == loc.x && y == loc.y))
			{
				// default movement if there is piece blocking
				if (current_owned[j].returnSprite().getGlobalBounds().contains(0 + x, 0 + y))
				{
					return true;
				}
			}
			

			// king check
			if (!king_check)
			{
				if (rival_owned[j].returnPieceType() == ChessPiece::PieceType::KING)
				{
					const int offsets_x[] = { 0, 64, -64 };
					const int offsets_y[] = { 0,64, -64 };

					for (int off_x : offsets_x)
					{
						for (int off_y : offsets_y)
						{
							if (rival_owned[j].returnSprite().
								getGlobalBounds().contains(off_x + x, off_y + y))
							{
								return true;
							}
						}
					}
					king_check = true;
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
					if (!(current_owned[j].getPosition() == loc))
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
					if (!(current_owned[j].getPosition() == loc))
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
					if (!(current_owned[j].getPosition() == loc))
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
					if (!(current_owned[j].getPosition() == loc))
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
					if (!(current_owned[j].getPosition() == loc))
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
					if (!(current_owned[j].getPosition() == loc))
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
					if (!(current_owned[j].getPosition() == loc))
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
					if (!(current_owned[j].getPosition() == loc))
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
			return k.getPosition();
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

					// enemy recovery
					sf::Vector2f loc{ m_chosen->getPosition() };
					m_eaten->returnSprite().setPosition(x + loc.x, y + loc.y);
					m_eaten->isMovable() = true;

					m_eaten = nullptr;

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
					sf::Vector2f loc{ m_chosen->getPosition() };
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

				checkSeeker(); // check current king condition
				
				// revert if movement causes check
				if (m_check)
				{
					m_chosen->returnSprite().move(-x, -y);
					m_chosen->setActive(false);
					m_chosen = nullptr;
				}
			}
			
			checkSeeker(); // in case king successfully eat while forced
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
	ChessPiece rook, bishop, queen, knight;
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

	m_choices = { rook, bishop, queen, knight};

	sf::Texture box;
	sf::Sprite promobox;

	box.loadFromFile("chessimages/promobox.png");
	promobox.setTexture(box);
	promobox.setOrigin(promobox.getLocalBounds().width / 2, promobox.getLocalBounds().height / 2);

	int pos_x{ 110 };
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
		loc = m_chosen->getPosition();

		if (!m_chosen->isMovable())
		{
			m_chosen = nullptr;
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

			movePawn(0, twotile, current_owned, rival_owned, loc); 
			x = 0, y = twotile;
			if (m_chosen == nullptr)
			{
				m_chosen = &p;
				movePawn(0, onetile, current_owned, rival_owned, loc);
				x = 0, y = onetile;
			}

			if (m_chosen == nullptr)
			{
				continue;
			}
			break;
		}
		case ChessPiece::PieceType::KNIGHT:
		{
			std::vector<std::pair<int, int>> knight_moves = {
				{64, 128}, {64, -128}, {-64, 128}, {-64, -128},
				{128, 64}, {128, -64}, {-128, 64}, {-128, -64}
			};

			std::vector<bool> valid_moves(8, true);
			for (std::size_t i{ 0 }; i < valid_moves.size(); i++)
			{
				m_chosen = &p;
				if (valid_moves[i])
				{
					int offset_x{ knight_moves[i].first };
					int offset_y{ knight_moves[i].second };

					moveKnight(offset_x, offset_y, current_owned, rival_owned, loc);
					valid_moves[i] = (m_chosen != nullptr);
					x = offset_x;
					y = offset_y;
					if (!m_check)
					{
						break;
					}
				}	
			}

			break;
		}

		case ChessPiece::PieceType::BISHOP:
		{
			for (int i{ 0 }; i < 480; i += 64)
			{
				int to_down{ 64 + i };
				int to_up{ -64 - i };
				int to_right{ 64 + i };
				int to_left{ -64 - i };
				std::vector<std::pair<int, int>> moves = {
				{to_right, to_up}, {to_left, to_up}, {to_right, to_down}, {to_left, to_down}
				};

				std::vector<bool> valid_moves(4, true);

				for (std::size_t i{ 0 }; i < valid_moves.size(); i++)
				{
					m_chosen = &p;
					if (valid_moves[i])
					{
						int offset_x{ moves[i].first };
						int offset_y{ moves[i].second };

						moveBishop(offset_x, offset_y, current_owned, rival_owned, loc);
						valid_moves[i] = (m_chosen != nullptr);
						x = offset_x, y = offset_y;
						if (!m_check)
						{
							break;
						}
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
			for (int i{ 0 }; i < 480; i += 64)
			{
				int to_down{ 64 + i };
				int to_up{ -64 - i };
				int to_right{ 64 + i };
				int to_left{ -64 - i };

				std::vector<std::pair<int, int>> moves = {
				{to_right, to_up}, {to_left, to_up}, {to_right, to_down}, {to_left, to_down},
					{0, to_up}, {0, to_down}, {to_left, 0}, {to_right, 0}
				};

				std::vector<bool> valid_moves(8, true);

				for (std::size_t i{ 0 }; i < valid_moves.size(); i++)
				{
					m_chosen = &p;
					if (valid_moves[i])
					{
						int offset_x{ moves[i].first };
						int offset_y{ moves[i].second };

						moveQueen(offset_x, offset_y, current_owned, rival_owned, loc);
						valid_moves[i] = (m_chosen != nullptr);
						x = offset_x, y = offset_y;
						if (!m_check)
						{
							break;
						}
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
			for (int i{ 0 }; i < 480; i += 64)
			{
				int to_down{ 64 + i };
				int to_up{ -64 - i };
				int to_right{ 64 + i };
				int to_left{ -64 - i };

				std::vector<std::pair<int, int>> moves = {
				{0, to_up}, {0, to_down}, {to_left, 0}, {to_right, 0}
				};

				std::vector<bool> valid_moves(4, true);

				for (std::size_t i{ 0 }; i < valid_moves.size(); i++)
				{
					m_chosen = &p;
					if (valid_moves[i])
					{
						int offset_x{ moves[i].first };
						int offset_y{ moves[i].second };

						moveBishop(offset_x, offset_y, current_owned, rival_owned, loc);
						valid_moves[i] = (m_chosen != nullptr);
						x = offset_x, y = offset_y;
						if (!m_check)
						{
							break;
						}
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

	static sf::SoundBuffer buffer;

	buffer.loadFromFile("sound/checkmate.mp3");

	static sf::Sound sound;
	if (sound.getBuffer() == nullptr)
	{
		sound.setBuffer(buffer);
	}
	
	sound.setVolume(80);
	sound.play();

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
	t.setSmooth(true);
	s.setTexture(t);

	s.setPosition(-85, 215);

	m_window.draw(s);

	
}

bool GameEvent::enemyTurn()
{
	if (!m_playerturn)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void GameEvent::enemyMove()
{
	sf::sleep(sf::seconds(1));

	int points{};
	int calc{};
	ChessPiece* p = nullptr;

	// store the x and y of largest point of chesspiece to be moved later
	int move_x{}, move_y{};

	std::vector<ChessPiece>& current_owned{ (m_playerturn) ? player_owned : enemy_owned };

	std::vector<bool> valid_moves(16, true);

	while (!m_playerturn)
	{
		std::size_t index{};
		for (; index < valid_moves.size(); index++)
		{
			if (!valid_moves[index])
			{
				continue;
			}

			m_chosen = &current_owned[index];

			if (!m_chosen->isMovable())
			{
				valid_moves[index] = false;
				continue;
			}

			sf::Vector2f loc{ m_chosen->getPosition() };


			switch (m_chosen->returnPieceType())
			{
			case ChessPiece::PieceType::PAWN:
			{
				const int twotile{ 128 };
				const int onetile{ 64 };
				const int offsets_x[] = { 0, 64, -64 };

				// double move attempt if not in starting position
				if (loc.y == 96)
				{
					calc = enemyMoveCalc(0, twotile, loc);
					calcPoints(p, calc, points, move_x, move_y, 0, twotile);

					// brute force move if check (put after calcPoints)
					if (m_check)
					{
						m_chosen = p;
						movePiece(move_x, move_y);
						if (m_check)
						{
							m_chosen = p;
						}
						else
						{
							return;
						}
					}
				}

				for (int x : offsets_x)
				{
					calc = enemyMoveCalc(x, onetile, loc);
					calcPoints(p, calc, points, move_x, move_y, x, onetile);
					// brute force move if check (put after calcPoints)
					if (m_check)
					{
						m_chosen = p;
						movePiece(move_x, move_y);
						if (m_check)
						{
							m_chosen = p;
						}
						else
						{
							return;
						}
					}
				}
				break;
			}
			case ChessPiece::PieceType::KNIGHT:
			{
				std::vector<std::pair<int, int>> knight_moves = {
				{64, 128}, {64, -128}, {-64, 128}, {-64, -128},
				{128, 64}, {128, -64}, {-128, 64}, {-128, -64}
				};

				std::vector<bool> valid_moves(8, true);

				for (std::size_t j{ 0 }; j < valid_moves.size(); j++)
				{
					if (valid_moves[j])
					{
						int off_x{ knight_moves[j].first };
						int off_y{ knight_moves[j].second };

						if (outOfBoundaries(off_x, off_y, loc))
						{
							continue;
						}

						calc = enemyMoveCalc(off_x, off_y, loc);
						// make false if that direction is blocked
						valid_moves[j] = (calc != -1);

						if (valid_moves[j])
						{
							calcPoints(p, calc, points, move_x, move_y, off_x, off_y);
							// brute force move if check (put after calcPoints)
							if (m_check)
							{
								m_chosen = p;
								movePiece(move_x, move_y);
								if (m_check)
								{
									m_chosen = p;
								}
								else
								{
									return;
								}
							}
						}
					}
				}
				break;
			}
			case ChessPiece::PieceType::BISHOP:
			{
				// put this bishop direction outside to not get reinitialized in loop
				std::vector<bool> valid_moves(4, true);

				for (int j{ 0 }; j < 480; j += 64)
				{
					int to_down{ 64 + j };
					int to_up{ -64 - j };
					int to_right{ 64 + j };
					int to_left{ -64 - j };
					std::vector<std::pair<int, int>> moves = {
					{to_right, to_up}, {to_left, to_up}, {to_right, to_down}, {to_left, to_down}
					};

					for (std::size_t k{ 0 }; k < valid_moves.size(); k++)
					{
						if (valid_moves[k])
						{
							int off_x{ moves[k].first };
							int off_y{ moves[k].second };

							if (outOfBoundaries(off_x, off_y, loc))
							{
								continue;
							}

							calc = enemyMoveCalc(off_x, off_y, loc);
							// make false if that direction is blocked
							valid_moves[k] = (calc != -1);
							if (valid_moves[k])
							{
								// if enemy is found, dont further check that direction
								if (calc > 0)
								{
									valid_moves[k] = false;
								}
								calcPoints(p, calc, points, move_x, move_y, off_x, off_y);
								// brute force move if check (put after calcPoints)
								if (m_check)
								{
									m_chosen = p;
									movePiece(move_x, move_y);
									if (m_check)
									{
										m_chosen = p;
									}
									else
									{
										return;
									}
								}
							}
						}
					}
				}
				break;
			}
			case ChessPiece::PieceType::QUEEN:
			{
				// put this queen direction outside to not get reinitialized in loop
				std::vector<bool> valid_moves(8, true);

				for (int j{ 0 }; j < 480; j += 64)
				{
					int to_down{ 64 + j };
					int to_up{ -64 - j };
					int to_right{ 64 + j };
					int to_left{ -64 - j };

					std::vector<std::pair<int, int>> moves = {
					{to_right, to_up}, {to_left, to_up}, {to_right, to_down}, {to_left, to_down},
						{0, to_up}, {0, to_down}, {to_left, 0}, {to_right, 0}
					};

					

					for (std::size_t k{ 0 }; k < valid_moves.size(); k++)
					{
						if (valid_moves[k])
						{
							int off_x{ moves[k].first };
							int off_y{ moves[k].second };

							if (outOfBoundaries(off_x, off_y, loc))
							{
								continue;
							}

							calc = enemyMoveCalc(off_x, off_y, loc);
							// make false if that direction is blocked
							valid_moves[k] = (calc != -1);
							if (valid_moves[k])
							{
								// if enemy is found, dont further check that direction
								if (calc > 0)
								{
									valid_moves[k] = false;
								}

								// discourage moving queen
								calc--;

								calcPoints(p, calc, points, move_x, move_y, off_x, off_y);
								// brute force move if check (put after calcPoints)
								if (m_check)
								{
									m_chosen = p;
									movePiece(move_x, move_y);
									if (m_check)
									{
										m_chosen = p;
									}
									else
									{
										return;
									}
								}
							}
						}
					}
				}
				break;
			}
			case ChessPiece::PieceType::ROOK:
			{
				// vector for rook direction (put it outside to not get reinitialized
				std::vector<bool> valid_moves(4, true);

				for (int j{ 0 }; j < 480; j += 64)
				{
					int to_down{ 64 + j };
					int to_up{ -64 - j };
					int to_right{ 64 + j };
					int to_left{ -64 - j };

					std::vector<std::pair<int, int>> moves = {
					{0, to_up}, {0, to_down}, {to_left, 0}, {to_right, 0}
					};

					for (std::size_t k{ 0 }; k < valid_moves.size(); k++)
					{
						if (valid_moves[k])
						{
							int off_x{ moves[k].first };
							int off_y{ moves[k].second };

							if (outOfBoundaries(off_x, off_y, loc))
							{
								continue;
							}

							calc = enemyMoveCalc(off_x, off_y, loc);
							
							// make false if that direction is blocked
							valid_moves[k] = (calc != -1);

							if (valid_moves[k])
							{
								// if enemy is found, dont further check that direction
								if (calc > 0)
								{
									valid_moves[k] = false;
								}

								calcPoints(p, calc, points, move_x, move_y, off_x, off_y);
								// brute force move if check (put after calcPoints)
								if (m_check)
								{
									m_chosen = p;
									movePiece(move_x, move_y);
									if (m_check)
									{
										m_chosen = p;
									}
									else
									{
										return;
									}
								}
							}
						}
					}
				}
				break;
			}
			case ChessPiece::PieceType::KING:
			{
				std::vector<std::pair<int, int>> king_moves = {
				{64, 64}, {64, -64}, {-64, 64}, {-64, -64},
				{0, 64}, {0, -64}, {64, 0}, {-64, 0}, {128, 0}, {-128, 0}
				};

				sf::Vector2f kingloc{ m_chosen->getPosition() };
				std::vector<bool> valid_moves(10, true);

				for (std::size_t j{ 0 }; j < valid_moves.size(); j++)
				{
					if (valid_moves[j])
					{
						int off_x{ king_moves[j].first };
						int off_y{ king_moves[j].second };

						if (outOfBoundaries(off_x, off_y, loc))
						{
							continue;
						}

						calc = enemyMoveCalc(off_x, off_y, loc);
						// make false if that direction is blocked
						valid_moves[j] = (calc != -1);

						if (valid_moves[j])
						{
							if (kingloc.x == 288 && kingloc.y == 32)
							{
								// prioritize castling
								if (off_x == 128 || off_x == -128)
								{
									calc++;
								}
							}
							else
							{
								if (off_x == 128 || off_x == -128)
								{
									calc = -1;
								}
							}
							calcPoints(p, calc, points, move_x, move_y, off_x, off_y);
							// brute force move if check (put after calcPoints)
							if (m_check)
							{
								m_chosen = p;
								movePiece(move_x, move_y);
								if (m_check)
								{
									m_chosen = p;
								}
								else
								{
									return;
								}
							}
						}
					}
				}
				break;
			}
			}

			
		}
		
		if (!m_check)
		{
			m_chosen = p;
			movePiece(move_x, move_y);
			if (pawnPromotion())
			{
				enemyPawnPromotion();
			}
			// still in the same turn then it movement failed
			// then block that piece from moving
			else if (!m_playerturn)
			{
				// find the index again
				for (std::size_t i{ 0 }; i < valid_moves.size(); i++)
				{
					if (current_owned[i].getPosition() == p->getPosition())
					{
						index = i;
					}
				}
				valid_moves[index] = false;
			}
		}
		else
		{
			// every movement failed that means checkmate
			checkMate();
			return;
		}
		
	}
}

bool GameEvent::outOfBoundaries(int x, int y, sf::Vector2f& loc)
{
	int pos_x{ static_cast<int>(x + loc.x) };
	int pos_y{ static_cast<int>(y + loc.y) };

	if ((pos_x > 480 || pos_x < 0) || (pos_y > 480 || pos_y < 0 ))
	{
		return true;
	}

	return false;
}

// function to predict best piece to move using points, if piece and enemy in the way, point extra
// returns points
int GameEvent::enemyMoveCalc(int x, int y, sf::Vector2f& loc)
{
	int points{};
	int helperpoints{};
	std::vector<ChessPiece>& current_owned{ (m_playerturn) ? player_owned : enemy_owned };
	std::vector<ChessPiece>& rival_owned{ (m_playerturn) ? enemy_owned : player_owned };

	int afterpos_x{ static_cast<int>(x + loc.x) };
	int afterpos_y{ static_cast<int>(y + loc.y) };

	bool not_blocked{ true };
	static bool found_enemy{};
	static ChessPiece* previous{ nullptr };

	// to reset the not_blocked if the piece changed
	if (previous != m_chosen)
	{
		// not_blocked = true;
		previous = m_chosen;
		found_enemy = false;
	}

	for (std::size_t j{ 0 }; j < rival_owned.size(); j++)
	{
		ChessPiece::PieceType type{ rival_owned[j].returnPieceType() };
		ChessPiece::PieceType current_type{ current_owned[j].returnPieceType() };

		if (!rival_owned[j].isMovable())
		{
			continue;
		}

		switch (m_chosen->returnPieceType())
		{
		case ChessPiece::PieceType::PAWN:
		{
			
			for (std::size_t k{ 0 }; k < current_owned.size(); k++)
			{
				if (!current_owned[k].isMovable())
				{
					continue;
				}

				if (x != 0)
				{
					// to the chosen tile (no extra offset for checking)
					if (rival_owned[j].returnSprite().getGlobalBounds().contains(afterpos_x, afterpos_y))
					{
						points = checkPieceType(type, points);
						points += 2;
					}
					else if (current_owned[k].returnSprite().getGlobalBounds().contains(afterpos_x, afterpos_y))
					{
						not_blocked = false;
						break;
					}
				}
				else
				{
					// double tile check
					if (y == 128)
					{
						if (rival_owned[j].returnSprite().getGlobalBounds().contains(afterpos_x, afterpos_y - 64))
						{
							not_blocked = false;
							break;
						}
						else if (current_owned[k].returnSprite().getGlobalBounds().contains(afterpos_x, afterpos_y - 64) &&
							current_owned[k].getPosition() != m_chosen->getPosition())
						{
							not_blocked = false;
							break;
						}
					}

					if (current_owned[k].returnSprite().getGlobalBounds().contains(afterpos_x, afterpos_y))
					{
						not_blocked = false;
						break;
					}
					else if (rival_owned[j].returnSprite().getGlobalBounds().contains(afterpos_x, afterpos_y))
					{
						not_blocked = false;
						break;
					}

					if (rival_owned[j].returnSprite().getGlobalBounds().
						contains(64 + afterpos_x, 64 + afterpos_y))
					{
						points = checkPieceType(type, points);
					}
					else if (rival_owned[j].returnSprite().getGlobalBounds().
						contains(-64 + afterpos_x, 64 + afterpos_y))
					{
						points = checkPieceType(type, points);
					}

					// help team piece on the way
					if (current_owned[k].returnSprite().getGlobalBounds().
						contains(-64 + afterpos_x, 64 + afterpos_y))
					{
						helperpoints += 1;
					}
					// help team piece on the way
					else if (current_owned[k].returnSprite().getGlobalBounds().
						contains(64 + afterpos_x, 64 + afterpos_y))
					{
						helperpoints += 1;
					}
				}
			}
			

			break;
		}
		case ChessPiece::PieceType::KNIGHT:
		{
			enemyPointsAssign(current_owned, rival_owned[j], afterpos_x,
				afterpos_y, not_blocked, found_enemy, points, type);
			break;
		}
		case ChessPiece::PieceType::BISHOP:
		{

			enemyPointsAssign(current_owned, rival_owned[j], afterpos_x,
				afterpos_y, not_blocked, found_enemy, points, type);
			break;
		}
		case ChessPiece::PieceType::ROOK:
		{
			enemyPointsAssign(current_owned, rival_owned[j], afterpos_x,
				afterpos_y, not_blocked, found_enemy, points, type);
			break;
		}
		case ChessPiece::PieceType::QUEEN:
		{
			enemyPointsAssign(current_owned, rival_owned[j], afterpos_x,
				afterpos_y, not_blocked, found_enemy, points, type);
			break;
		}
		case ChessPiece::PieceType::KING:
		{
			enemyPointsAssign(current_owned, rival_owned[j], afterpos_x,
				afterpos_y, not_blocked, found_enemy, points, type);
			break;
		}
		}

	}
	
	if (!not_blocked)
	{
		return -1;
	}
	else
	{
		int total{ points + helperpoints };
		return total;
	}
	
}

int GameEvent::checkPieceType(ChessPiece::PieceType& p, int points)
{
	enum class EnemyRank
	{
		PAWN = 2,
		BISHOP = 5,
		ROOK = 5,
		KNIGHT = 5,
		QUEEN = 10,
		KING = 100,
		MAX_RANK
	};

	int value{};

	switch (p)
	{
	case ChessPiece::PieceType::PAWN:
		value = (points <= static_cast<int>(EnemyRank::PAWN)) ?
			static_cast<int>(EnemyRank::PAWN) : points;
		break;
	case ChessPiece::PieceType::BISHOP:
		value = (points <= static_cast<int>(EnemyRank::BISHOP)) ?
			static_cast<int>(EnemyRank::BISHOP) : points;
		break;
	case ChessPiece::PieceType::ROOK:
		value = (points <= static_cast<int>(EnemyRank::ROOK)) ?
			static_cast<int>(EnemyRank::ROOK) : points;
		break;
	case ChessPiece::PieceType::KNIGHT:
		value = (points <= static_cast<int>(EnemyRank::KNIGHT)) ?
			static_cast<int>(EnemyRank::KNIGHT) : points;
		break;
	case ChessPiece::PieceType::QUEEN:
		value = (points <= static_cast<int>(EnemyRank::QUEEN)) ?
			static_cast<int>(EnemyRank::QUEEN) : points;
		break;
	case ChessPiece::PieceType::KING:
		value = (points <= static_cast<int>(EnemyRank::KING)) ?
			static_cast<int>(EnemyRank::KING) : points;
		break;
	}

	return value;
}

// function to compare with the main points, if calc is larger, p is assigned
void GameEvent::calcPoints(ChessPiece*& p, int& calc, int& points, int& move_x, int& move_y,
	int offset_x, int offset_y)
{
	ChessPiece::PieceType chosentype{ m_chosen->returnPieceType() };
	// if check accept any piece
	if (m_check)
	{
		p = m_chosen;
		move_x = offset_x, move_y = offset_y;
		return;
	}
	// prioritize moving forward (backward if end of board)
	if (m_chosen->getPosition().y == 480)
	{
		if (offset_y < 0)
		{
			calc++;
		}
	}
	else if (offset_y > 0 && chosentype !=
		ChessPiece::PieceType::KING)
	{
		if (!(offset_y == 128 && chosentype !=
			ChessPiece::PieceType::PAWN))
		{
			calc++;
		}
	}

	// if currently in dangerous position, prioritize moving this piece
	if (checkSeeker(0, 0) && chosentype !=
		ChessPiece::PieceType::PAWN)
	{
		
		calc += checkPieceType(chosentype, 0);
	}

	// if going to dangerous position reduce probability being moved
	// except pawn
	if (checkSeeker(offset_x, offset_y) && chosentype != 
		ChessPiece::PieceType::PAWN)
	{
		// preventing kamikaze
		if (chosentype != ChessPiece::PieceType::PAWN)
		{
			calc -= 1;
		}
		else
		{
			calc -= 2;
		}
		
	}
	if (calc == points)
	{
		// for early game, p might be nullptr
		if (p == nullptr)
		{
			p = m_chosen;
			move_x = offset_x, move_y = offset_y;
		}
		else
		{
			int random{ Random::get(1,10) };
			if (random % 2 == 0)
			{
				p = m_chosen;
				move_x = offset_x, move_y = offset_y;
			}
			else
			{
				p = p;
			}
		}

		points = calc;
	}
	else if (calc > points)
	{
		points = calc;
		p = m_chosen;
		move_x = offset_x, move_y = offset_y;
	}
}

// function to check if enemy / current in afterpos and its points
void GameEvent::enemyPointsAssign(std::vector<ChessPiece>& current_owned, ChessPiece& rival_owned, int afterpos_x,
	int afterpos_y, bool& not_blocked, bool& found_enemy, int& points, ChessPiece::PieceType type)
{
	for (ChessPiece& current : current_owned)
	{
		if (!current.isMovable())
		{
			continue;
		}

		// in case current piece is blocking, check this condition first
		if (current.returnSprite().getGlobalBounds().
			contains(afterpos_x, afterpos_y))
		{
			not_blocked = false;
		}
	}
	

	// to the chosen tile (no extra offset for checking)
	if (not_blocked)
	{
		if (rival_owned.returnSprite().getGlobalBounds().contains(afterpos_x, afterpos_y))
		{
			// prevent castling move to enemy
			if (m_chosen->returnPieceType() == ChessPiece::PieceType::KING)
			{
				sf::Vector2f loc{ m_chosen->getPosition() };
				if (afterpos_x - 128 == loc.x || afterpos_x + 128 == loc.x)
				{
					points = -100;
				}
			}
			int chosen_value = checkPieceType(m_chosen->returnPieceType(), 0);
			points = checkPieceType(type, points);
			if (chosen_value > points)
			{
				points--;
			}
			points += 1;
		}
	}
	
}

void GameEvent::enemyPawnPromotion()
{
	int random{ Random::get(1, 10) };

	if (random <= 2)
	{
		m_chosen->updateSprite(ChessPiece::ColorType::BLACK,
			ChessPiece::PieceType::ROOK);
	}
	else if (random <= 4)
	{
		m_chosen->updateSprite(ChessPiece::ColorType::BLACK,
			ChessPiece::PieceType::KNIGHT);
	}
	else if (random <= 6)
	{
		m_chosen->updateSprite(ChessPiece::ColorType::BLACK,
			ChessPiece::PieceType::BISHOP);
	}
	else if (random <= 10)
	{
		m_chosen->updateSprite(ChessPiece::ColorType::BLACK,
			ChessPiece::PieceType::QUEEN);
	}

	m_chosen->resetSprite();
	m_playerturn = !m_playerturn;
	m_chosen = nullptr;
	m_promotion = false;
}

const bool GameEvent::isStarted()
{
	return m_gamestart;
}

void GameEvent::showTitle()
{
	sf::Texture t;
	sf::Texture g1;
	sf::Texture g2;
	sf::Texture back;
	if (!t.loadFromFile("chessimages/title.png"))
	{
		std::cout << "showTitle";
	}
	if (!g1.loadFromFile("chessimages/singleplayer.png"))
	{
		std::cout << "showTitle";
	}
	if (!g2.loadFromFile("chessimages/multiplayer.png"))
	{
		std::cout << "showTitle";
	}
	if (!back.loadFromFile("chessimages/menu.png"))
	{
		std::cout << "menu";
	}

	sf::Sprite title;
	sf::Sprite single;
	sf::Sprite multi;
	sf::Sprite background;

	t.setSmooth(true);
	g1.setSmooth(true);
	g2.setSmooth(true);
	back.setSmooth(true);

	title.setTexture(t);
	single.setTexture(g1);
	multi.setTexture(g2);
	background.setTexture(back);

	title.setPosition(130, 120);
	single.setPosition(95, 200);
	single.scale(0.7, 0.7);
	multi.scale(0.7, 0.7);
	multi.setPosition(95, 300);
	background.scale(0.9, 0.9);
	background.setPosition(45, 50);

	m_gamemodes = { single, multi };
	m_window.draw(background);
	m_window.draw(title);
	m_window.draw(single);
	m_window.draw(multi);
}

void GameEvent::chooseModes(int x, int y)
{
	enum GameModes
	{
		SINGLE,
		MULTI,
		MAX_MODES
	};

	GameModes mode{ GameModes::MAX_MODES };
	
	for (std::size_t i{ 0 }; i < m_gamemodes.size(); i++)
	{
		if (m_gamemodes[i].getGlobalBounds().contains(x, y))
		{
			mode = static_cast<GameModes>(i);
			break;
		}
	}

	if (mode == GameModes::SINGLE)
	{
		m_singleplayer = true;
		m_gamestart = true;
	}
	else if (mode == GameModes::MULTI)
	{
		m_singleplayer = false;
		m_gamestart = true;
	}
}

const bool GameEvent::isSinglePlayer()
{
	return m_singleplayer;
}