#pragma once
#include <SAGE/Inc/SAGE.h>

enum class Biomes
{
	Desert,
	Forest,
	Mountains,
	Ocean,
	Plains,
	SnowyTundra
};

class Slot
{
public:
	bool isCollapsed = false;
	SAGE::Math::Vector2 position;
	SAGE::Graphics::TextureId collapsedTexture = 0;
	Biomes biome = Biomes::Plains;

	bool isDesert = true;
	bool isForest = true;
	bool isMountains = true;
	bool isOcean = true;
	bool isPlains = true;
	bool isSnowyTundra = true;
};