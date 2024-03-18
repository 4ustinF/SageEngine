#pragma once

enum class Direction
{
	None = -1,
	Up, 
	Right,
	Down,
	Left,
	Size
};

enum class GhostType
{
	Blinky,
	Pinky,
	Inky,
	Clyde,
	Size
};

enum class GhostMode
{
	Chase,
	Scatter,
	Frightened,
	Eaten,
	Home,
};

enum class GhostHomeState
{
	None = -1,
	GoingToHomeEntrance,
	GoingIntoHome,
	Idle,
	LeavingHome,
};

enum class PelletType
{
	Small = 10, // Represents 10 points
	Big = 50 // Represents 50 points
};

enum class BonusSymbol
{
	Cherries = 100,
	Strawberry = 300,
	Peach = 500,
	Apple = 700,
	Grapes = 1000,
	Galaxian = 2000,
	Bell = 3000,
	Key = 5000
};

namespace
{
	SAGE::Math::Vector2Int DirectionToVector2Int(Direction direction)
	{
		switch (direction)
		{
		case Direction::Up:
			return { 0, -1 };
		case Direction::Right:
			return { +1, 0 };
		case Direction::Down:
			return { 0, +1 };
		case Direction::Left:
			return { -1, 0 };
		}
		return SAGE::Math::Vector2Int::Zero;
	}

	Direction GetOppositeDirection(Direction direction)
	{
		switch (direction)
		{
		case Direction::Up:
			return Direction::Down;
		case Direction::Right:
			return Direction::Left;
		case Direction::Down:
			return Direction::Up;
		case Direction::Left:
			return Direction::Right;
		}

		return Direction::None;
	}

	std::string GhostTypeToString(GhostType type)
	{
		switch (type)
		{
		case GhostType::Blinky:
			return "Blinky";
		case GhostType::Pinky:
			return "Pinky";
		case GhostType::Inky:
			return "Inky";
		case GhostType::Clyde:
			return "Clyde";
		}
		return "";
	}
}