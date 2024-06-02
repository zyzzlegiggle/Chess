#include "Board.h"

Board::Board(std::size_t row)
	: m_boardsize{ row }
{
	m_board.resize(row * row);
	m_black.loadFromFile("chessimages/square_gray_dark_png_shadow_128px.png");
	m_white.loadFromFile("chessimages/square_gray_light_png_shadow_128px.png");

	m_boxB.setTexture(m_black);
	m_boxW.setTexture(m_white);
}

std::vector<sf::Sprite>& Board::returnVector()
{
	return m_board;
}

sf::Sprite& Board::returnTileB()
{
	return m_boxB;
}

sf::Sprite& Board::returnTileW()
{
	return m_boxW;
}

const std::size_t Board::returnBoardSize()
{
	return m_boardsize;
}
