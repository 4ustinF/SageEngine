#pragma once

#include "Colors.h"

namespace SAGE::Graphics
{
	class Camera;

	namespace SimpleDraw
	{
		void StaticInitialize(uint32_t maxVertexCount);
		void StaticTerminate();

		void AddLine(const Math::Vector3& v0, const Math::Vector3& v1, Color color);

		void AddAABB(const Math::AABB& aabb, Color color);
		void AddAABB(const Math::Vector3& center, const Math::Vector3& extend, Color color);
		void AddAABB(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, Color color);
		void AddFilledAABB(const Math::AABB& aabb, Color color);
		void AddFilledAABB(const Math::Vector3& center, const Math::Vector3& extend, Color color);
		void AddFilledAABB(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, Color color);

		void AddOBB(const Math::OBB& obb, const Color color);
		void AddOBB(const Math::Vector3& center, const Math::Vector3& extend, const Math::Quaternion& rotation, const Color color);
		void AddFilledOBB(const Math::OBB& obb, const Color color);
		void AddFilledOBB(const Math::Vector3& center, const Math::Vector3& extend, const Math::Quaternion& rotation, const Color color);

		void AddCylinder(const Math::Cylinder& cylinder, Color color, bool hasLid = true);
		void AddCylinder(Math::Vector3 center, int slices, int rings, float radius, float height, Color color, bool hasLid = true);
		void AddFilledCylinder(const Math::Cylinder& cylinder, Color color, bool hasLid = true);
		void AddFilledCylinder(Math::Vector3 center, int slices, int rings, float radius, float height, Color color, bool hasLid = true);

		void AddPlane(int grid, Color color);

		void AddPyramid(const Math::Pyramid& pyramid, Color color);
		void AddPyramid(const Math::Vector3& center, float baseLength, float baseWidth, float height, const Math::Quaternion& rotation, Color color);
		void AddFilledPyramid(const Math::Pyramid& pyramid, Color color);
		void AddFilledPyramid(const Math::Vector3& center, float baseLength, float baseWidth, float height, const Math::Quaternion& rotation, Color color);

		void AddSphere(const Math::Sphere& sphere, Color color);
		void AddSphere(Math::Vector3 center, int slices, int rings, float radius, Color color);
		void AddFilledSphere(const Math::Sphere& sphere, Color color);
		void AddFilledSphere(Math::Vector3 center, int slices, int rings, float radius, Color color);

		void AddTransform(Math::Matrix4 matrix);

		void Render(const Camera& camera);
	}
}