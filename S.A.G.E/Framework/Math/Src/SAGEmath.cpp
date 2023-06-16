#include "Precompiled.h"
#include "SAGEmath.h"

using namespace SAGE::Math;

//Vector2
#pragma region ---Vector2---
const Vector2 Vector2::Zero(0.0f);
const Vector2 Vector2::One(1.0f);
const Vector2 Vector2::XAxis(1.0f, 0.0f);
const Vector2 Vector2::YAxis(0.0f, 1.0f);
#pragma endregion

//Vector2Int
#pragma region ---Vector2Int---
const Vector2Int Vector2Int::Zero(0);
const Vector2Int Vector2Int::One(1);
const Vector2Int Vector2Int::XAxis(1, 0);
const Vector2Int Vector2Int::YAxis(0, 1);
#pragma endregion

//Vector3
#pragma region ---Vector3---
const Vector3 Vector3::Zero(0.0f);
const Vector3 Vector3::One(1.0f);
const Vector3 Vector3::XAxis(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::YAxis(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::ZAxis(0.0f, 0.0f, 1.0f);
#pragma endregion

//Vector3Int
#pragma region ---Vector3Int---
const Vector3Int Vector3Int::Zero(0);
const Vector3Int Vector3Int::One(1);
const Vector3Int Vector3Int::XAxis(1, 0, 0);
const Vector3Int Vector3Int::YAxis(0, 1, 0);
const Vector3Int Vector3Int::ZAxis(0, 0, 0);
#pragma endregion

//Matrix3
#pragma region ---Matrix3---

const Matrix3 Matrix3::Identity(
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f
);

#pragma endregion

//Matrix4
#pragma region ---Matrix4---

const Matrix4 Matrix4::Identity(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
);

Matrix4 Matrix4::RotationAxis(const Vector3& axis, float rad)
{
	const Vector3 u = Normalize(axis);
	const float x = u.x;
	const float y = u.y;
	const float z = u.z;
	const float s = sin(rad);
	const float c = cos(rad);

	return Matrix4 (
		c + (x * x * (1.0f - c)),
		x * y * (1.0f - c) + z * s,
		x * z * (1.0f - c) - y * s,
		0.0f,

		x * y * (1.0f - c) - z * s,
		c + (y * y * (1.0f - c)),
		y * z * (1.0f - c) + x * s,
		0.0f,

		x * z * (1.0f - c) + y * s,
		y * z * (1.0f - c) - x * s,
		c + (z * z * (1.0f - c)),
		0.0f,

		0.0f, 0.0f, 0.0f, 1.0f
	);
}

Matrix4 Matrix4::RotationQuaternion(const Quaternion& q)
{
	Quaternion quat = Normalize(q);

	Matrix4 mat = {
		1.0f - 2.0f * quat.y * quat.y - 2.0f * quat.z * quat.z, 2.0f * quat.x * quat.y - 2.0f * quat.z * quat.w, 2.0f * quat.x * quat.z + 2.0f * quat.y * quat.w, 0.0f,
		2.0f * quat.x * quat.y + 2.0f * quat.z * quat.w, 1.0f - 2.0f * quat.x * quat.x - 2.0f * quat.z * quat.z, 2.0f * quat.y * quat.z - 2.0f * quat.x * quat.w, 0.0f,
		2.0f * quat.x * quat.z - 2.0f * quat.y * quat.w, 2.0f * quat.y * quat.z + 2.0f * quat.x * quat.w, 1.0f - 2.0f * quat.x * quat.x - 2.0f * quat.y * quat.y, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return Transpose(mat);
}
#pragma endregion

//Quaternion
#pragma region ---Quaternion---

const Quaternion Quaternion::Zero( 0.0f, 0.0f, 0.0f, 0.0f );
const Quaternion Quaternion::Identity( 1.0f, 0.0f, 0.0f, 0.0f );

Quaternion Quaternion::RotationAxis(const Vector3& axis, float rad)
{
	const float c = cos(rad * 0.5f);
	const float s = sin(rad * 0.5f);
	const Vector3 a = Normalize(axis);
	return Quaternion(c, a.x * s, a.y * s, a.z * s);
}

Quaternion Quaternion::RotationEuler(const Vector3& eulerAngles)
{
	const float cr = cos(eulerAngles.x * 0.5f);
	const float sr = sin(eulerAngles.x * 0.5f);
	const float cp = cos(eulerAngles.y * 0.5f);
	const float sp = sin(eulerAngles.y * 0.5f);
	const float cy = cos(eulerAngles.z * 0.5f);
	const float sy = sin(eulerAngles.z * 0.5f);

	return Quaternion(
	cr * cp * cy + sr * sp * sy,
	sr * cp * cy - cr * sp * sy,
	cr * sp * cy + sr * cp * sy, 
	cr * cp * sy - sr * sp * cy);
}

Quaternion Quaternion::RotationMatrix(const Matrix4& m)
{
	const float w = sqrt(m._11 + m._22 + m._33 + 1) * 0.5f;
	const float x = sqrt(m._11 - m._22 - m._33 + 1) * 0.5f;
	const float y = sqrt(-m._11 + m._22 - m._33 + 1) * 0.5f;
	const float z = sqrt(-m._11 - m._22 + m._33 + 1) * 0.5f;

	Quaternion quat{};
	if (w >= x && w >= y && w >= z)
	{
		quat.w = w;
		quat.x = (m._23 - m._32) / (4 * w);
		quat.y = (m._31 - m._13) / (4 * w);
		quat.z = (m._12 - m._21) / (4 * w);
	}
	else if (x >= w && x >= y && x >= z)
	{
		quat.w = (m._23 - m._32) / (4 * x);
		quat.x = x;
		quat.y = (m._12 - m._21) / (4 * x);
		quat.z = (m._31 - m._13) / (4 * x);
	}
	else if (y >= w && y >= x && y >= z)
	{
		quat.w = (m._31 - m._13) / (4 * y);
		quat.x = (m._12 - m._21) / (4 * y);
		quat.y = y;
		quat.z = (m._23 - m._32) / (4 * y);
	}
	else if (z >= w && z >= x && z >= y)
	{
		quat.w = (m._12 - m._21) / (4 * z);
		quat.x = (m._31 - m._13) / (4 * z);
		quat.y = (m._23 - m._32) / (4 * z);;
		quat.z = z;
	}

	return quat;
}

Quaternion Quaternion::RotationLook(const Vector3& direction, const Vector3& up) // Use RotationMatrix
{
	// Direction is normalized before entering method

	Vector3 vector = Normalize(direction);
	Vector3 vector2 = Normalize(Cross(up, vector));
	Vector3 vector3 = Cross(vector, vector2);

	Matrix4 m{};
	m._11 = vector2.x;
	m._12 = vector2.y;
	m._13 = vector2.z;
	m._21 = vector3.x;
	m._22 = vector3.y;
	m._23 = vector3.z;
	m._31 = vector.x;
	m._32 = vector.y;
	m._33 = vector.z;

	float num8 = (m._11 + m._22) + m._33;
	Quaternion quaternion{};
	if (num8 > 0.0f)
	{
		float num = sqrt(num8 + 1.0f);
		quaternion.w = num * 0.5f;
		num = 0.5f / num;
		quaternion.x = (m._23 - m._32) * num;
		quaternion.y = (m._31 - m._13) * num;
		quaternion.z = (m._12 - m._21) * num;
		return quaternion;
	}
	if ((m._11 >= m._22) && (m._11 >= m._33))
	{
		float num = sqrt(((1.0f + m._11) - m._22) - m._33);
		float num2 = 0.5f / num;
		quaternion.w = (m._23 - m._32) * num2;
		quaternion.x = 0.5f * num;
		quaternion.y = (m._12 + m._21) * num2;
		quaternion.z = (m._13 + m._31) * num2;
		return quaternion;
	}
	if (m._22 > m._33)
	{
		float num = sqrt(((1.0f + m._22) - m._11) - m._33);
		float num2 = 0.5f / num;
		quaternion.w = (m._31 - m._13) * num2;
		quaternion.x = (m._21 + m._12) * num2;
		quaternion.y = 0.5f * num;
		quaternion.z = (m._32 + m._23) * num2;
		return quaternion;
	}

	float num = sqrt(((1.0f + m._33) - m._11) - m._22);
	float num2 = 0.5f / num;
	quaternion.w = (m._12 - m._21) * num2;
	quaternion.x = (m._31 + m._13) * num2;
	quaternion.y = (m._32 + m._23) * num2;
	quaternion.z = 0.5f * num;
	return quaternion;
}

Quaternion Quaternion::RotationFromTo(const Vector3& from, const Vector3& to)
{
	float dot = Dot(from, to);

	float wx = from.y * to.z - from.z * to.y;
	float wy = from.z * to.x - from.x * to.z;
	float wz = from.x * to.y - from.y * to.x;

	Quaternion quat(dot + sqrt(dot * dot + wx * wx + wy * wy + wz * wz), wx, wy, wz);

	return Normalize(quat);
}

#pragma endregion

// Other
#pragma region ---Other---

bool SAGE::Math::Intersect(const Ray& ray, const Plane& plane, float& distance)
{
	const float originDotN = Dot(ray.origin, plane.normal);
	const float directionDotN = Dot(ray.direction, plane.normal);

	// Check if ray is parrallel to the plane
	if (directionDotN == 0.0f)
	{
		if (originDotN == plane.distance)
		{
			distance = 0.0f;
			return true;
		}
		return false;
	}

	// Compute distance
	distance = (plane.distance - originDotN) / directionDotN;
	return true;
}

bool SAGE::Math::Intersect(const Ray& ray, const OBB& obb, Vector3& point, Vector3& normal)
{
	// Compute the local to world / world to local matrices
	const Matrix4 matTrans = Matrix4::Translation(obb.center);
	const Matrix4 matRot = Matrix4::RotationQuaternion(obb.rotation);
	const Matrix4 matWorld = matRot * matTrans;
	const Matrix4 matWorldinv = Inverse(matWorld);

	// Transform the ray into the OBBS local space
	const Vector3 org = TransformCoord(ray.origin, matWorldinv);
	const Vector3 dir = TransformNormal(ray.direction, matWorldinv);
	const Ray localRay{ org, dir };

	const Plane planes[] =
	{
		{{ 0.0f,  0.0f, -1.0f}, obb.extend.z},
		{{ 0.0f,  0.0f,  1.0f}, obb.extend.z},
		{{ 0.0f, -1.0f,  0.0f}, obb.extend.y},
		{{ 0.0f,  1.0f,  0.0f}, obb.extend.y},
		{{-1.0f,  0.0f,  0.0f}, obb.extend.x},
		{{ 1.0f,  0.0f,  0.0f}, obb.extend.x}
	};

	uint32_t numIntersection = 0;
	for (auto& plane : planes)
	{
		const float d = Dot(org, plane.normal);
		if (d > plane.distance)
		{
			float distance = 0.0f;
			if (Intersect(localRay, plane, distance) && distance >= 0.0f)
			{
				Vector3 pt = org + (dir * distance);
				if (abs(pt.x) <= obb.extend.x + 0.01f &&
					abs(pt.y) <= obb.extend.y + 0.01f &&
					abs(pt.z) <= obb.extend.z + 0.01f)
				{
					point = pt;
					normal = plane.normal;
					++numIntersection;
				}
			}
		}
	}

	if (numIntersection == 0) {
		return false;
	}

	point = TransformCoord(point, matWorld);
	normal = TransformNormal(normal, matWorld);
	return true;
}

bool SAGE::Math::Intersect(const Vector3& point, const AABB& aabb)
{
	const auto min = aabb.Min();
	const auto max = aabb.Max();

	if (point.x < min.x || point.x > max.x ||
		point.y < min.y || point.y > max.y ||
		point.z < min.z || point.z > max.z)
	{
		return false;
	}
	return true;
}

bool SAGE::Math::Intersect(const Vector3& point, const OBB& obb)
{
	const Matrix4 matTran = Matrix4::Translation(obb.center);
	const Matrix4 matRot = Matrix4::RotationQuaternion(obb.rotation);
	const Matrix4 matScale = Matrix4::Scaling(obb.extend);
	const Matrix4 transform = matScale * matRot * matTran;

	const Vector3 localPoint = TransformCoord(point, Inverse(transform));
	const AABB unitAABB = { Vector3::Zero, Vector3::One };

	return Intersect(localPoint, unitAABB);
}

bool SAGE::Math::Intersect(const Vector3& point, const Plane& plane, float& penetration)
{
	const float centerDistance = Dot(point, plane.normal);
	const float distToPlane = centerDistance - plane.distance;

	if (distToPlane > 0.0f) {
		return false;
	}

	penetration = plane.distance - centerDistance;
	return true;
}

bool SAGE::Math::Intersect(const Sphere & sphere, const Plane & plane, float& penetration)
{
	const float centerDistance = Dot(sphere.center, plane.normal);
	const float distToPlane = centerDistance - plane.distance - sphere.radius;

	if (distToPlane > 0.0f) {
		return false;
	}

	penetration = plane.distance - centerDistance + sphere.radius;
	return true;
}

bool SAGE::Math::Intersect(const AABB& aabb, const Plane& plane, float& penetration)
{
	// https://github.com/gdbooks/3DCollisions
	// Convert AABB to center-extents representation
	const Vector3 c = aabb.center;
	const Vector3 e = aabb.extend;

	// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
	float r = e.x * Abs(plane.normal.x) + e.y * Abs(plane.normal.y) + e.y * Abs(plane.normal.z);

	// Compute distance of box center from plane
	float s = Dot(plane.normal, c) - plane.distance;

	// Intersection occurs when distance s falls within [-r,+r] interval
	penetration = r - s;
	return abs(s) <= r;
}

bool SAGE::Math::Intersect(const AABB& aabb1, AABB& aabb2, Vector3& normal, float& penetration)
{
	const auto& aMin = aabb1.Min();
	const auto& aMax = aabb1.Max();
	const auto& bMin = aabb2.Min();
	const auto& bMax = aabb2.Max();

	// No Collision
	if (aMin.x > bMax.x || aMax.x < bMin.x ||
		aMin.y > bMax.y || aMax.y < bMin.y ||
		aMin.z > bMax.z || aMax.z < bMin.z)
	{
		return false;
	}

	// Collision - Find penetration amount
	// Create 6 planes with aabb2
	penetration = FLT_MAX;
	float penetrationAmt = FLT_MAX;

	Plane plane = Plane(Vector3::YAxis, aabb2.center.y + aabb2.extend.y); // Y Axis
	if (Intersect(aabb1, plane, penetrationAmt) && penetrationAmt < penetration) {
		normal = plane.normal;
		penetration = penetrationAmt;
	}

	plane = Plane(-Vector3::YAxis, -(aabb2.center.y - aabb2.extend.y)); // -Y Axis
	if (Intersect(aabb1, plane, penetrationAmt) && penetrationAmt < penetration) {
		normal = plane.normal;
		penetration = penetrationAmt;
	}

	plane = Plane(Vector3::XAxis, aabb2.center.x + aabb2.extend.x); // X Axis
	if (Intersect(aabb1, plane, penetrationAmt) && penetrationAmt < penetration) {
		normal = plane.normal;
		penetration = penetrationAmt;
	}

	plane = Plane(-Vector3::XAxis, -(aabb2.center.x - aabb2.extend.x)); // -X Axis
	if (Intersect(aabb1, plane, penetrationAmt) && penetrationAmt < penetration) {
		normal = plane.normal;
		penetration = penetrationAmt;
	}

	plane = Plane(Vector3::ZAxis, aabb2.center.z); // Z Axis
	if (Intersect(aabb1, plane, penetrationAmt) && penetrationAmt < penetration) {
		normal = plane.normal;
		penetration = penetrationAmt;
	}

	plane = Plane(-Vector3::ZAxis, -aabb2.center.z ); // -Z Axis
	if (Intersect(aabb1, plane, penetrationAmt) && penetrationAmt < penetration) {
		normal = plane.normal;
		penetration = penetrationAmt;
	}

	return true;
}

bool SAGE::Math::Intersect(const AABB& aabb, OBB& obb)
{
	std::vector<Vector3> points;
	points.push_back({ aabb.center + Vector3(+aabb.extend.x, +aabb.extend.y, +aabb.extend.z) });
	points.push_back({ aabb.center + Vector3(+aabb.extend.x, +aabb.extend.y, -aabb.extend.z) });
	points.push_back({ aabb.center + Vector3(-aabb.extend.x, +aabb.extend.y, +aabb.extend.z) });
	points.push_back({ aabb.center + Vector3(-aabb.extend.x, +aabb.extend.y, -aabb.extend.z) });
	points.push_back({ aabb.center + Vector3(+aabb.extend.x, -aabb.extend.y, +aabb.extend.z) });
	points.push_back({ aabb.center + Vector3(+aabb.extend.x, -aabb.extend.y, -aabb.extend.z) });
	points.push_back({ aabb.center + Vector3(-aabb.extend.x, -aabb.extend.y, +aabb.extend.z) });
	points.push_back({ aabb.center + Vector3(-aabb.extend.x, -aabb.extend.y, -aabb.extend.z) });

	for (const auto& p : points)
	{
		if (Intersect(p, obb))
		{
			return true;
		}
	}
	return false;
}

#pragma endregion
