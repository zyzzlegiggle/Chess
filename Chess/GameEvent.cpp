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

bool GameEvent::returnChosen()
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
	bool point_piece{ false };
	for (ChessPiece& piece : player_owned)
	{
		if (piece.returnSprite().getGlobalBounds().contains(x, y))
		{
			point_piece = true;
		}
	}

	if (point_piece == false)
	{
		sf::Vector2i loc{ m_chosen->returnSprite().getPosition() };
		x -= loc.x;
		y -= loc.y;
		m_chosen->returnSprite().move(x, y);

	}

	m_chosen = nullptr;

}
