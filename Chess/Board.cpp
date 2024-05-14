#include "Board.h"

Board::Board(std::size_t row)
	: m_boardsize{ row }
{
	m_board.resize(row * row);
}

void Board::drawBoard(sf::RenderWindow& window)
{
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

			m_board[current_tile].drawTile(white_turn, origin_x, origin_y);

			// draw tile on the app
			window.draw(m_board[current_tile]);
			origin_x += 64;
		}

		// move to new row and reset column position
		origin_y += 64;
		origin_x = 0;
	}


}
