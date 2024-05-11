#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <array>

class ChessPiece: public sf::Drawable
{
public:
	enum ColorType
	{
		BLACK,
		WHITE,
		MAX_COLORS
	};

	enum PieceType
	{
		PAWN,
		ROOK,
		KNIGHT,
		BISHOP,
		KING,
		QUEEN,
		MAX_PIECES
	};

	static constexpr std::array<PieceType, static_cast<std::size_t>(MAX_PIECES)> all_pieces{ PAWN, ROOK,
																			KNIGHT, BISHOP, KING, QUEEN };

private:
	ColorType m_color{};
	PieceType m_piece{};
	sf::Texture m_texture;
	sf::Sprite m_sprite;
	

public:
	ChessPiece() = default;

	void updateSprite(ColorType color, PieceType piece)
	{
		m_color = color;
		m_piece = piece;
		std::string filename{};

		if (m_color == ColorType::BLACK)
		{
			switch (m_piece)
			{
			case ChessPiece::PieceType::PAWN:
				filename = "chessimages/b_pawn_png_shadow_128px.png";
				break;
			case ChessPiece::PieceType::ROOK:
				filename = "chessimages/b_rook_png_shadow_128px.png";
				break;
			case ChessPiece::PieceType::KNIGHT:
				filename = "chessimages/b_knight_png_shadow_128px.png";
				break;
			case ChessPiece::PieceType::BISHOP:
				filename = "chessimages/b_bishop_png_shadow_128px.png";
				break;
			case ChessPiece::PieceType::KING:
				filename = "chessimages/b_king_png_shadow_128px.png";
				break;
			case ChessPiece::PieceType::QUEEN:
				filename = "chessimages/b_queen_png_shadow_128px.png";
				break;
			}
		}
		else if (m_color == ColorType::WHITE)
		{
			switch (m_piece)
			{
			case ChessPiece::PieceType::PAWN:
				filename = "chessimages/w_pawn_png_shadow_128px.png";
				break;
			case ChessPiece::PieceType::ROOK:
				filename = "chessimages/w_rook_png_shadow_128px.png";
				break;
			case ChessPiece::PieceType::KNIGHT:
				filename = "chessimages/w_knight_png_shadow_128px.png";
				break;
			case ChessPiece::PieceType::BISHOP:
				filename = "chessimages/w_bishop_png_shadow_128px.png";
				break;
			case ChessPiece::PieceType::KING:
				filename = "chessimages/w_king_png_shadow_128px.png";
				break;
			case ChessPiece::PieceType::QUEEN:
				filename = "chessimages/w_queen_png_shadow_128px.png";
				break;
			}
		}

		if (!m_texture.loadFromFile(filename))
		{
			std::cout << "Error: Couldn't load texture\n";
		}
		else
		{
			m_sprite.setTexture(m_texture);
			m_sprite.scale(0.5f, 0.5f);
		}

		
	}

	sf::Sprite& returnSprite()
	{
		return m_sprite;
	}

	sf::Texture& returnTexture()
	{
		return m_texture;
	}
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const 
	{
		if (m_sprite.getTexture() == nullptr)
		{
			std::cout << "no";
		}
		target.draw(m_sprite, states);
		
	}
};

class Player
{
private:
	bool is_white{};
	std::vector<ChessPiece> m_pieces{};
	
public:
	Player(bool white)
		: is_white{white}
	{
		m_pieces.resize(16);

		std::size_t index{};

		while (index < 8)
		{
			m_pieces[index++].updateSprite(ChessPiece::ColorType::WHITE, ChessPiece::PieceType::PAWN);
		}

		m_pieces[index++].updateSprite(ChessPiece::ColorType::WHITE, ChessPiece::PieceType::ROOK);
		m_pieces[index++].updateSprite(ChessPiece::ColorType::WHITE, ChessPiece::PieceType::KNIGHT);
		m_pieces[index++].updateSprite(ChessPiece::ColorType::WHITE, ChessPiece::PieceType::BISHOP);
		m_pieces[index++].updateSprite(ChessPiece::ColorType::WHITE, ChessPiece::PieceType::QUEEN);
		m_pieces[index++].updateSprite(ChessPiece::ColorType::WHITE, ChessPiece::PieceType::KING);
		m_pieces[index++].updateSprite(ChessPiece::ColorType::WHITE, ChessPiece::PieceType::BISHOP);
		m_pieces[index++].updateSprite(ChessPiece::ColorType::WHITE, ChessPiece::PieceType::KNIGHT);
		m_pieces[index].updateSprite(ChessPiece::ColorType::WHITE, ChessPiece::PieceType::ROOK);
	}

	bool& returnColor()
	{
		return is_white;
	}

	ChessPiece& returnPiece(std::size_t i)
	{
		return m_pieces[i];
	}
	
	void showPieces(sf::RenderWindow& window)
	{
		float origin_x{};
		float origin_y{ 384 };
		std::size_t index{0};
		for (std::size_t i{ 0 }; i < 2; i++)
		{
			for (std::size_t j{ 0 }; j < 8; j++)
			{
				m_pieces[index].returnSprite().setPosition(origin_x, origin_y);
				window.draw(m_pieces[index++]);
				origin_x += 64;
			}
			origin_y += 64;
			origin_x = 0;
		}
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
	Player player{true};

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
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		// clear the window with black color
		window.clear(sf::Color::Black);

		board.drawBoard(window);


		window.draw(player.returnPiece(1));
		player.showPieces(window);

		window.display();
	}
}