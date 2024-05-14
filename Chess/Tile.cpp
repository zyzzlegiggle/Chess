#include "Tile.h"
#include <iostream>

Tile::Tile()
{
	// assigning tile images
	m_white.loadFromFile("chessimages/square_brown_light_png_shadow_128px.png");
	m_black.loadFromFile("chessimages/square_brown_dark_png_shadow_128px.png");
}

void Tile::drawTile(bool& white_turn, float origin_x, float origin_y)
{
	if (white_turn)
	{
		m_current.setTexture(m_white);
		white_turn = false;
	}
	else
	{
		m_current.setTexture(m_black);
		white_turn = true;
	}

	m_current.setPosition(origin_x, origin_y);
	m_current.scale(0.5f, 0.5f);
}

void Tile::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_current, states);
}
