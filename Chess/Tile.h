#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

class Tile
{
public:
	sf::Texture loadWhite();

	sf::Texture loadBlack();

	
private:
	sf::Sprite m_current;
	sf::Texture m_white, m_black;
public:
	Tile();

	void drawTile(bool& white_turn, float origin_x, float origin_y);

	//virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	sf::Sprite returnS();
};
