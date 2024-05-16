#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Board
{
private:
	std::size_t m_boardsize{};
	sf::Texture m_white, m_black;
	sf::Sprite m_boxB, m_boxW;
	std::vector <sf::Sprite> m_board{};
public:
	Board(std::size_t row);

	void drawBoard(sf::RenderWindow& window);

	std::vector<sf::Sprite>& returnVector();
};
