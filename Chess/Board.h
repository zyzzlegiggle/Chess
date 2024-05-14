#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Tile.h"

class Board
{
private:
	std::size_t m_boardsize;
	std::vector <Tile> m_board;
public:
	Board(std::size_t row);

	void drawBoard(sf::RenderWindow& window);
};
