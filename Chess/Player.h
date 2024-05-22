#pragma once
#include <vector>
#include "ChessPiece.h"

class Player
{
private:
	bool is_white{};
	std::vector<ChessPiece> m_pieces{};

public:
	Player(bool white);

	bool& returnColor();

	ChessPiece& returnPiece(std::size_t i);

	std::vector<ChessPiece>& returnVector();

	
};

