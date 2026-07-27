#include "Precompiled.h"
#include "MeshBuilder.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Input;
using namespace SAGE::Graphics;

Color GetNextColor(int& index)
{
	constexpr Color colorTable[] = {
		Colors::DarkMagenta,
		Colors::LightSkyBlue,
		Colors::MediumSpringGreen,
		Colors::Violet,
		Colors::Azure,
		Colors::Firebrick,
		Colors::Orange,
		Colors::DarkTurquoise
	};

	index = (index + 1) % std::size(colorTable);
	return colorTable[index];
}

#pragma region ---PC---

MeshPC MeshBuilder::CreateCubePC()
{
	MeshPC mesh;
	const float size = 0.5f;
	int colorIndex = 0;

	//8
	mesh.vertices.push_back({{-size, -size, -size}, GetNextColor(colorIndex)}); //0 - BackBotLeft
	mesh.vertices.push_back({{-size, +size, -size}, GetNextColor(colorIndex)}); //1 - BackTopLeft
	mesh.vertices.push_back({{+size, +size, -size}, GetNextColor(colorIndex)}); //2 - BackTopRight
	mesh.vertices.push_back({{+size, -size, -size}, GetNextColor(colorIndex)}); //3 - BackBotRight

	mesh.vertices.push_back({{-size, -size, +size}, GetNextColor(colorIndex)}); //4 - FrontBotLeft
	mesh.vertices.push_back({{-size, +size, +size}, GetNextColor(colorIndex)}); //5 - FrontTopLeft
	mesh.vertices.push_back({{+size, +size, +size}, GetNextColor(colorIndex)}); //6 - FrontTopRight
	mesh.vertices.push_back({{+size, -size, +size}, GetNextColor(colorIndex)}); //7 - FrontBotRight
	
	//36					
	mesh.indices = {0, 1, 2,	3, 0, 2,
					3, 2, 6,	7, 3, 6,
					4, 5, 1,	0, 4, 1,
					6, 5, 7,	4, 7, 5,
					1, 5, 6,	2, 1, 6,
					4, 0, 3,	7, 4, 3 
	};

	return mesh;
}

MeshPC MeshBuilder::CreatePlanePC(int columns, int rows)
{
	if (columns < 1) {
		columns = 1;
	}
	if (rows < 1) {
		rows = 1;
	}

	MeshPC mesh;
	const float spacing = 1.0f;
	int colorIndex = 0;
	const float xOffSet = columns * 0.5f;
	const float zOffSet = rows * 0.5f;

	mesh.indices = {};

	for (int i = 0; i <= rows; ++i) {
		for (int j = 0; j <= columns; ++j) {
			mesh.vertices.push_back({ {j * spacing - xOffSet, 0.0f, i * spacing - zOffSet}, GetNextColor(colorIndex) });

			if (i != rows && j != columns) {
				mesh.indices.push_back(j + i + (i * columns) + 2 + columns);	//TopRight
				mesh.indices.push_back(j + i + (i * columns) + 1);				//Bottom Right
				mesh.indices.push_back(j + i + (i * columns));					//Bottom Left

				mesh.indices.push_back(j + i + (i * columns) + 2 + columns);	//TopRight
				mesh.indices.push_back(j + i + (i * columns));					//Bottom Left
				mesh.indices.push_back(j + i + (i * columns) + 1 + columns);	//TopLeft
			}
		}
	}

	return mesh;
}

MeshPC MeshBuilder::CreateCylinderPC(int slices, int rings)
{
	if (slices < 3) { slices = 3; }
	if (rings < 1) { rings = 1; }

	MeshPC mesh;
	const float spacing = 1.0f;
	int colorIndex = 0;

	const float yOffSet = rings * 0.5f;

	const float sectorStep = Math::Constants::TwoPi / slices;
	float sectorAngle; //Radian

	mesh.indices = {};

	for (int i = 0; i <= rings; ++i) {
		for (int j = 0; j <= slices; ++j) {
			sectorAngle = j * sectorStep;
			mesh.vertices.push_back({ {cos(sectorAngle), i * spacing - yOffSet, sin(sectorAngle)}, GetNextColor(colorIndex) });

			if (i != rings && j != slices) {
				mesh.indices.push_back(j + i + (i * slices) + 2 + slices);		//TopRight
				mesh.indices.push_back(j + i + (i * slices) + 1);				//Bottom Right
				mesh.indices.push_back(j + i + (i * slices));					//Bottom Left

				mesh.indices.push_back(j + i + (i * slices) + 2 + slices);		//TopRight
				mesh.indices.push_back(j + i + (i * slices));					//Bottom Left
				mesh.indices.push_back(j + i + (i * slices) + 1 + slices);		//TopLeft
			}
		}
	}
	mesh.vertices.push_back({ {0.0f, rings * spacing - yOffSet, 0.0f}, GetNextColor(colorIndex) }); //TopMiddle
	mesh.vertices.push_back({ {0.0f, rings * 0.5f * spacing - 2 * yOffSet, 0.0f}, GetNextColor(colorIndex) }); //BotMiddle

	//Does the top and bot circles
	const int meshVerticesSize = static_cast<int>(mesh.vertices.size());
	for (int i = 0; i < slices; ++i) 
	{
		mesh.indices.push_back(meshVerticesSize - 2); //TopMiddle
		mesh.indices.push_back(meshVerticesSize - (3 + i));
		mesh.indices.push_back(meshVerticesSize - (4 + i));

		mesh.indices.push_back(meshVerticesSize - 1); //BotMiddle
		mesh.indices.push_back(i);
		mesh.indices.push_back(i + 1);
	}
	
	return mesh;
}

MeshPC MeshBuilder::CreateSpherePC(int slices, int rings, float radius)
{
	if (slices < 4) { slices = 4; }
	if (rings < 2) { rings = 2; }
	if (radius < 0.0f) { radius *= -1.0f; }

	MeshPC mesh;
	int colorIndex = 0;
	mesh.indices = {};

	const float sectorStep = Math::Constants::TwoPi / slices;
	float sectorAngle; //Radian

	const float sectorLeap = Math::Constants::Pi / rings;
	float sectorLeapAngle; //Radian

	for (int i = 0; i <= rings; ++i) {
		sectorLeapAngle = i * sectorLeap;
		for (int j = 0; j <= slices; ++j) {
			sectorAngle = j * sectorStep;
			float x = radius * sin(sectorAngle) * sin(sectorLeapAngle);
			float y = radius * cos(sectorLeapAngle);
			float z = radius * cos(sectorAngle) * sin(sectorLeapAngle);
			mesh.vertices.push_back({{x, y, z}, GetNextColor(colorIndex)});

			if (i != rings && j != slices) {
				mesh.indices.push_back(j + i + (i * slices) + 2 + slices);		//TopRight
				mesh.indices.push_back(j + i + (i * slices) + 1);				//Bottom Right
				mesh.indices.push_back(j + i + (i * slices));					//Bottom Left

				mesh.indices.push_back(j + i + (i * slices) + 2 + slices);		//TopRight
				mesh.indices.push_back(j + i + (i * slices));					//Bottom Left
				mesh.indices.push_back(j + i + (i * slices) + 1 + slices);		//TopLeft
			}
		}
	}

	return mesh;
}

#pragma endregion

#pragma region ---PX---

MeshPX MeshBuilder::CreateCubePX()
{
	//24 unique vertices
	MeshPX mesh;
	const float size = 0.5f;

	//Front
	mesh.vertices.push_back({ {-size, -size, +size}, Math::Vector2::YAxis});	//0 - FrontBotLeft
	mesh.vertices.push_back({ {-size, +size, +size}, Math::Vector2::Zero });	//1 - FrontTopLeft
	mesh.vertices.push_back({ {+size, +size, +size}, Math::Vector2::XAxis });	//2 - FrontTopRight
	mesh.vertices.push_back({ {+size, -size, +size}, Math::Vector2::One });		//3 - FrontBotRight

	//Back
	mesh.vertices.push_back({ {-size, -size, -size}, Math::Vector2::YAxis });	//4 - BackBotLeft
	mesh.vertices.push_back({ {-size, +size, -size}, Math::Vector2::Zero });	//5 - BackTopLeft
	mesh.vertices.push_back({ {+size, +size, -size}, Math::Vector2::XAxis });	//6 - BackTopRight
	mesh.vertices.push_back({ {+size, -size, -size}, Math::Vector2::One });		//7 - BackBotRight

	//Left
	mesh.vertices.push_back({ {+size, -size, -size}, Math::Vector2::YAxis });	//8  - LeftBotLeft
	mesh.vertices.push_back({ {+size, +size, -size}, Math::Vector2::Zero });	//9  - LeftTopLeft
	mesh.vertices.push_back({ {+size, +size, +size}, Math::Vector2::XAxis });	//10 - LeftTopRight
	mesh.vertices.push_back({ {+size, -size, +size}, Math::Vector2::One });		//11 - LeftBotRight

	//Right
	mesh.vertices.push_back({ {-size, -size, -size}, Math::Vector2::YAxis });	//12 - RightBotLeft
	mesh.vertices.push_back({ {-size, +size, -size}, Math::Vector2::Zero });	//13 - RightTopLeft
	mesh.vertices.push_back({ {-size, +size, +size}, Math::Vector2::XAxis });	//14 - RightTopRight
	mesh.vertices.push_back({ {-size, -size, +size}, Math::Vector2::One });		//15 - RightBotRight

	//Top
	mesh.vertices.push_back({ {+size, +size, +size}, Math::Vector2::YAxis });	//16 - TopBotLeft
	mesh.vertices.push_back({ {+size, +size, -size}, Math::Vector2::Zero });	//17 - TopTopLeft
	mesh.vertices.push_back({ {-size, +size, -size}, Math::Vector2::XAxis });	//18 - TopTopRight
	mesh.vertices.push_back({ {-size, +size, +size}, Math::Vector2::One });		//19 - TopBotRight

	//Bot
	mesh.vertices.push_back({ {+size, -size, +size}, Math::Vector2::YAxis });	//20 - BotBotLeft
	mesh.vertices.push_back({ {+size, -size, -size}, Math::Vector2::Zero });	//21 - BotTopLeft
	mesh.vertices.push_back({ {-size, -size, -size}, Math::Vector2::XAxis });	//22 - BotTopRight
	mesh.vertices.push_back({ {-size, -size, +size}, Math::Vector2::One });		//23 - BotBotRight

	mesh.indices = {2, 1, 0,	2, 0, 3,	//Front
					4, 5, 6,	7, 4, 6,	//Back
					8, 9, 10,	11, 8,10,	//Left  
					14,13,12,	14,12,15,	//Right 
					16,17,18,	19,16,18,	//Top
					22,21,20,	22,20,23,	//Bot
	};

	return mesh;
}

MeshPX MeshBuilder::CreateSkyBoxPX()
{
	//24 unique vertices
	MeshPX mesh;
	const float size = 0.5f;

	const float third = 1.0f / 3.0f;
	const float twoThird = 2.0f / 3.0f;

	//Left
	mesh.vertices.push_back({ {+size, -size, -size}, Math::Vector2{0.00f, twoThird}});	//0  - LeftBotLeft
	mesh.vertices.push_back({ {+size, +size, -size}, Math::Vector2{0.00f, third}});		//1  - LeftTopLeft
	mesh.vertices.push_back({ {+size, +size, +size}, Math::Vector2{0.25f, third}});		//2 - LeftTopRight
	mesh.vertices.push_back({ {+size, -size, +size}, Math::Vector2{0.25f, twoThird}});	//3 - LeftBotRight

	//Top
	mesh.vertices.push_back({ {+size, +size, +size}, Math::Vector2{0.25f, third}});		//4 - TopBotLeft
	mesh.vertices.push_back({ {+size, +size, -size}, Math::Vector2{0.25f, 0.0f}});		//5 - TopTopLeft
	mesh.vertices.push_back({ {-size, +size, -size}, Math::Vector2{0.50f, 0.0f}});		//6 - TopTopRight
	mesh.vertices.push_back({ {-size, +size, +size}, Math::Vector2{0.50f, third}});		//7 - TopBotRight

	//Front
	mesh.vertices.push_back({ {-size, -size, +size}, Math::Vector2{0.50f, twoThird}});	//8 - FrontBotLeft
	mesh.vertices.push_back({ {-size, +size, +size}, Math::Vector2{0.50f, third}});		//9 - FrontTopLeft
	mesh.vertices.push_back({ {+size, +size, +size}, Math::Vector2{0.25f, third}});		//10 - FrontTopRight
	mesh.vertices.push_back({ {+size, -size, +size}, Math::Vector2{0.25f, twoThird}});	//11 - FrontBotRight

	//Bot
	mesh.vertices.push_back({ {+size, -size, +size}, Math::Vector2{0.25f, twoThird}});	//12 - BotBotLeft
	mesh.vertices.push_back({ {+size, -size, -size}, Math::Vector2{0.25f, 1.0f}});		//13 - BotTopLeft
	mesh.vertices.push_back({ {-size, -size, -size}, Math::Vector2{0.50f, 1.0f}});		//14 - BotTopRight
	mesh.vertices.push_back({ {-size, -size, +size}, Math::Vector2{0.50f, twoThird}});	//15 - BotBotRight

	//Right
	mesh.vertices.push_back({ {-size, -size, -size}, Math::Vector2{0.75f, twoThird}});	//16 - RightBotLeft
	mesh.vertices.push_back({ {-size, +size, -size}, Math::Vector2{0.75f, third}});		//17 - RightTopLeft
	mesh.vertices.push_back({ {-size, +size, +size}, Math::Vector2{0.50f, third}});		//18 - RightTopRight
	mesh.vertices.push_back({ {-size, -size, +size}, Math::Vector2{0.50f, twoThird}});	//19 - RightBotRight

	//Back
	mesh.vertices.push_back({ {-size, -size, -size}, Math::Vector2{0.75f, twoThird } });//20 - BackBotLeft
	mesh.vertices.push_back({ {-size, +size, -size}, Math::Vector2{0.75f, third } });	//21 - BackTopLeft
	mesh.vertices.push_back({ {+size, +size, -size}, Math::Vector2{1.00f, third } });	//22 - BackTopRight
	mesh.vertices.push_back({ {+size, -size, -size}, Math::Vector2{1.00f, twoThird } });//23 - BackBotRight

	mesh.indices = {2, 1, 0,	2, 0, 3,	//Left
					6, 5, 4,	6, 4, 7,	//Top
					8, 9, 10,	11, 8, 10,	//Front
					12,13,14,	15, 12, 14,	//Bot
					16,17,18,	19,16,18,	//Right 
					22, 21,20,	22, 20,23,	//Back
	};

	return mesh;
}

MeshPX MeshBuilder::CreateSpherePX(int slices, int rings, float radius)
{
	if (slices < 4) { slices = 4; }
	if (rings < 2) { rings = 2; }
	if (radius < 0.0f) { radius *= -1.0f; }

	MeshPX mesh;
	mesh.indices = {};

	const float sectorStep = Math::Constants::TwoPi / slices;
	float sectorAngle; //Radian

	const float sectorLeap = Math::Constants::Pi / rings;
	float sectorLeapAngle; //Radian

	for (int i = 0; i <= rings; ++i) {
		sectorLeapAngle = i * sectorLeap;
		for (int j = 0; j <= slices; ++j) {
			sectorAngle = j * sectorStep;
			const float x = radius * sin(sectorAngle) * sin(sectorLeapAngle);
			const float y = radius * cos(sectorLeapAngle);
			const float z = radius * cos(sectorAngle) * sin(sectorLeapAngle);

			const float u = (1.0f / slices) * j; //Slices - Horizontal
			const float v = (1.0f / rings) * i; //rings - vertical

			mesh.vertices.push_back({ {x, y, z}, Math::Vector2{u , v}});

			if (i != rings && j != slices) {
				mesh.indices.push_back(j + i + (i * slices) + 2 + slices);		//TopRight
				mesh.indices.push_back(j + i + (i * slices) + 1);				//Bottom Right
				mesh.indices.push_back(j + i + (i * slices));					//Bottom Left

				mesh.indices.push_back(j + i + (i * slices) + 2 + slices);		//TopRight
				mesh.indices.push_back(j + i + (i * slices));					//Bottom Left
				mesh.indices.push_back(j + i + (i * slices) + 1 + slices);		//TopLeft
			}
		}
	}

	return mesh;
}

MeshPX MeshBuilder::CreateSkydomePX(int slices, int rings, float radius)
{
	if (slices < 4) {
		slices = 4;
	}
	if (rings < 2) {
		rings = 2;
	}
	if (radius < 0.0f) {
		radius *= -1.0f;
	}

	MeshPX mesh;
	int colorIndex = 0;
	mesh.indices = {};

	const float sectorStep = Math::Constants::TwoPi / slices;
	float sectorAngle; //Radian

	const float sectorLeap = Math::Constants::Pi / rings;
	float sectorLeapAngle; //Radian

	for (int i = 0; i <= rings; ++i) {
		sectorLeapAngle = i * sectorLeap;
		for (int j = 0; j <= slices; ++j) {
			sectorAngle = j * sectorStep;
			const float x = radius * sin(sectorAngle) * sin(sectorLeapAngle);
			const float y = radius * cos(sectorLeapAngle);
			const float z = radius * cos(sectorAngle) * sin(sectorLeapAngle);

			const float u = (1.0f / slices) * j; //Slices - Horizontal
			const float v = (1.0f / rings) * i; //rings - vertical

			mesh.vertices.push_back({ {x, y, z}, Math::Vector2{u , v}});

			if (i != rings && j != slices) {
				mesh.indices.push_back(j + i + (i * slices));					//Bottom Left
				mesh.indices.push_back(j + i + (i * slices) + 1);				//Bottom Right
				mesh.indices.push_back(j + i + (i * slices) + 2 + slices);		//TopRight

				mesh.indices.push_back(j + i + (i * slices) + 1 + slices);		//TopLeft
				mesh.indices.push_back(j + i + (i * slices));					//Bottom Left
				mesh.indices.push_back(j + i + (i * slices) + 2 + slices);		//TopRight
			}
		}
	}

	return mesh;
}

MeshPX MeshBuilder::CreateScreenQuad()
{
	MeshPX mesh;
	mesh.vertices.push_back({ {-1.0f, -1.0f, 0.0f}, {Math::Vector2(0.0f, 1.0f)} }); // Left Bot
	mesh.vertices.push_back({ {-1.0f, +1.0f, 0.0f}, {Math::Vector2(0.0f, 0.0f)} }); // Left Top
	mesh.vertices.push_back({ {+1.0f, +1.0f, 0.0f}, {Math::Vector2(1.0f, 0.0f)} }); // Right Top
	mesh.vertices.push_back({ {+1.0f, -1.0f, 0.0f}, {Math::Vector2(1.0f, 1.0f)} }); // Right Bot
	mesh.indices = { 0, 1, 2, 0, 2, 3 };
	return mesh;
}

#pragma endregion

#pragma region ---Standard---

Mesh MeshBuilder::CreateCube()
{
	//24 unique vertices
	Mesh mesh;
	const float size = 0.5f;

	//Front
	mesh.vertices.push_back({ {-size, -size, +size}, Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2::YAxis });	//0 - FrontBotLeft
	mesh.vertices.push_back({ {-size, +size, +size}, Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2::Zero });	//1 - FrontTopLeft
	mesh.vertices.push_back({ {+size, +size, +size}, Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2::XAxis });	//2 - FrontTopRight
	mesh.vertices.push_back({ {+size, -size, +size}, Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2::One });		//3 - FrontBotRight

	//Back
	mesh.vertices.push_back({ {-size, -size, -size}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2::YAxis });	//4 - BackBotLeft
	mesh.vertices.push_back({ {-size, +size, -size}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2::Zero });	//5 - BackTopLeft
	mesh.vertices.push_back({ {+size, +size, -size}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2::XAxis });	//6 - BackTopRight
	mesh.vertices.push_back({ {+size, -size, -size}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2::One });	//7 - BackBotRight

	//Left
	mesh.vertices.push_back({ {+size, -size, -size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2::YAxis });	//8  - LeftBotLeft
	mesh.vertices.push_back({ {+size, +size, -size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2::Zero });	//9  - LeftTopLeft
	mesh.vertices.push_back({ {+size, +size, +size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2::XAxis });	//10 - LeftTopRight
	mesh.vertices.push_back({ {+size, -size, +size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2::One });		//11 - LeftBotRight

	//Right
	mesh.vertices.push_back({ {-size, -size, -size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2::YAxis });	//12 - RightBotLeft
	mesh.vertices.push_back({ {-size, +size, -size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2::Zero });	//13 - RightTopLeft
	mesh.vertices.push_back({ {-size, +size, +size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2::XAxis });	//14 - RightTopRight
	mesh.vertices.push_back({ {-size, -size, +size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2::One });		//15 - RightBotRight

	//Top
	mesh.vertices.push_back({ {+size, +size, +size}, Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2::YAxis });	//16 - TopBotLeft
	mesh.vertices.push_back({ {+size, +size, -size}, Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2::Zero });	//17 - TopTopLeft
	mesh.vertices.push_back({ {-size, +size, -size}, Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2::XAxis });	//18 - TopTopRight
	mesh.vertices.push_back({ {-size, +size, +size}, Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2::One });		//19 - TopBotRight

	//Bot
	mesh.vertices.push_back({ {+size, -size, +size}, -Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2::YAxis });	//20 - BotBotLeft
	mesh.vertices.push_back({ {+size, -size, -size}, -Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2::Zero });	//21 - BotTopLeft
	mesh.vertices.push_back({ {-size, -size, -size}, -Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2::XAxis });	//22 - BotTopRight
	mesh.vertices.push_back({ {-size, -size, +size}, -Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2::One });	//23 - BotBotRight

	mesh.indices = { 2, 1, 0,	2, 0, 3,	//Front
					4, 5, 6,	7, 4, 6,	//Back
					8, 9, 10,	11, 8,10,	//Left  
					14,13,12,	14,12,15,	//Right 
					16,17,18,	19,16,18,	//Top
					22,21,20,	22,20,23,	//Bot
	};

	return mesh;
}

Mesh MeshBuilder::CreateCrossCubeSkyBox()
{
	//24 unique vertices
	Mesh mesh;
	const float size = 0.5f;
	const float third = 1.0f / 3.0f;
	const float twoThird = 2.0f / 3.0f;

	//Front
	mesh.vertices.push_back({ {-size, -size, +size}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2{0.50f, twoThird} });	//0 - FrontBotLeft
	mesh.vertices.push_back({ {-size, +size, +size}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2{0.50f, third} });		//1 - FrontTopLeft
	mesh.vertices.push_back({ {+size, +size, +size}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2{0.25f, third} });		//2 - FrontTopRight
	mesh.vertices.push_back({ {+size, -size, +size}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2{0.25f, twoThird} });	//3 - FrontBotRight

	//Back
	mesh.vertices.push_back({ {-size, -size, -size}, Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2{0.75f, twoThird } });	//4 - BackBotLeft
	mesh.vertices.push_back({ {-size, +size, -size}, Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2{0.75f, third } });		//5 - BackTopLeft
	mesh.vertices.push_back({ {+size, +size, -size}, Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2{1.00f, third } });		//6 - BackTopRight
	mesh.vertices.push_back({ {+size, -size, -size}, Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2{1.00f, twoThird } });	//7 - BackBotRight

	//Left
	mesh.vertices.push_back({ {-size, -size, -size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.00f, twoThird} });	//8  - LeftBotLeft
	mesh.vertices.push_back({ {-size, +size, -size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.00f, third} });		//9  - LeftTopLeft
	mesh.vertices.push_back({ {-size, +size, +size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.25f, third} });		//10 - LeftTopRight
	mesh.vertices.push_back({ {-size, -size, +size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.25f, twoThird} });	//11 - LeftBotRight

	//Right
	mesh.vertices.push_back({ {+size, -size, -size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.75f, twoThird} });		//12 - RightBotLeft
	mesh.vertices.push_back({ {+size, +size, -size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.75f, third} });		//13 - RightTopLeft
	mesh.vertices.push_back({ {+size, +size, +size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.50f, third} });		//14 - RightTopRight
	mesh.vertices.push_back({ {+size, -size, +size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.50f, twoThird} });		//15 - RightBotRight

	//Top
	mesh.vertices.push_back({ {+size, +size, +size}, -Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2{0.25f, third} });	//16 - TopBotLeft
	mesh.vertices.push_back({ {+size, +size, -size}, -Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2{0.25f, 0.0f} });	//17 - TopTopLeft
	mesh.vertices.push_back({ {-size, +size, -size}, -Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2{0.50f, 0.0f} });	//18 - TopTopRight
	mesh.vertices.push_back({ {-size, +size, +size}, -Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2{0.50f, third} });	//19 - TopBotRight

	//Bot
	mesh.vertices.push_back({ {+size, -size, +size}, Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2{0.25f, twoThird} });		//20 - BotBotLeft
	mesh.vertices.push_back({ {+size, -size, -size}, Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2{0.25f, 1.0f} });			//21 - BotTopLeft
	mesh.vertices.push_back({ {-size, -size, -size}, Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2{0.50f, 1.0f} });			//22 - BotTopRight
	mesh.vertices.push_back({ {-size, -size, +size}, Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2{0.50f, twoThird} });		//23 - BotBotRight

	mesh.indices = {0, 1, 2,	3, 0, 2,	//Front
					6, 5, 4,	6, 4, 7,	//Back
					10, 9, 8,	10, 8,11,	//Left  
					12,13,14,	15,12,14,	//Right 
					18,17,16,	18,16,19,	//Top
					20,21,22,	23,20,22,	//Bot
	};

	return mesh;
}

std::vector<Mesh> MeshBuilder::CreateCubeSkyBox()
{
	std::vector<Mesh> meshes;
	meshes.reserve(6);

	//24 unique vertices
	Mesh mesh1, mesh2, mesh3, mesh4, mesh5, mesh6;
	const float size = 0.5f;

	//Front
	mesh1.vertices.push_back({ {-size, -size, +size}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2{0.0f, 1.0f} });	//0 - FrontBotLeft
	mesh1.vertices.push_back({ {-size, +size, +size}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2{0.0f, 0.0f} });	//1 - FrontTopLeft
	mesh1.vertices.push_back({ {+size, +size, +size}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2{1.0f, 0.0f} });	//2 - FrontTopRight
	mesh1.vertices.push_back({ {+size, -size, +size}, -Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2{1.0f, 1.0f} });	//3 - FrontBotRight

	//Back
	mesh2.vertices.push_back({ {-size, -size, -size}, Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2{1.0f, 1.0f } });	//0 - BackBotLeft
	mesh2.vertices.push_back({ {-size, +size, -size}, Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2{1.0f, 0.0f} });		//1 - BackTopLeft
	mesh2.vertices.push_back({ {+size, +size, -size}, Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2{0.0f, 0.0f} });		//2 - BackTopRight
	mesh2.vertices.push_back({ {+size, -size, -size}, Math::Vector3::ZAxis, Math::Vector3::XAxis, Math::Vector2{0.0f, 1.0f } });	//3 - BackBotRight

	//Left
	mesh3.vertices.push_back({ {-size, -size, -size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.0f, 1.0f} });	//0 - LeftBotLeft
	mesh3.vertices.push_back({ {-size, +size, -size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.0f, 0.0f} });	//1 - LeftTopLeft
	mesh3.vertices.push_back({ {-size, +size, +size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{1.0f, 0.0f} });	//2 - LeftTopRight
	mesh3.vertices.push_back({ {-size, -size, +size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{1.0f, 1.0f} });	//3 - LeftBotRight

	//Right
	mesh4.vertices.push_back({ {+size, -size, -size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{1.0f, 1.0f} });		//0 - RightBotLeft
	mesh4.vertices.push_back({ {+size, +size, -size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{1.0f, 0.0f} });		//1 - RightTopLeft
	mesh4.vertices.push_back({ {+size, +size, +size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.0f, 0.0f} });		//2 - RightTopRight
	mesh4.vertices.push_back({ {+size, -size, +size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.0f, 1.0f} });		//3 - RightBotRight

	//Top
	mesh5.vertices.push_back({ {+size, +size, +size}, -Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2{1.0f, 1.0f} });	//0 - TopBotLeft
	mesh5.vertices.push_back({ {+size, +size, -size}, -Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2{1.0f, 0.0f} });	//1 - TopTopLeft
	mesh5.vertices.push_back({ {-size, +size, -size}, -Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2{0.0f, 0.0f} });	//2 - TopTopRight
	mesh5.vertices.push_back({ {-size, +size, +size}, -Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2{0.0f, 1.0f} });	//3 - TopBotRight

	//Bot
	mesh6.vertices.push_back({ {+size, -size, +size}, Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2{1.0f, 0.0f} });		//0 - BotBotLeft
	mesh6.vertices.push_back({ {+size, -size, -size}, Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2{1.0f, 1.0f} });		//1 - BotTopLeft
	mesh6.vertices.push_back({ {-size, -size, -size}, Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2{0.0f, 1.0f} });		//2 - BotTopRight
	mesh6.vertices.push_back({ {-size, -size, +size}, Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2{0.0f, 0.0f} });		//3 - BotBotRight

	mesh1.indices = { 0, 1, 2,	 3, 0, 2 };
	mesh2.indices = { 2, 1, 0,	 2, 0, 3 };
	mesh3.indices = { 0, 1, 2,	 3, 0, 2 };
	mesh4.indices = { 2, 1, 0,	 2, 0, 3 };
	mesh5.indices = { 2, 1, 0,	 2, 0, 3 };
	mesh6.indices = { 0, 1, 2,	 3, 0, 2 };

	meshes.push_back(mesh1);
	meshes.push_back(mesh2);
	meshes.push_back(mesh3);
	meshes.push_back(mesh4);
	meshes.push_back(mesh5);
	meshes.push_back(mesh6);

	return meshes;
}

Mesh MeshBuilder::CreateCylinder(const Math::Cylinder& cylinder)
{
	return CreateCylinder(cylinder.slices, cylinder.rings);
}

Mesh MeshBuilder::CreateCylinder(int slices, int rings)
{
	return CreateSphere(slices, rings, 1.0f); // TODO:
}

Mesh MeshBuilder::CreateSphere(const Math::Sphere& sphere)
{
	return CreateSphere(sphere.slices, sphere.rings, sphere.radius);
}

Mesh MeshBuilder::CreateSphere(int slices, int rings, float radius)
{
	if (slices < 4) { slices = 4; }
	if (rings < 2) { rings = 2; }
	if (radius < 0.0f) { radius *= -1.0f; }

	Mesh mesh;

	const float sectorStep = Math::Constants::TwoPi / slices;
	float sectorAngle; //Radian

	const float sectorLeap = Math::Constants::Pi / rings;
	float sectorLeapAngle; //Radian

	for (int i = 0; i <= rings; ++i) {
		sectorLeapAngle = i * sectorLeap;
		for (int j = 0; j <= slices; ++j) {
			sectorAngle = j * sectorStep;
			const float x = radius * sin(sectorAngle) * sin(sectorLeapAngle);
			const float y = radius * cos(sectorLeapAngle);
			const float z = radius * cos(sectorAngle) * sin(sectorLeapAngle);
			const Math::Vector3 pos = { x, y, z };

			const float u = 1.0f - (1.0f / slices) * j; //Slices - Horizontal
			const float v = (1.0f / rings) * i; //rings - vertical
			const Math::Vector2 UV = { u, v };
			
			const Math::Vector3 norm = Math::Normalize(pos);
			const Math::Vector3 tang = Math::Normalize({ -z, 0.0f, x });
			mesh.vertices.push_back({ pos, norm, tang, UV });

			if (i != rings && j != slices) {
				mesh.indices.push_back(j + i + (i * slices) + 2 + slices);		//TopRight
				mesh.indices.push_back(j + i + (i * slices) + 1);				//Bottom Right
				mesh.indices.push_back(j + i + (i * slices));					//Bottom Left

				mesh.indices.push_back(j + i + (i * slices) + 2 + slices);		//TopRight
				mesh.indices.push_back(j + i + (i * slices));					//Bottom Left
				mesh.indices.push_back(j + i + (i * slices) + 1 + slices);		//TopLeft
			}
		}
	}

	return mesh;
}

Mesh MeshBuilder::CreateSkyDome(const Math::Sphere& sphere)
{
	return CreateSkyDome(sphere.slices, sphere.rings, sphere.radius);
}

Mesh MeshBuilder::CreateSkyDome(int slices, int rings, float radius)
{
	if (slices < 4) { slices = 4; }
	if (rings < 2) { rings = 2; }
	if (radius < 0.0f) { radius *= -1.0f; }

	Mesh mesh;

	const float sectorStep = Math::Constants::TwoPi / slices;
	float sectorAngle; //Radian

	const float sectorLeap = Math::Constants::Pi / rings;
	float sectorLeapAngle; //Radian

	for (int i = 0; i <= rings; ++i) {
		sectorLeapAngle = i * sectorLeap;
		for (int j = 0; j <= slices; ++j) {
			sectorAngle = j * sectorStep;
			const float x = radius * sin(sectorAngle) * sin(sectorLeapAngle);
			const float y = radius * cos(sectorLeapAngle);
			const float z = radius * cos(sectorAngle) * sin(sectorLeapAngle);
			const Math::Vector3 pos = { x, y, z };

			const float u = 1.0f - (1.0f / slices) * j; //Slices - Horizontal
			const float v = (1.0f / rings) * i; //rings - vertical
			const Math::Vector2 UV = { u, v };

			const Math::Vector3 norm = Math::Normalize(pos);
			const Math::Vector3 tang = Math::Normalize({ -z, 0.0f, x });
			mesh.vertices.push_back({ pos, norm, tang, UV });

			if (i != rings && j != slices) {
				mesh.indices.push_back(j + i + (i * slices));					//Bottom Left
				mesh.indices.push_back(j + i + (i * slices) + 1);				//Bottom Right
				mesh.indices.push_back(j + i + (i * slices) + 2 + slices);		//TopRight

				mesh.indices.push_back(j + i + (i * slices) + 1 + slices);		//TopLeft
				mesh.indices.push_back(j + i + (i * slices));					//Bottom Left
				mesh.indices.push_back(j + i + (i * slices) + 2 + slices);		//TopRight
			}
		}
	}

	return mesh;
}

Mesh MeshBuilder::CreatePlane(int columns, int rows, float spacing, bool flipVertices)
{
	return CreatePlane(columns, rows, Vector2(spacing, spacing), flipVertices);
}

Mesh MeshBuilder::CreatePlane(int columns, int rows, const Vector2& spacing, bool flipVertices, Pivot pivot)
{
	if (columns < 1) { columns = 1; }
	if (rows < 1) { rows = 1; }

	Mesh mesh;
	const float width = columns * spacing.x;
	const float height = rows * spacing.y;
	const float invCols = 1.0f / static_cast<float>(columns);
	const float invRows = 1.0f / static_cast<float>(rows);
	float xOffset = 0.0f;
	float zOffset = 0.0f;

	switch (pivot)
	{
	case Pivot::BottomLeft:
		xOffset = 0.0f;
		zOffset = 0.0f;
		break;
	case Pivot::Bottom:
		xOffset = width * 0.5f;
		zOffset = 0.0f;
		break;
	case Pivot::BottomRight:
		xOffset = width;
		zOffset = 0.0f;
		break;
	case Pivot::Left:
		xOffset = 0.0f;
		zOffset = height * 0.5f;
		break;
	case Pivot::Center:
		xOffset = width * 0.5f;
		zOffset = height * 0.5f;
		break;
	case Pivot::Right:
		xOffset = width;
		zOffset = height * 0.5f;
		break;
	case Pivot::TopLeft:
		xOffset = 0.0f;
		zOffset = height;
		break;
	case Pivot::Top:
		xOffset = width * 0.5f;
		zOffset = height;
		break;
	case Pivot::TopRight:
		xOffset = width;
		zOffset = height;
		break;
	}

	const int stride = columns + 1;

	mesh.vertices.reserve((columns + 1) * (rows + 1));
	mesh.indices.reserve(columns * rows * 6);

	for (int i = 0; i <= rows; ++i)
	{
		for (int j = 0; j <= columns; ++j)
		{
			Math::Vector3 pos =
			{
				j * spacing.x - xOffset,
				0.0f,
				i * spacing.y - zOffset
			};

			const float u = j * invCols;
			const float v = 1.0f - i * invRows;

			mesh.vertices.push_back(
				{
					pos,
					flipVertices ? -Vector3::YAxis : Vector3::YAxis,
					Vector3::XAxis,
					Vector2{ u, v }
				});

			if (i < rows && j < columns)
			{
				const uint32_t bottomLeft = j + i * stride;
				const uint32_t bottomRight = (j + 1) + i * stride;
				const uint32_t topLeft = j + (i + 1) * stride;
				const uint32_t topRight = (j + 1) + (i + 1) * stride;

				if (!flipVertices)
				{
					mesh.indices.push_back(topRight);
					mesh.indices.push_back(bottomRight);
					mesh.indices.push_back(bottomLeft);

					mesh.indices.push_back(topRight);
					mesh.indices.push_back(bottomLeft);
					mesh.indices.push_back(topLeft);
				}
				else
				{
					mesh.indices.push_back(bottomLeft);
					mesh.indices.push_back(bottomRight);
					mesh.indices.push_back(topRight);

					mesh.indices.push_back(topLeft);
					mesh.indices.push_back(bottomLeft);
					mesh.indices.push_back(topRight);
				}
			}
		}
	}

	return mesh;
}

Mesh MeshBuilder::CreateQuad(float width, float height)
{
	Mesh mesh;
	const float halfWidth = width * 0.5f;
	const float halfHeight = height * 0.5f;

	mesh.vertices =
	{
		{ { -halfWidth,  halfHeight, 0.0f }, Vector3::ZAxis,  Vector3::XAxis, { 0.0f, 0.0f } },
		{ {  halfWidth,  halfHeight, 0.0f }, Vector3::ZAxis,  Vector3::XAxis, { 1.0f, 0.0f } },
		{ {  halfWidth, -halfHeight, 0.0f }, Vector3::ZAxis,  Vector3::XAxis, { 1.0f, 1.0f } },
		{ { -halfWidth, -halfHeight, 0.0f }, Vector3::ZAxis,  Vector3::XAxis, { 0.0f, 1.0f } },
	};

	mesh.indices =
	{
		// Front
		0, 1, 2,
		0, 2, 3,

		// Back
		2, 1, 0,
		3, 2, 0
	};

	return mesh;
}

Mesh MeshBuilder::CreateGlass(float width, float height)
{
	Mesh mesh;
	const float halfWidth = width * 0.5f;
	const float halfHeight = height * 0.5f;

	// -halfWidth = left
	// halfWidth = right
	// -halfHeight = down
	// halfHeight = up

	mesh.vertices =
	{
		{ { -halfWidth,  halfHeight, 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.0f, 0.0f } },														// 0  = 0.00, 0.00
		{ {  Lerp(-halfWidth, halfWidth, 0.21f),  halfHeight, 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.21f, 0.00f } },							// 1  = 0.21, 0.00
		{ {  Lerp(-halfWidth, halfWidth, 0.27f), Lerp(halfHeight, -halfHeight, 0.14f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.27f, 0.14f } },	// 2  = 0.27, 0.14
		{ {  -halfWidth, Lerp(halfHeight, -halfHeight, 0.20f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.00f, 0.20f } },							// 3  = 0.00, 0.20
		{ {  Lerp(-halfWidth, halfWidth, 0.26f), Lerp(halfHeight, -halfHeight, 0.28f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.26f, 0.28f } },	// 4  = 0.26, 0.28
		{ {  Lerp(-halfWidth, halfWidth, 0.32f), Lerp(halfHeight, -halfHeight, 0.27f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.32f, 0.27f } },	// 5  = 0.32, 0.27
																																						  
		{ {  Lerp(-halfWidth, halfWidth, 0.55f),  halfHeight, 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.55f, 0.00f } },							// 6  = 0.55, 0.00
		{ {  Lerp(-halfWidth, halfWidth, 0.52f), Lerp(halfHeight, -halfHeight, 0.10f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.52f, 0.10f } },	// 7  = 0.52, 0.10
		{ {  Lerp(-halfWidth, halfWidth, 0.47f), Lerp(halfHeight, -halfHeight, 0.22f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.47f, 0.22f } },	// 8  = 0.47, 0.22

		{ {  Lerp(-halfWidth, halfWidth, 0.82f),  halfHeight, 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.82f, 0.00f } },							// 9  = 0.82, 0.00
		{ {  Lerp(-halfWidth, halfWidth, 0.74f), Lerp(halfHeight, -halfHeight, 0.21f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.74f, 0.21f } },	// 10 = 0.74, 0.21
		{ {  Lerp(-halfWidth, halfWidth, 0.64f), Lerp(halfHeight, -halfHeight, 0.31f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.64f, 0.31f } },	// 11 = 0.64, 0.31
		{ {  Lerp(-halfWidth, halfWidth, 0.62f), Lerp(halfHeight, -halfHeight, 0.32f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.62f, 0.32f } },	// 12 = 0.62, 0.32

		{ {  halfWidth, halfHeight, 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 1.00f, 0.00f } },														// 13 = 1.00, 0.00
		{ {  halfWidth, Lerp(halfHeight, -halfHeight, 0.12f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 1.00f, 0.12f } },							// 14 = 1.00, 0.12
		{ {  Lerp(-halfWidth, halfWidth, 0.77f), Lerp(halfHeight, -halfHeight, 0.21f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.77f, 0.21f } },	// 15 = 0.77, 0.21
		
		{ { -halfWidth, Lerp(halfHeight, -halfHeight, 0.54f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.00f, 0.54f } },							// 16 = 0.00, 0.54
		{ {  Lerp(-halfWidth, halfWidth, 0.22f), Lerp(halfHeight, -halfHeight, 0.49f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.22f, 0.49f } },	// 17 = 0.22, 0.49

		{ {  Lerp(-halfWidth, halfWidth, 0.37f), Lerp(halfHeight, -halfHeight, 0.43f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.37f, 0.43f } },	// 18 = 0.37, 0.43
		{ {  Lerp(-halfWidth, halfWidth, 0.44f), Lerp(halfHeight, -halfHeight, 0.54f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.44f, 0.54f } },	// 19 = 0.44, 0.54
		{ {  Lerp(-halfWidth, halfWidth, 0.38f), Lerp(halfHeight, -halfHeight, 0.62f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.38f, 0.62f } },	// 20 = 0.38, 0.62

		{ {  Lerp(-halfWidth, halfWidth, 0.46f), Lerp(halfHeight, -halfHeight, 0.39f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.46f, 0.39f } },	// 21 = 0.46, 0.39

		{ {  Lerp(-halfWidth, halfWidth, 0.52f), Lerp(halfHeight, -halfHeight, 0.38f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.52f, 0.38f } },	// 22 = 0.53, 0.38

		{ {  Lerp(-halfWidth, halfWidth, 0.56f), Lerp(halfHeight, -halfHeight, 0.53f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.56f, 0.53f } },	// 23 = 0.56, 0.53

		{ {  Lerp(-halfWidth, halfWidth, 0.49f), Lerp(halfHeight, -halfHeight, 0.64f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.49f, 0.64f } },	// 24 = 0.49, 0.64

		{ {  Lerp(-halfWidth, halfWidth, 0.62f), Lerp(halfHeight, -halfHeight, 0.55f), 0.0f }, Vector3::ZAxis, Vector3::XAxis, { 0.62f, 0.55f } },	// 25 = 0.62, 0.55
	};

	mesh.indices =
	{
		// 1
		0, 1, 2,
		0, 2, 3,
		3, 2, 4,
		4, 2, 5,

		// 2
		1, 6, 2,
		2, 6, 7,
		2, 7, 8,

		// 3
		6, 9, 7,
		7, 9, 10,
		7, 10, 11,
		7, 11, 12,

		// 4
		9, 13, 14,
		9, 14, 15,
		9, 15, 10,

		//5
		3, 4, 16,
		16, 4, 17,

		//6
		17, 4, 5,
		17, 5, 18,
		17, 18, 19,
		17, 19, 20,

		// 7
		5, 2, 8,
		5, 8, 21, 
		5, 21, 18,

		// 8
		8, 7, 12,
		8, 12, 22,
		8, 22, 21,

		// 9
		18, 21, 22,
		18, 22, 23,
		18, 23, 19,
		19, 23, 24,

		// 10
		22, 12, 23,
		23, 12, 11,
		23, 11, 25,
	};

	return mesh;
}


//Mesh MeshBuilder::CreatePlane(int columns, int rows, const Vector2& spacing, bool flipVertices)
//{
//	if (columns < 1) { columns = 1; }
//	if (rows < 1) { rows = 1; }
//
//	Mesh mesh;
//	const float width = columns * spacing.x;
//	const float height = rows * spacing.y;
//	const float xOffSet = width * 0.5f;
//	const float zOffSet = height * 0.5f;
//	const float invCols = 1.0f / static_cast<float>(columns);
//	const float invRows = 1.0f / static_cast<float>(rows);
//
//	mesh.indices = {};
//
//	for (int i = 0; i <= rows; ++i) {
//		for (int j = 0; j <= columns; ++j) {
//			// pos, normal, tangent, uv
//			Math::Vector3 pos = { j * spacing.x - xOffSet, 0.0f, i * spacing.y - zOffSet };
//			const float u = j * invCols; // 1.0f - (1.0f / columns) * j;
//			const float v = 1.0f - i * invRows; // 1.0f - ((1.0f / rows) * i);
//
//			mesh.vertices.push_back({ pos, Vector3::YAxis, Vector3::XAxis, Vector2{u, v} });
//
//			if (!flipVertices) {
//				if (i != rows && j != columns) {
//					mesh.indices.push_back(j + i + (i * columns) + 2 + columns);	//TopRight
//					mesh.indices.push_back(j + i + (i * columns) + 1);				//Bottom Right
//					mesh.indices.push_back(j + i + (i * columns));					//Bottom Left
//
//					mesh.indices.push_back(j + i + (i * columns) + 2 + columns);	//TopRight
//					mesh.indices.push_back(j + i + (i * columns));					//Bottom Left
//					mesh.indices.push_back(j + i + (i * columns) + 1 + columns);	//TopLeft
//				}
//			}
//			else {
//				if (i != rows && j != columns) {
//					mesh.indices.push_back(j + i + (i * columns));					//Bottom Left
//					mesh.indices.push_back(j + i + (i * columns) + 1);				//Bottom Right
//					mesh.indices.push_back(j + i + (i * columns) + 2 + columns);	//TopRight
//
//					mesh.indices.push_back(j + i + (i * columns) + 1 + columns);	//TopLeft
//					mesh.indices.push_back(j + i + (i * columns));					//Bottom Left
//					mesh.indices.push_back(j + i + (i * columns) + 2 + columns);	//TopRight
//				}
//			}
//		}
//	}
//	return mesh;
//}

#pragma endregion