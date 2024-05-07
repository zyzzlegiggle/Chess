#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

class ChessPiece
{
private:
	sf::Texture m_texture{};
	sf::Sprite m_sprite{};
public:
	ChessPiece()
	{
		m_texture.loadFromFile("chessimages/white_rook.png");
		m_sprite.setTexture(m_texture);
	}

	sf::Sprite& returnSprite()
	{
		return m_sprite;
	}
};


class Board
{
private:
	sf::Texture m_white, m_black;
	sf::Sprite m_boxW, m_boxB;

	std::size_t m_boardsize;
	std::vector <sf::Sprite> m_board;
public:
	Board(std::size_t row)
		: m_boardsize{row}
	{
		// assigning tile images
		m_white.loadFromFile("chessimages/square_brown_light_png_shadow_128px.png");
		m_boxW.setTexture(m_white);

		m_black.loadFromFile("chessimages/square_brown_dark_png_shadow_128px.png");
		m_boxB.setTexture(m_black);

		m_board.resize(row * row);

		
		
	}

	sf::Sprite& checkColor()
	{
		return m_board[1];
	}

	void drawBoard(sf::RenderWindow& window)
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
				origin_x += 64.f;
			}

			// move to new row and reset column position
			origin_y += 64.f;
			origin_x = 0;
		}


	}
};



int main()
{
	// get screen size and adjust
	/*
	sf::VideoMode desktop{ sf::VideoMode::getDesktopMode() };
	*/

	unsigned int screen_width{ 512 };
	unsigned int screen_height{ 512 };
	

	sf::RenderWindow window(sf::VideoMode(screen_width, screen_height), "Chess");
	Board board{ static_cast<std::size_t>(8) };
	

	// run program as long window is open

	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;

		while (window.pollEvent(event))
		{
			// The window was resized
			if (event.type == sf::Event::Resized)
			{
				sf::Vector2u size = window.getSize();
				screen_height = size.y;
				screen_height = 
			}
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		// clear the window with black color
		window.clear(sf::Color::Black);

		board.drawBoard(window);

		window.display();
	}
}