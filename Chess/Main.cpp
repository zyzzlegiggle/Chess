#include <SFML/Graphics.hpp>
#include "Tile.h"
#include "Board.h"
#include "Player.h"
#include "Enemy.h"
#include "GameEvent.h"
#include <iostream>


/*
void resizeHandler(unsigned int& height, unsigned int& width, Board& board)
{

}
*/

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
	Player player{ true };
	Enemy enemy{ false };
	GameEvent game{ player, board, enemy, window};

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
				screen_width = size.x;

				//resizeHandler(screen_height, screen_width, board);
			}

			// click a piece
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					std::cout << "the right button was pressed" << std::endl;
					std::cout << "mouse x: " << event.mouseButton.x << std::endl;
					std::cout << "mouse y: " << event.mouseButton.y << std::endl;

					game.checkSeeker();
					game.staleCheck();
					
					

					if (!game.isCheckmate())
					{
						if (game.pawnPromotion())
						{
							game.choosePromotion(event.mouseButton.x, event.mouseButton.y);
						}
						// main game 
						else if (game.isChosen())
						{
							game.movePiece(event.mouseButton.x, event.mouseButton.y);
						}
						else
						{
							if (game.oneKing())
							{
								game.checkMate();
							}
							else if (game.isStale())
							{
								// makes m_chosen null, if outside, will stuck at !isChosen()
								if (!game.findHelper())
								{
									game.checkMate();
									std::cout << "checkmate";
								}
							}
							game.choosePiece(event.mouseButton.x, event.mouseButton.y);
						}
					}
				}
			}

			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		// clear the window with black color
		window.clear(sf::Color::Black);

		game.showBoard();

		game.showPieces();
		
		if (game.pawnPromotion())
		{
 			game.promotionCheck();
		}

		window.display();
	}
}