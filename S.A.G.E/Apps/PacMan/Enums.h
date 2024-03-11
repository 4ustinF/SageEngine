#pragma once

enum class Direction
{
	None = -1,
	Up, 
	Right,
	Down,
	Left
};

enum class GhostType
{
	Blinky,
	Pinky,
	Inky,
	Clyde
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
