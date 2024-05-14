#include "Player.h"

Player::Player(bool white)
	: is_white{ white }
{
	m_pieces.resize(16);

	std::size_t index{};

	if (is_white)
	{
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
	else
	{
		while (index < 8)
		{
			m_pieces[index++].updateSprite(ChessPiece::ColorType::BLACK, ChessPiece::PieceType::PAWN);
		}

		m_pieces[index++].updateSprite(ChessPiece::ColorType::BLACK, ChessPiece::PieceType::ROOK);
		m_pieces[index++].updateSprite(ChessPiece::ColorType::BLACK, ChessPiece::PieceType::KNIGHT);
		m_pieces[index++].updateSprite(ChessPiece::ColorType::BLACK, ChessPiece::PieceType::BISHOP);
		m_pieces[index++].updateSprite(ChessPiece::ColorType::BLACK, ChessPiece::PieceType::QUEEN);
		m_pieces[index++].updateSprite(ChessPiece::ColorType::BLACK, ChessPiece::PieceType::KING);
		m_pieces[index++].updateSprite(ChessPiece::ColorType::BLACK, ChessPiece::PieceType::BISHOP);
		m_pieces[index++].updateSprite(ChessPiece::ColorType::BLACK, ChessPiece::PieceType::KNIGHT);
		m_pieces[index].updateSprite(ChessPiece::ColorType::BLACK, ChessPiece::PieceType::ROOK);
	}

}

bool& Player::returnColor()
{
	return is_white;
}

ChessPiece& Player::returnPiece(std::size_t i)
{
	return m_pieces[i];
}

std::vector<ChessPiece>& Player::returnVector()
{
	return m_pieces;
}