#include "Board.h"

Board::Board(std::size_t row)
	: m_boardsize{ row }
{
	m_board.resize(row * row);
	m_black.loadFromFile("chessimages/square_brown_dark_png_shadow_128px.png");
	m_white.loadFromFile("chessimages/square_brown_light_png_shadow_128px.png");

	m_boxB.setTexture(m_black);
	m_boxW.setTexture(m_white);
}

void Board::drawBoard(sf::RenderWindow& window)
{
	// put it inside the vector
	std::size_t current_tile;
	bool white_turn{ true };

	// set to 32 because origin is in middle of sprite (64/2)
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
				m_board[current_tile] = m_boxW;
				white_turn = false;
			}
			else
			{
				m_board[current_tile] = m_boxB;
				white_turn = true;
			}

			// draw tile on the app
			m_board[current_tile].setPosition(origin_x, origin_y);
			m_board[current_tile].scale(0.5f, 0.5f);
			window.draw(m_board[current_tile]);
			origin_x += 64;
		}

		// move to new row and reset column position
		origin_y += 64;
		origin_x = 0;
	}
}

std::vector<sf::Sprite>& Board::returnVector()
{
	return m_board;
}
