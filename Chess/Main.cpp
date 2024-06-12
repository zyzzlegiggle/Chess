#include <SFML/Graphics.hpp>
#include "Board.h"
#include "Player.h"
#include "Enemy.h"
#include "GameEvent.h"
#include <iostream>


int WinMain()
{

	unsigned int screen_width{ 512 };
	unsigned int screen_height{ 512 };


	sf::RenderWindow window(sf::VideoMode(screen_width, screen_height), "Chess by zyzzlegiggle", sf::Style::Close);

	sf::Image icon;
	icon.loadFromFile("chessimages/b_knight_png_shadow_128px.png");

	// set icon
	window.setIcon(128, 128, icon.getPixelsPtr());

	// Limit the framerate to 30 frames per second
	window.setFramerateLimit(30);

	Board board{ static_cast<std::size_t>(8) };
	Player player{ true };
	Enemy enemy{ false };
	GameEvent game{ player, board, enemy, window};

	
	// run program as long window is open
	while (window.isOpen())
	{
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		
		game.checkSeeker();
		game.staleCheck();
		//test
		
		if (game.isStarted() && game.isSinglePlayer())
		{
			if (game.enemyTurn() && !game.isCheckmate())
			{
				if (game.oneKing())
				{
					game.checkMate();
				}
				else
				{
					game.enemyMove();
				}
			}
		}
		
		while (window.pollEvent(event))
		{

			// click a piece
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					std::cout << "the right button was pressed" << std::endl;
					std::cout << "mouse x: " << event.mouseButton.x << std::endl;
					std::cout << "mouse y: " << event.mouseButton.y << std::endl;
					
					if (!game.isStarted())
					{
						game.chooseModes(event.mouseButton.x, event.mouseButton.y);
					}
					else
					{
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
										std::cout << "checkmate\n";
									}
								}

								game.choosePiece(event.mouseButton.x, event.mouseButton.y);
							}
						}
						else
						{
							break;
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



		if (!game.isStarted())
		{
			game.showTitle();
		}
		else
		{
			
			game.showPieces();

			if (game.pawnPromotion())
			{
				game.promotionCheck();
			}

			if (game.isCheckmate())
			{
				game.showCheckmate();
			}
		}
		window.display();
	}
}