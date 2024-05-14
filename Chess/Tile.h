#pragma once
#include <SFML/Graphics.hpp>

class Tile : public sf::Drawable
{
private:
	sf::Texture m_white, m_black;
	sf::Sprite m_current;
public:
	Tile();

	void drawTile(bool& white_turn, float origin_x, float origin_y);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
