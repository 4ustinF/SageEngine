#include "Precompiled.h"
#include "SimpleDraw.h"

#include "BlendState.h"
#include "Camera.h"
#include "ConstantBuffer.h"
#include "MeshBuffer.h"
#include "PixelShader.h"
#include "Transform.h"
#include "VertexShader.h"
#include "VertexTypes.h"

using namespace SAGE;
using namespace SAGE::Math;
using namespace SAGE::Graphics;

namespace
{
	class SimpleDrawImpl
	{
	public:
		void Initiazlie(uint32_t maxVertexCount);
		void Terminate();

		void AddLine(const Math::Vector3& v0, const Math::Vector3& v1, Color color);
		void AddFace(const Math::Vector3& v0, const Math::Vector3& v1, const Math::Vector3& v2, Color color);

		void Render(const Camera& camera);

	private:
		VertexShader mVertexShader;
		PixelShader mPixelShader;
		ConstantBuffer mConstantBuffer;
		MeshBuffer mMeshBuffer;
		BlendState mAlphaBlendState;

		std::unique_ptr<VertexPC[]> mLineVertices;
		std::unique_ptr<VertexPC[]> mFaceVertices;
		uint32_t mLineVertexCount = 0;
		uint32_t mFaceVertexCount = 0;
		uint32_t mMaxVertexCount = 0;
	};

	void SimpleDrawImpl::Initiazlie(uint32_t maxVertexCount)
	{
		mVertexShader.Initialize<VertexPC>(L"../../Assets/Shaders/SimpleDraw.fx");
		mPixelShader.Initialize(L"../../Assets/Shaders/SimpleDraw.fx");
		mConstantBuffer.Initialize(sizeof(Math::Matrix4));
		mMeshBuffer.Initialize(nullptr, sizeof(VertexPC), maxVertexCount);
		mAlphaBlendState.Initialize(BlendState::Mode::AlphaBlend);

		mLineVertices = std::make_unique<VertexPC[]>(maxVertexCount);
		mFaceVertices = std::make_unique<VertexPC[]>(maxVertexCount);
		mLineVertexCount = 0;
		mFaceVertexCount = 0;
		mMaxVertexCount = maxVertexCount;
	}

	void SimpleDrawImpl::Terminate()
	{
		mAlphaBlendState.Terminate();
		mMeshBuffer.Terminate();
		mConstantBuffer.Terminate();
		mPixelShader.Terminate();
		mVertexShader.Terminate();
	}

	void SimpleDrawImpl::AddLine(const Math::Vector3& v0, const Math::Vector3& v1, const Color color)
	{
		if (mLineVertexCount + 2 <= mMaxVertexCount) {
			mLineVertices[mLineVertexCount++] = VertexPC{v0, color};
			mLineVertices[mLineVertexCount++] = VertexPC{v1, color};
		}
	}

	void SimpleDrawImpl::AddFace(const Math::Vector3& v0, const Math::Vector3& v1, const Math::Vector3& v2, const Color color)
	{
		if (mFaceVertexCount + 3 <= mMaxVertexCount) {
			mFaceVertices[mFaceVertexCount++] = VertexPC{ v0, color };
			mFaceVertices[mFaceVertexCount++] = VertexPC{ v1, color };
			mFaceVertices[mFaceVertexCount++] = VertexPC{ v2, color };
		}
	}

	void SimpleDrawImpl::Render(const Camera& camera)
	{
		auto matView = camera.GetViewMatrix();
		auto matProj = camera.GetProjectionMatrix();
		auto transform = Math::Transpose(matView * matProj);
		mConstantBuffer.Update(&transform);
		mConstantBuffer.BindVS(0);

		mVertexShader.Bind();
		mPixelShader.Bind();

		mAlphaBlendState.Set();

		mMeshBuffer.Update(mLineVertices.get(), mLineVertexCount);
		mMeshBuffer.SetToplogy(MeshBuffer::Topology::Lines);
		mMeshBuffer.Render();

		mMeshBuffer.Update(mFaceVertices.get(), mFaceVertexCount);
		mMeshBuffer.SetToplogy(MeshBuffer::Topology::Triangles);
		mMeshBuffer.Render();

		BlendState::ClearState();

		mLineVertexCount = 0;
		mFaceVertexCount = 0;
	}

	std::unique_ptr<SimpleDrawImpl> sInstance;
}

void SimpleDraw::StaticInitialize(uint32_t maxVertexCount)
{
	sInstance = std::make_unique<SimpleDrawImpl>();
	sInstance->Initiazlie(maxVertexCount);
}

void SimpleDraw::StaticTerminate()
{
	sInstance->Terminate();
	sInstance.reset();
}

void SimpleDraw::AddLine(const Math::Vector3& v0, const Math::Vector3& v1, const Color color)
{
	sInstance->AddLine(v0, v1, color);
}

void SimpleDraw::AddFace(const Math::Vector3& v0, const Math::Vector3& v1, const Math::Vector3& v2, const Color color)
{
	sInstance->AddLine(v0, v1, color);
	sInstance->AddLine(v1, v2, color);
	sInstance->AddLine(v2, v0, color);
}

void SimpleDraw::AddFilledFace(const Math::Vector3& v0, const Math::Vector3& v1, const Math::Vector3& v2, const Color color)
{
	sInstance->AddFace(v0, v1, v2, color);
}

void SimpleDraw::AddAABB(const Math::AABB& aabb, Color color)
{
	AddAABB(aabb.center, aabb.extend, color);
}

void SimpleDraw::AddAABB(const Math::Vector3& center, const Math::Vector3& extend, Color color)
{
	AddAABB(
		center.x - extend.x,
		center.x + extend.x,
		center.y - extend.y,
		center.y + extend.y,
		center.z - extend.z,
		center.z + extend.z,
		color );
}

void SimpleDraw::AddAABB(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, Color color)
{
	const Math::Vector3 NNN = { minX, minY, minZ };
	const Math::Vector3 NNP = { minX, minY, maxZ };
	const Math::Vector3 NPN = { minX, maxY, minZ };
	const Math::Vector3 NPP = { minX, maxY, maxZ };
	const Math::Vector3 PNN = { maxX, minY, minZ };
	const Math::Vector3 PNP = { maxX, minY, maxZ };
	const Math::Vector3 PPN = { maxX, maxY, minZ };
	const Math::Vector3 PPP = { maxX, maxY, maxZ };

	//Bot
	AddLine(NNN, NNP, color); //BotLeft	 -> TopLeft
	AddLine(NNP, PNP, color); //TopLeft	 -> TopRight
	AddLine(PNP, PNN, color); //TopRight -> BotRight
	AddLine(PNN, NNN, color); //BotRight -> BotLeft

	//Mid
	AddLine(NNN, NPN, color); //Bot BotLeft  -> Top BotLeft
	AddLine(NNP, NPP, color); //Bot TopLeft  -> Top TopLeft
	AddLine(PNP, PPP, color); //Bot TopRight -> Top TopRight
	AddLine(PNN, PPN, color); //Bot BotRight -> Top BotRight

	//Top
	AddLine(NPN, NPP, color); //BotLeft  -> TopLeft
	AddLine(NPP, PPP, color); //TopLeft  -> TopRight
	AddLine(PPP, PPN, color); //TopRight -> BotRight
	AddLine(PPN, NPN, color); //BotRight -> BotLeft
}

void SimpleDraw::AddFilledAABB(const Math::AABB& aabb, Color color)
{
	AddFilledAABB(aabb.center, aabb.extend, color);
}
					
void SimpleDraw::AddFilledAABB(const Math::Vector3& center, const Math::Vector3& extend, Color color)
{
	AddFilledAABB(
		center.x - extend.x,
		center.x + extend.x,
		center.y - extend.y,
		center.y + extend.y,
		center.z - extend.z,
		center.z + extend.z,
		color);
}
					
void SimpleDraw::AddFilledAABB(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, Color color)
{
	const Math::Vector3 NNN = { minX, minY, minZ };
	const Math::Vector3 NNP = { minX, minY, maxZ };
	const Math::Vector3 NPN = { minX, maxY, minZ };
	const Math::Vector3 NPP = { minX, maxY, maxZ };
	const Math::Vector3 PNN = { maxX, minY, minZ };
	const Math::Vector3 PNP = { maxX, minY, maxZ };
	const Math::Vector3 PPN = { maxX, maxY, minZ };
	const Math::Vector3 PPP = { maxX, maxY, maxZ };

	sInstance->AddFace(NNN, NPN, PPN, color);
	sInstance->AddFace(NNN, PPN, PNN, color);

	sInstance->AddFace(NNP, PNP, PPP, color);
	sInstance->AddFace(NPP, NNP, PPP, color);

	sInstance->AddFace(NNN, NNP, NPN, color);
	sInstance->AddFace(NNP, NPP, NPN, color);

	sInstance->AddFace(PNN, PPN, PNP, color);
	sInstance->AddFace(PPN, PPP, PNP, color);

	sInstance->AddFace(NPN, NPP, PPP, color);
	sInstance->AddFace(PPP, PPN, NPN, color);

	sInstance->AddFace(PNP, NNP, NNN, color);
	sInstance->AddFace(PNN, PNP, NNN, color);
}

void SimpleDraw::AddOBB(const Math::OBB& obb, const Color color)
{
	Math::Vector3 points[] =
	{
		{ -1.0f, -1.0f, -1.0f },
		{ -1.0f, +1.0f, -1.0f },
		{ +1.0f, +1.0f, -1.0f },
		{ +1.0f, -1.0f, -1.0f },
		{ -1.0f, -1.0f, +1.0f },
		{ -1.0f, +1.0f, +1.0f },
		{ +1.0f, +1.0f, +1.0f },
		{ +1.0f, -1.0f, +1.0f }
	};

	const Math::Matrix4 transform = Transform{obb.center, obb.rotation, obb.extend}.GetMatrix4();

	for (auto& point : points) {
		point = Math::TransformCoord(point, transform);
	}

	AddLine(points[0], points[1], color);
	AddLine(points[1], points[2], color);
	AddLine(points[2], points[3], color);
	AddLine(points[3], points[0], color);

	AddLine(points[4], points[5], color);
	AddLine(points[5], points[6], color);
	AddLine(points[6], points[7], color);
	AddLine(points[7], points[4], color);

	AddLine(points[0], points[4], color);
	AddLine(points[1], points[5], color);
	AddLine(points[2], points[6], color);
	AddLine(points[3], points[7], color);
}

void SimpleDraw::AddOBB(const Math::Vector3& center, const Math::Vector3& extend, const Math::Quaternion& rotation, const Color color)
{
	const Math::OBB obb = {center, extend, rotation};
	AddOBB(obb, color);
}

void SimpleDraw::AddFilledOBB(const Math::OBB& obb, const Color color)
{
	Math::Vector3 points[] =
	{
		{ -1.0f, -1.0f, -1.0f },
		{ -1.0f, +1.0f, -1.0f },
		{ +1.0f, +1.0f, -1.0f },
		{ +1.0f, -1.0f, -1.0f },
		{ -1.0f, -1.0f, +1.0f },
		{ -1.0f, +1.0f, +1.0f },
		{ +1.0f, +1.0f, +1.0f },
		{ +1.0f, -1.0f, +1.0f }
	};

	const Math::Matrix4 transform = Transform{ obb.center, obb.rotation, obb.extend }.GetMatrix4();

	for (auto& point : points) {
		point = Math::TransformCoord(point, transform);
	}

	sInstance->AddFace(points[0], points[1], points[2], color);
	sInstance->AddFace(points[0], points[2], points[3], color);

	sInstance->AddFace(points[6], points[5], points[4], color);
	sInstance->AddFace(points[7], points[6], points[4], color);

	sInstance->AddFace(points[4], points[5], points[0], color);
	sInstance->AddFace(points[5], points[1], points[0], color);

	sInstance->AddFace(points[3], points[2], points[6], color);
	sInstance->AddFace(points[6], points[7], points[3], color);

	sInstance->AddFace(points[1], points[5], points[2], color);
	sInstance->AddFace(points[5], points[6], points[2], color);

	sInstance->AddFace(points[3], points[4], points[0], color);
	sInstance->AddFace(points[3], points[7], points[4], color);
}

void SimpleDraw::AddFilledOBB(const Math::Vector3& center, const Math::Vector3& extend, const Math::Quaternion& rotation, const Color color)
{
	AddFilledOBB({center, extend, rotation }, color);
}

void SimpleDraw::AddCapsule(const Math::Vector3& center, int ringSegments, int arcSegments, float radius, float height, const Math::Quaternion& rotation, const Color& color)
{
	ringSegments = Max(4, ringSegments);
	arcSegments = Max(2, arcSegments);
	const float halfHeight = Max(0.05f, height * 0.5f - radius);

	// Local-space capsule oriented along +Y, caps at +halfHeight / -halfHeight
	std::vector<Vector3> topRing(ringSegments);
	std::vector<Vector3> bottomRing(ringSegments);
	for (int i = 0; i < ringSegments; ++i)
	{
		const float theta = (float)i / (float)ringSegments * Constants::TwoPi;
		const float x = radius * cosf(theta);
		const float z = radius * sinf(theta);
		topRing[i] = { x, halfHeight, z };
		bottomRing[i] = { x, -halfHeight, z };
	}

	// 4 arcs per cap: +X/-X in the XY plane, +Z/-Z in the ZY plane, each a
	// half-circle (0..pi) from the ring out to the pole and back down.
	std::vector<Vector3> topArcXY(arcSegments + 1);
	std::vector<Vector3> topArcNegXY(arcSegments + 1);
	std::vector<Vector3> topArcZY(arcSegments + 1);
	std::vector<Vector3> topArcNegZY(arcSegments + 1);
	std::vector<Vector3> botArcXY(arcSegments + 1);
	std::vector<Vector3> botArcNegXY(arcSegments + 1);
	std::vector<Vector3> botArcZY(arcSegments + 1);
	std::vector<Vector3> botArcNegZY(arcSegments + 1);
	for (int i = 0; i <= arcSegments; ++i)
	{
		const float theta = (float)i / (float)arcSegments * Constants::HalfPi; // 0..90 deg
		const float c = radius * cosf(theta);
		const float s = radius * sinf(theta);

		topArcXY[i] = { c,  halfHeight + s, 0.0f };
		topArcNegXY[i] = { -c,  halfHeight + s, 0.0f };
		topArcZY[i] = { 0.0f,  halfHeight + s,  c };
		topArcNegZY[i] = { 0.0f,  halfHeight + s, -c };

		botArcXY[i] = { c, -halfHeight - s, 0.0f };
		botArcNegXY[i] = { -c, -halfHeight - s, 0.0f };
		botArcZY[i] = { 0.0f, -halfHeight - s,  c };
		botArcNegZY[i] = { 0.0f, -halfHeight - s, -c };
	}

	// Transform everything into world space
	const Matrix4 transform = Transform(center, rotation, Vector3::One).GetMatrix4();
	auto xformArray = [&transform](std::vector<Vector3>& arr, int count)
		{
			for (int i = 0; i < count; ++i)
			{
				arr[i] = TransformCoord(arr[i], transform);
			}
		};
	xformArray(topRing, ringSegments);
	xformArray(bottomRing, ringSegments);
	xformArray(topArcXY, arcSegments + 1);
	xformArray(topArcNegXY, arcSegments + 1);
	xformArray(topArcZY, arcSegments + 1);
	xformArray(topArcNegZY, arcSegments + 1);
	xformArray(botArcXY, arcSegments + 1);
	xformArray(botArcNegXY, arcSegments + 1);
	xformArray(botArcZY, arcSegments + 1);
	xformArray(botArcNegZY, arcSegments + 1);

	// Rings
	for (int i = 0; i < ringSegments; ++i)
	{
		const int next = (i + 1) % ringSegments;
		AddLine(topRing[i], topRing[next], color);
		AddLine(bottomRing[i], bottomRing[next], color);
	}

	// 4 side lines connecting the rings (cardinal directions)
	const int step = ringSegments / 4;
	for (int i = 0; i < ringSegments; i += step)
	{
		AddLine(topRing[i], bottomRing[i], color);
	}

	// Hemisphere arcs
	auto drawArc = [color](std::vector<Vector3>& arc, int count)
		{
			for (int i = 0; i < count; ++i)
			{
				AddLine(arc[i], arc[i + 1], color);
			}
		};
	drawArc(topArcXY, arcSegments);
	drawArc(topArcNegXY, arcSegments);
	drawArc(topArcZY, arcSegments);
	drawArc(topArcNegZY, arcSegments);
	drawArc(botArcXY, arcSegments);
	drawArc(botArcNegXY, arcSegments);
	drawArc(botArcZY, arcSegments);
	drawArc(botArcNegZY, arcSegments);
}

void SimpleDraw::AddFilledCapsule(const Math::Vector3& center, int ringSegments, int arcSegments, float radius, float height, const Math::Quaternion& rotation, const Color& color)
{
	ringSegments = Max(4, ringSegments);
	arcSegments = Max(2, arcSegments);

	const float halfHeight = Max(0.05f, height * 0.5f - radius);
	const Math::Matrix4 transform = Transform(center, rotation, Math::Vector3::One).GetMatrix4();

	// Each ring runs around the capsule's local Y axis.
	// Rings are stored from bottom to top.
	std::vector<std::vector<Math::Vector3>> rings;
	rings.reserve(arcSegments * 2);

	auto createRing = [&rings, ringSegments, transform](float y, float ringRadius)
	{
		std::vector<Math::Vector3> ring(ringSegments);
		for (int i = 0; i < ringSegments; ++i)
		{
			const float theta = static_cast<float>(i) / static_cast<float>(ringSegments) * Constants::TwoPi;
			Math::Vector3 point(ringRadius * cosf(theta), y, ringRadius * sinf(theta));
			ring[i] = Math::TransformCoord(point, transform);
		}

		rings.emplace_back(std::move(ring));
	};

	// Bottom hemisphere, starting near the bottom pole and ending
	// at the bottom cylinder ring.
	for (int arc = arcSegments - 1; arc >= 0; --arc)
	{
		const float angle = static_cast<float>(arc) / static_cast<float>(arcSegments) * Constants::HalfPi;
		const float ringRadius = radius * cosf(angle);
		const float y = -halfHeight - radius * sinf(angle);
		createRing(y, ringRadius);
	}

	// Top hemisphere, starting at the top cylinder ring and ending
	// near the top pole.
	for (int arc = 0; arc < arcSegments; ++arc)
	{
		const float angle = static_cast<float>(arc) / static_cast<float>(arcSegments) * Constants::HalfPi;
		const float ringRadius = radius * cosf(angle);
		const float y = halfHeight + radius * sinf(angle);
		createRing(y, ringRadius);
	}

	// Connect adjacent rings.
	for (size_t ringIndex = 0; ringIndex + 1 < rings.size(); ++ringIndex)
	{
		const auto& lowerRing = rings[ringIndex];
		const auto& upperRing = rings[ringIndex + 1];

		for (int i = 0; i < ringSegments; ++i)
		{
			const int next = (i + 1) % ringSegments;
			sInstance->AddFace(lowerRing[i], upperRing[i], upperRing[next], color);
			sInstance->AddFace(lowerRing[i], upperRing[next], lowerRing[next], color);
		}
	}

	// Transform the two capsule poles.
	Vector3 bottomPole = TransformCoord({ 0.0f, -halfHeight - radius, 0.0f }, transform);
	Vector3 topPole = TransformCoord({ 0.0f, halfHeight + radius, 0.0f }, transform);

	const auto& bottomRing = rings.front();
	const auto& topRing = rings.back();

	// Close the hemisphere poles using triangle fans.
	for (int i = 0; i < ringSegments; ++i)
	{
		const int next = (i + 1) % ringSegments;
		sInstance->AddFace(bottomPole, bottomRing[i], bottomRing[next], color);
		sInstance->AddFace(topPole, topRing[next], topRing[i], color);
	}
}

//void SimpleDraw::AddCapsule(const Math::Vector3& pointA, const Math::Vector3& pointB, float radius, const Color& color)
//{
//	// Convenience overload: capsule defined by its two segment endpoints (like Unity's).
//	const Vector3 axis = pointB - pointA;
//	const float height = Magnitude(axis);
//	const Vector3 center = (pointA + pointB) * 0.5f;
//	const float halfHeight = height * 0.5f;
//
//	// Build a rotation that maps local +Y to the capsule's axis direction.
//	const Vector3 up = (height > 0.0001f) ? (axis / height) : Vector3::YAxis;
//	const Quaternion rotation = Quaternion::RotationFromTo(Vector3::YAxis, up);
//
//	AddCapsule(center, radius, halfHeight, rotation, color);
//}

void SimpleDraw::AddCylinder(const Math::Cylinder& cylinder, Color color, bool hasLid)
{
	AddCylinder(cylinder.center, cylinder.slices, cylinder.rings, cylinder.radius, cylinder.height, color, hasLid ? cylinder.hasLid : hasLid);
}

void SimpleDraw::AddCylinder(Math::Vector3 center, int slices, int rings, float radius, float height, Color color, bool hasLid)
{
	if (slices < 3) { slices = 3; }
	if (rings < 1) { rings = 1; }
	if (radius < 0.0f) { radius *= -1.0f; }
	if (height < 0.0f) { height *= -1.0f; }

	Math::Vector3 v0, v1 = {};
	std::vector<Math::Vector3> vertices;

	const float spacing = height / rings;
	const float yOffSet = height * 0.5f;

	const float sectorStep = Math::Constants::TwoPi / slices;
	float sectorAngle; //Radian

	for (int i = 0; i <= rings; ++i) {
		for (int j = 0; j <= slices; ++j) {
			sectorAngle = j * sectorStep;

			const float x = radius * cos(sectorAngle) + center.x;
			const float y = i * spacing - yOffSet + center.y;
			const float z = radius * sin(sectorAngle) + center.z;

			v1 = { x, y, z };
			vertices.push_back(v1);

			//Draws Horizontal Lines
			if (j != 0) {
				AddLine(v0, v1, color);
			}
			v0 = v1;
		}
	}

	//Draws Vertical Lines
	for (int i = 0; i < vertices.size() - slices - 1; ++i) {
		v0 = vertices[i];
		v1 = vertices[i + slices + 1];
		AddLine(v0, v1, color);
	}

	if (!hasLid) {
		return;
	}

	Math::Vector3 botCent, topCent = Math::Vector3::Zero;
	botCent.x += center.x;
	topCent.x += center.x;

	botCent.z += center.z;
	topCent.z += center.z;

	botCent.y -= yOffSet - center.y;
	topCent.y += yOffSet + center.y;

	//Draws Base
	v0 = botCent;
	for (int i = 0; i < slices; ++i) {
		v1 = vertices[i];
		AddLine(v0, v1, color);
	}

	//Draws Lid
	v0 = topCent;
	for (int i = static_cast<int>(vertices.size()) - slices; i < vertices.size(); ++i) 
	{
		v1 = vertices[i];
		AddLine(v0, v1, color);
	}
}

void SimpleDraw::AddFilledCylinder(const Math::Cylinder& cylinder, Color color, bool hasLid)
{
	AddFilledCylinder(cylinder.center, cylinder.slices, cylinder.rings, cylinder.radius, cylinder.height, color, hasLid ? cylinder.hasLid : hasLid);
}

void SimpleDraw::AddFilledCylinder(Math::Vector3 center, int slices, int rings, float radius, float height, Color color, bool hasLid)
{
	if (slices < 3) { slices = 3; }
	if (rings < 1) { rings = 1; }
	if (radius < 0.0f) { radius *= -1.0f; }
	if (height < 0.0f) { height *= -1.0f; }

	const float spacing = height / rings;
	const float yOffSet = height * 0.5f;

	const float sectorStep = Math::Constants::TwoPi / slices;
	float sectorAngle; //Radian

	std::vector<Math::Vector3> vertices;
	std::vector<float> indices;

	for (int i = 0; i <= rings; ++i) {
		for (int j = 0; j <= slices; ++j) {
			sectorAngle = j * sectorStep;
			const float x = radius * cos(sectorAngle) + center.x;
			const float y = i * spacing - yOffSet + center.y;
			const float z = radius * sin(sectorAngle) + center.z;
			vertices.push_back({ x, y, z});

			if (i != rings && j != slices) {
				indices.push_back(j + i + (i * slices) + 2 + slices);		//TopRight
				indices.push_back(j + i + (i * slices) + 1);				//Bottom Right
				indices.push_back(j + i + (i * slices));					//Bottom Left

				indices.push_back(j + i + (i * slices) + 2 + slices);		//TopRight
				indices.push_back(j + i + (i * slices));					//Bottom Left
				indices.push_back(j + i + (i * slices) + 1 + slices);		//TopLeft

				if (!hasLid) {
					indices.push_back(j + i + (i * slices));					//Bottom Left
					indices.push_back(j + i + (i * slices) + 1);				//Bottom Right
					indices.push_back(j + i + (i * slices) + 2 + slices);		//TopRight

					indices.push_back(j + i + (i * slices) + 1 + slices);		//TopLeft
					indices.push_back(j + i + (i * slices));					//Bottom Left
					indices.push_back(j + i + (i * slices) + 2 + slices);		//TopRight
				}
			}
		}
	}

	if (hasLid) {
		vertices.push_back({ center.x, rings * spacing - yOffSet + center.y, center.z }); //TopMiddle
		vertices.push_back({ center.x, rings * 0.5f * spacing - 2.0f * yOffSet + center.y, center.z }); //BotMiddle

		//Does the top and bot circles
		for (int i = 0; i < slices; ++i) {
			indices.push_back(vertices.size() - 2); //TopMiddle
			indices.push_back(vertices.size() - (3 + i));
			indices.push_back(vertices.size() - (4 + i));

			indices.push_back(vertices.size() - 1); //BotMiddle
			indices.push_back(i);
			indices.push_back(i + 1);
		}
	}

	for (int i = 0; i < indices.size(); i += 3) {
		sInstance->AddFace(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]], color);
	}
}

void SimpleDraw::AddPlane(int grid, Color color)
{
	if (grid < 1) {
		grid = 1;
	}

	const float spacing = 1.0f;
	Math::Vector3 v0, v1 = {};
	std::vector<Math::Vector3> vertices;

	const float OffSet = grid * 0.5f;

	for (int i = 0; i <= grid; ++i) {
		for (int j = 0; j <= grid; ++j) {
			v1 = { j * spacing - OffSet, 0.0f, i * spacing - OffSet };
			vertices.push_back(v1);

			//Draws Horizontal Lines
			if (j != 0) {
				AddLine(v0, v1, color);
			}
			v0 = v1;
		}
	}

	//Draws Vertical Lines
	for (int i = 0; i < vertices.size() - grid - 1; ++i) {
		v0 = vertices[i];
		v1 = vertices[i + grid + 1];
		AddLine(v0, v1, color);
	}
}

void SimpleDraw::AddPyramid(const Math::Pyramid& pyramid, Color color)
{
	AddPyramid(pyramid.center, pyramid.length, pyramid.width, pyramid.height, pyramid.rotation, color);
}

void SimpleDraw::AddPyramid(const Math::Vector3& center, float baseLength, float baseWidth, float height, const Math::Quaternion& rotation, Color color)
{
	baseLength = Math::Abs(baseLength);
	baseWidth = Math::Abs(baseWidth);
	height = Math::Abs(height);

	const float length = baseLength * 0.5f;
	const float width = baseWidth * 0.5f;

	Math::Vector3 LL = { center.x - length, center.y, center.z - width };
	Math::Vector3 LR = { center.x + length, center.y, center.z - width };
	Math::Vector3 UR = { center.x + length, center.y, center.z + width };
	Math::Vector3 UL = { center.x - length, center.y, center.z + width };
	Math::Vector3 Tip = { center.x, center.y + height, center.z };

	//const Math::Matrix4 transform = Transform{ {center.x, center.y + length, center.z}, rotation, {width, height, length} }.GetMatrix4();
	//LL = Math::TransformCoord(LL, transform);
	//LR = Math::TransformCoord(LR, transform);
	//UR = Math::TransformCoord(UR, transform);
	//UL = Math::TransformCoord(UL, transform);
	//Tip = Math::TransformCoord(Tip, transform);

	//Base
	AddLine(LL, LR, color);
	AddLine(LR, UR, color);
	AddLine(UR, UL, color);
	AddLine(UL, LL, color);

	//To Tip
	AddLine(LL, Tip, color);
	AddLine(LR, Tip, color);
	AddLine(UR, Tip, color);
	AddLine(UL, Tip, color);
}

void SimpleDraw::AddFilledPyramid(const Math::Pyramid& pyramid, Color color)
{
	AddFilledPyramid(pyramid.center, pyramid.length, pyramid.width, pyramid.height, pyramid.rotation, color);
}

void SimpleDraw::AddFilledPyramid(const Math::Vector3& center, float baseLength, float baseWidth, float height, const Math::Quaternion& rotation, Color color)
{
	if (baseLength < 0.0f) {
		baseLength *= -1.0f;
	}
	if (baseWidth < 0.0f) {
		baseWidth *= -1.0f;
	}
	if (height < 0.0f) {
		height *= -1.0f;
	}

	const float length = baseLength * 0.5f;
	const float width = baseWidth * 0.5f;

	Math::Vector3 LL = { center.x - length, center.y, center.z - width };
	Math::Vector3 LR = { center.x + length, center.y, center.z - width };
	Math::Vector3 UR = { center.x + length, center.y, center.z + width };
	Math::Vector3 UL = { center.x - length, center.y, center.z + width };
	Math::Vector3 Tip = { center.x, center.y + height, center.z };

	const Math::Matrix4 transform = Transform{ center, rotation, {1.0f, 1.0f, 1.0f} }.GetMatrix4();
	LL = Math::TransformCoord(LL, transform);
	LR = Math::TransformCoord(LR, transform);
	UR = Math::TransformCoord(UR, transform);
	UL = Math::TransformCoord(UL, transform);
	Tip = Math::TransformCoord(Tip, transform);

	//Base
	sInstance->AddFace(LL, LR, UR, color);
	sInstance->AddFace(LL, UR, UL, color);

	//To Tip
	sInstance->AddFace(Tip, LR, LL, color);
	sInstance->AddFace(Tip, UR, LR, color);
	sInstance->AddFace(Tip, UL, UR, color);
	sInstance->AddFace(Tip, LL, UL, color);
}

void SimpleDraw::AddSphere(const Math::Sphere& sphere, Color color)
{
	AddSphere(sphere.center, sphere.slices, sphere.rings, sphere.radius, color);
}

void SimpleDraw::AddSphere(Math::Vector3 center, int divisions, float radius, Color color)
{
	AddSphere(center, divisions, divisions, radius, color);
}

void SimpleDraw::AddSphere(Vector3 center, int slices, int rings, float radius, Color color)
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

	const float sectorStep = Math::Constants::TwoPi / slices;
	float sectorAngle; //Radian

	const float sectorLeap = Math::Constants::Pi / rings;
	float sectorLeapAngle; //Radian

	Vector3 v0, v1 = {};
	std::vector<Math::Vector3> vertices;

	for (int i = 0; i <= rings; ++i) {
		sectorLeapAngle = i * sectorLeap;
		for (int j = 0; j <= slices; ++j) {
			sectorAngle = j * sectorStep;
			float x = radius * sin(sectorAngle) * sin(sectorLeapAngle) + center.x;
			float y = radius * cos(sectorLeapAngle) + center.y;
			float z = radius * cos(sectorAngle) * sin(sectorLeapAngle) + center.z;

			v1 = { x, y, z };
			vertices.push_back(v1);

			//Draws Horizontal Lines
			if (j != 0) {
				AddLine(v0, v1, color);
			}
			v0 = v1;
		}
	}

	//Draws Vertical Lines
	for (int i = 0; i < vertices.size() - slices - 1; ++i) {
		v0 = vertices[i];
		v1 = vertices[i + slices + 1];
		AddLine(v0, v1, color);
	}
}

void SimpleDraw::AddSphere(Math::Vector3 center, int divisions, float radius, Color color, const Quaternion& rotation)
{
	AddSphere(center, divisions, divisions, radius, color, rotation);
}

void SimpleDraw::AddSphere(Math::Vector3 center, int slices, int rings, float radius, Color color, const Quaternion& rotation)
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

	const float sectorStep = Math::Constants::TwoPi / slices;
	const float ringStep = Math::Constants::Pi / rings;

	std::vector<Math::Vector3> vertices;
	vertices.reserve((rings + 1) * (slices + 1));

	Math::Vector3 v0, v1;

	// =========================
	// Generate + horizontal lines
	// =========================
	for (int i = 0; i <= rings; ++i)
	{
		float ringAngle = i * ringStep;

		for (int j = 0; j <= slices; ++j)
		{
			float sectorAngle = j * sectorStep;

			// 1. Local position (centered at origin)
			Math::Vector3 localPos = {
				radius * sin(sectorAngle) * sin(ringAngle),
				radius * cos(ringAngle),
				radius * cos(sectorAngle) * sin(ringAngle)
			};

			// 2. Apply rotation
			Math::Vector3 rotated = rotation * localPos;

			// 3. Translate
			v1 = rotated + center;

			vertices.push_back(v1);

			// Horizontal lines (along slices)
			if (j > 0)
			{
				AddLine(v0, v1, color);
			}

			v0 = v1;
		}
	}

	// =========================
	// Vertical lines (fixed)
	// =========================
	const int stride = slices + 1;

	for (int ring = 0; ring < rings; ++ring)
	{
		for (int slice = 0; slice <= slices; ++slice)
		{
			int current = ring * stride + slice;
			int next = (ring + 1) * stride + slice;

			AddLine(vertices[current], vertices[next], color);
		}
	}
}

void SimpleDraw::AddFilledSphere(const Math::Sphere& sphere, Color color)
{
	AddFilledSphere(sphere.center, sphere.slices, sphere.rings, sphere.radius, color);
}

void SimpleDraw::AddFilledSphere(Math::Vector3 center, int divisions, float radius, Color color)
{
	AddFilledSphere(center, divisions, divisions, radius, color);
}

void SimpleDraw::AddFilledSphere(Math::Vector3 center, int slices, int rings, float radius, Color color)
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

	std::vector<Math::Vector3> vertices;
	std::vector<float> indices;

	const float sectorStep = Math::Constants::TwoPi / slices;
	float sectorAngle; //Radian

	const float sectorLeap = Math::Constants::Pi / rings;
	float sectorLeapAngle; //Radian

	for (int i = 0; i <= rings; ++i) {
		sectorLeapAngle = i * sectorLeap;
		for (int j = 0; j <= slices; ++j) {
			sectorAngle = j * sectorStep;
			float x = radius * sin(sectorAngle) * sin(sectorLeapAngle) + center.x;
			float y = radius * cos(sectorLeapAngle) + center.y;
			float z = radius * cos(sectorAngle) * sin(sectorLeapAngle) + center.z;
			vertices.push_back({ x, y, z });

			if (i != rings && j != slices) {
				indices.push_back(j + i + (i * slices) + 2 + slices);	//TopRight
				indices.push_back(j + i + (i * slices) + 1);			//Bottom Right
				indices.push_back(j + i + (i * slices));				//Bottom Left

				indices.push_back(j + i + (i * slices) + 2 + slices);	//TopRight
				indices.push_back(j + i + (i * slices));				//Bottom Left
				indices.push_back(j + i + (i * slices) + 1 + slices);	//TopLeft
			}
		}
	}

	for (int i = 0; i < indices.size(); i += 3) {
		sInstance->AddFace(vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]], color);
	}
}

void SimpleDraw::AddTransform(Math::Matrix4 matrix)
{
	const Math::Vector3 S = {matrix._11, matrix._12, matrix._13}; //Side
	const Math::Vector3 L = {matrix._31, matrix._32, matrix._33}; //Look
	const Math::Vector3 U = {matrix._21, matrix._22, matrix._23}; //Up
	const Math::Vector3 T = {matrix._41, matrix._42, matrix._43}; //Translation

	AddLine(T, T + S, Colors::Red);
	AddLine(T, T + U, Colors::Green);
	AddLine(T, T + L, Colors::Blue);
}

void SimpleDraw::Render(const Camera& camera)
{
	sInstance->Render(camera);
}