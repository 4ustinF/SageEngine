#include "Precompiled.h"
#include "MeshBuilder.h"

using namespace SAGE;
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
	for (int i = 0; i < slices; ++i) {
		mesh.indices.push_back(mesh.vertices.size() - 2); //TopMiddle
		mesh.indices.push_back(mesh.vertices.size() - (3 + i));
		mesh.indices.push_back(mesh.vertices.size() - (4 + i));

		mesh.indices.push_back(mesh.vertices.size() - 1); //BotMiddle
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

Mesh MeshBuilder::CreateSkyBox()
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
	mesh.vertices.push_back({ {+size, -size, -size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.00f, twoThird} });	//8  - LeftBotLeft
	mesh.vertices.push_back({ {+size, +size, -size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.00f, third} });		//9  - LeftTopLeft
	mesh.vertices.push_back({ {+size, +size, +size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.25f, third} });		//10 - LeftTopRight
	mesh.vertices.push_back({ {+size, -size, +size}, -Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.25f, twoThird} });	//11 - LeftBotRight

	//Right
	mesh.vertices.push_back({ {-size, -size, -size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.75f, twoThird} });		//12 - RightBotLeft
	mesh.vertices.push_back({ {-size, +size, -size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.75f, third} });		//13 - RightTopLeft
	mesh.vertices.push_back({ {-size, +size, +size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.50f, third} });		//14 - RightTopRight
	mesh.vertices.push_back({ {-size, -size, +size}, Math::Vector3::XAxis, Math::Vector3::ZAxis, Math::Vector2{0.50f, twoThird} });		//15 - RightBotRight

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
	if (columns < 1) { columns = 1; }
	if (rows < 1) { rows = 1; }

	Mesh mesh;
	//const float spacing = 1.0f;
	const float xOffSet = columns * 0.5f;
	const float zOffSet = rows * 0.5f;

	mesh.indices = {};

	for (int i = 0; i <= rows; ++i) {
		for (int j = 0; j <= columns; ++j) {
			// pos, normal, tangent, uv
			Math::Vector3 pos = { j * spacing - xOffSet, 0.0f, i * spacing - zOffSet };
			const float u = 1.0f - (1.0f / columns) * j;
			const float v = (1.0f / rows) * i;

			mesh.vertices.push_back({ pos, Math::Vector3::YAxis, Math::Vector3::XAxis, Math::Vector2{u, v} });

			if (!flipVertices) {
				if (i != rows && j != columns) {
					mesh.indices.push_back(j + i + (i * columns) + 2 + columns);	//TopRight
					mesh.indices.push_back(j + i + (i * columns) + 1);				//Bottom Right
					mesh.indices.push_back(j + i + (i * columns));					//Bottom Left

					mesh.indices.push_back(j + i + (i * columns) + 2 + columns);	//TopRight
					mesh.indices.push_back(j + i + (i * columns));					//Bottom Left
					mesh.indices.push_back(j + i + (i * columns) + 1 + columns);	//TopLeft
				}
			}
			else {
				if (i != rows && j != columns) {
					mesh.indices.push_back(j + i + (i * columns));					//Bottom Left
					mesh.indices.push_back(j + i + (i * columns) + 1);				//Bottom Right
					mesh.indices.push_back(j + i + (i * columns) + 2 + columns);	//TopRight

					mesh.indices.push_back(j + i + (i * columns) + 1 + columns);	//TopLeft
					mesh.indices.push_back(j + i + (i * columns));					//Bottom Left
					mesh.indices.push_back(j + i + (i * columns) + 2 + columns);	//TopRight
				}
			}
		}
	}
	return mesh;
}

#pragma endregion