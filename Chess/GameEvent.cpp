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
	if (m_playerturn)
	{
		for (ChessPiece& piece : player_owned)
		{
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
	else
	{
		for (ChessPiece& piece : enemy_owned)
		{
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

	switch (m_chosen->returnPieceType())
	{
	case ChessPiece::PieceType::PAWN:
		movePawn(x, y);
	}
	/*
	sf::Vector2i loc{ m_chosen->returnSprite().getPosition() };
	x -= loc.x;
	y -= loc.y;
	m_chosen->returnSprite().move(x, y);
	*/
	
	// if player do a wrong move (clicking random tiles) 
	// stay at player turn
	if (m_chosen == nullptr)
	{
		m_playerturn = m_playerturn;
	}
	// else, enemy turn and reset pointer
	else
	{
		m_playerturn = !m_playerturn;
		m_chosen = nullptr;
	}
	

}

void GameEvent::movePawn(int x, int y)
{
	bool first_time{};
	bool eat_enemy{ false };
	sf::Vector2f loc{ m_chosen->returnSprite().getPosition() };

	// check if pawn is in default position. if so enable double move skill
	if (m_chosen->returnSprite().getPosition().y == 416 || 
		m_chosen->returnSprite().getPosition().y == 96)
	{
		first_time = true;
	}
	else
	{
		first_time = false;
	}


	for (sf::Sprite& s : board_vector)
	{
		if (s.getGlobalBounds().contains(x, y))
		{
			// + 32 because chess origin is in middle of sprite
			x = s.getPosition().x - loc.x + 32;
			y = s.getPosition().y - loc.y + 32;
			break;
		}
	}

	// prevent moving back

	if (m_playerturn)
	{
		if (y >= 0)
		{
			m_chosen = nullptr;
			return;
		}
	}
	else
	{
		if (y <= 0)
		{
			m_chosen = nullptr;
			return;
		}
	}

	if (first_time)
	{
		if (y < -160)
		{
			m_chosen = nullptr;
			return;
		}
	}
	else
	{
		if (y < -96)
		{
			m_chosen = nullptr;
			return;
		}
	}

	if ((x == -64 || x == 64) && (y != -160))
	{
		eat_enemy = eatEnemy(x, y);
	}
	if (!eat_enemy)
	{
		m_chosen->returnSprite().move(0, y);
	}
	
	
}

bool GameEvent::eatEnemy(int x, int y)
{
	for (std::size_t i{ 0 }; i < enemy_owned.size(); i++)
	{
		if (enemy_owned[i].returnSprite().getGlobalBounds().
			contains(x + m_chosen->returnSprite().getPosition().x, y + m_chosen->returnSprite().getPosition().y))
		{
			enemy_owned[i].returnSprite().setPosition(600, 500);
			enemy_owned[i].isMovable() = false;
			m_chosen->returnSprite().move(x, y);
			return true;
		}
	}

	return false;
}
