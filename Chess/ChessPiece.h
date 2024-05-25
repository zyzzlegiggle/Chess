#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <string_view>
#include <iostream>

class ChessPiece : public sf::Drawable
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

	static constexpr std::array<std::string_view,
		static_cast<std::size_t>(MAX_PIECES)> all_names{ "PAWN", "ROOK", "KNIGHT", "BISHOP", "KING", "QUEEN" };

private:
	ColorType m_color{};
	PieceType m_piece{};
	sf::Texture m_texture;
	sf::Sprite m_sprite;
	bool m_movable{ true };

public:
	ChessPiece() = default;

	void updateSprite(ColorType color, PieceType piece);

	sf::Sprite& returnSprite();

	sf::Texture& returnTexture();

	ColorType& returnColorType();

	PieceType& returnPieceType();

	const std::string_view returnName();

	void moveSprite(int x, int y);

	bool& isMovable();

	void resetSprite();

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
