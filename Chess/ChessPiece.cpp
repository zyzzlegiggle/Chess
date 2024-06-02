#include "ChessPiece.h"

void ChessPiece::updateSprite(ColorType color, PieceType piece)
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
		m_sprite.setOrigin(m_sprite.getLocalBounds().width / 2, m_sprite.getLocalBounds().height / 2);
			
	}


}

sf::Sprite& ChessPiece::returnSprite()
{
	return m_sprite;
}

sf::Texture& ChessPiece::returnTexture()
{
	return m_texture;
}

ChessPiece::ColorType& ChessPiece::returnColorType()
{
	return m_color;
}

ChessPiece::PieceType& ChessPiece::returnPieceType()
{
	return m_piece;
}

const std::string_view ChessPiece::returnName()
{
	return all_names[m_piece];
}

void ChessPiece::moveSprite(int x, int y)
{
	m_sprite.move(x, y);
}

bool& ChessPiece::isMovable()
{
	return m_movable;
}

void ChessPiece::resetSprite()
{
	m_sprite.setScale(0.5f, 0.5f);
	m_sprite.setOrigin(m_sprite.getLocalBounds().width / 2, m_sprite.getLocalBounds().height / 2);
}

// always call this function with false argument before assigning m_chosen to nullptr
void ChessPiece::setActive(bool is_active)
{
	static sf::Texture active;
	std::string filename{};

	if (m_color == ColorType::BLACK)
	{
		switch (m_piece)
		{
		case ChessPiece::PieceType::PAWN:
			filename = "chessimages/active_b_pawn_png_shadow_128px.png";
			break;
		case ChessPiece::PieceType::ROOK:
			filename = "chessimages/active_b_rook_png_shadow_128px.png";
			break;
		case ChessPiece::PieceType::KNIGHT:
			filename = "chessimages/active_b_knight_png_shadow_128px.png";
			break;
		case ChessPiece::PieceType::BISHOP:
			filename = "chessimages/active_b_bishop_png_shadow_128px.png";
			break;
		case ChessPiece::PieceType::KING:
			filename = "chessimages/active_b_king_png_shadow_128px.png";
			break;
		case ChessPiece::PieceType::QUEEN:
			filename = "chessimages/active_b_queen_png_shadow_128px.png";
			break;
		}
	}
	else if (m_color == ColorType::WHITE)
	{
		switch (m_piece)
		{
		case ChessPiece::PieceType::PAWN:
			filename = "chessimages/active_w_pawn_png_shadow_128px.png";
			break;
		case ChessPiece::PieceType::ROOK:
			filename = "chessimages/active_w_rook_png_shadow_128px.png";
			break;
		case ChessPiece::PieceType::KNIGHT:
			filename = "chessimages/active_w_knight_png_shadow_128px.png";
			break;
		case ChessPiece::PieceType::BISHOP:
			filename = "chessimages/active_w_bishop_png_shadow_128px.png";
			break;
		case ChessPiece::PieceType::KING:
			filename = "chessimages/active_w_king_png_shadow_128px.png";
			break;
		case ChessPiece::PieceType::QUEEN:
			filename = "chessimages/active_w_queen_png_shadow_128px.png";
			break;
		
		}
	}
	if (is_active)
	{
		active.loadFromFile(filename);
		m_sprite.setTexture(active);
	}
	else
	{
		m_sprite.setTexture(m_texture);
	}
}

void ChessPiece::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (m_sprite.getTexture() == nullptr)
	{
		std::cout << "no";
	}
	target.draw(m_sprite, states);

}

