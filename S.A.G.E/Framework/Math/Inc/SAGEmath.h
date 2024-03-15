#pragma once

#include "Common.h"

// Linear Algerbra
#include "Vector2.h"
#include "Vector2Int.h"
#include "Vector3.h"
#include "Vector3Int.h"
#include "Vector4.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include "Rect.h"
#include "RectInt.h"

// Utility
#include "Constant.h"
#include "Noise.h"
#include "Random.h"

// Shapes
#include "AABB.h"
#include "Cylinder.h"
#include "OBB.h"
#include "Plane.h"
#include "Pyramid.h"
#include "Ray.h"
#include "Sphere.h"

namespace SAGE::Math
{
	//Template
	template <class T>
	constexpr T Min(T a, T b) { return (a < b) ? a : b; }

	template <class T>
	constexpr T Max(T a, T b) { return (a > b) ? a : b; }

	template <class T>
	constexpr T Clamp(T value, T min, T max) { return Max(min, Min(max, value)); }

	template <class T>
	constexpr T Lerp(T a, T b, float t) { return static_cast<T>(a + (b - a) * t); }

	template <class T>
	constexpr T Abs(T value) { return value >= 0 ? value : -value; }

	template <class T>
	constexpr T Sqr(T value) { return value * value; }

	//Vector2
	#pragma region ---Vector2---
	inline float Dot(const Vector2& a, const Vector2& b) { return (a.x * b.x) + (a.y * b.y); }
	inline float MagnitudeSqr(const Vector2& v) { return (v.x * v.x) + (v.y * v.y); }
	inline float Magnitude(const Vector2& v) { return sqrt(MagnitudeSqr(v)); }
	inline float DistanceSqr(const Vector2& a, const Vector2& b) { return MagnitudeSqr(a - b); }
	inline float Distance(const Vector2& a, const Vector2& b) { return sqrt(DistanceSqr(a, b)); }
	inline Vector2 Normalize(const Vector2& v) { return v / Magnitude(v); }
	inline Vector2 PerpendicularLH(const Vector2& v) { return Vector2(-v.y, v.x); }
	inline Vector2 PerpendicularRH(const Vector2& v) { return Vector2(v.y, -v.x); }
#pragma endregion

	//Vector3
	#pragma region ---Vector3---
	constexpr Vector3 GetRight(const Matrix4& m)					{ return { m._11, m._12 , m._13 }; }
	constexpr Vector3 GetUp(const Matrix4& m)						{ return { m._21, m._22 , m._23 }; }
	constexpr Vector3 GetLook(const Matrix4& m)						{ return { m._31, m._32 , m._33 }; }
	constexpr Vector3 GetTranslation(const Matrix4& m)				{ return { m._41, m._42 , m._43 }; }
	inline float Dot(const Vector3& a, const Vector3& b)			{ return (a.x * b.x) + (a.y * b.y) + (a.z * b.z); }
	inline float MagnitudeSqr(const Vector3& v)						{ return (v.x * v.x) + (v.y * v.y) + (v.z * v.z); }
	inline float Magnitude(const Vector3& v)						{ return sqrt(MagnitudeSqr(v)); }
	inline float DistanceSqr(const Vector3& a, const Vector3& b)	{ return MagnitudeSqr(a - b);  }
	inline float Distance(const Vector3& a, const Vector3& b)		{ return sqrt(DistanceSqr(a, b)); }
	inline Vector3 Normalize(const Vector3& v)						{ return v / Magnitude(v); }
	inline Vector3 Cross(const Vector3& a, const Vector3& b)		{ return Vector3((a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z), (a.x * b.y) - (a.y * b.x)); }
#pragma endregion

	//Matrix3
	#pragma region ---Matrix3---
	inline Vector2 GetTranslation(const Matrix3& m) { return Vector2(m._31, m._32); }

	inline Vector2 TransformCoord(const Vector2& v, const Matrix3& m)
	{
		return Vector2 (
			v.x * m._11 + v.y * m._21 + m._31,
			v.x * m._12 + v.y * m._22 + m._32
		);
	}

	inline Vector2 TransformNormal(const Vector2& v, const Matrix3& m)
	{
		return Vector2 (
			v.x * m._11 + v.y * m._21,
			v.x * m._12 + v.y * m._22
		);
	}

	inline float Determinant(const Matrix3& m)
	{
		float det = 0.0f;
		det = (m._11 * (m._22 * m._33 - m._23 * m._32));
		det -= (m._12 * (m._21 * m._33 - m._23 * m._31));
		det += (m._13 * (m._21 * m._32 - m._22 * m._31));
		return det;
	}

	inline Matrix3 Adjoint(const Matrix3& m)
	{
		return Matrix3 (
			(m._22 * m._33 - m._23 * m._32),
			-(m._12 * m._33 - m._13 * m._32),
			(m._12 * m._23 - m._13 * m._22),

			-(m._21 * m._33 - m._23 * m._31),
			(m._11 * m._33 - m._13 * m._31),
			-(m._11 * m._23 - m._13 * m._21),

			(m._21 * m._32 - m._22 * m._31),
			-(m._11 * m._32 - m._12 * m._31),
			(m._11 * m._22 - m._12 * m._21)
		);
	}

	inline Matrix3 Inverse(const Matrix3& m)
	{
		const float determinant = Determinant(m);
		const float invDet = 1.0f / determinant;
		return Adjoint(m) * invDet;
	}

	inline Matrix3 Transpose(const Matrix3& m)
	{
		return Matrix3 (
			m._11, m._21, m._31,
			m._12, m._22, m._32,
			m._13, m._23, m._33
		);
	}

#pragma endregion

	//Matrix4
	#pragma region ---Matrix4---

	inline Vector3 TransformCoord(const Vector3& v, const Matrix4& m) {
		return Vector3 (
			v.x * m._11 + v.y * m._21 + v.z * m._31 + m._41,
			v.x * m._12 + v.y * m._22 + v.z * m._32 + m._42,
			v.x * m._13 + v.y * m._23 + v.z * m._33 + m._43
		);
	}

	inline Vector3 TransformNormal(const Vector3& v, const Matrix4& m) {
		return Vector3 (
			v.x * m._11 + v.y * m._21 + v.z * m._31,
			v.x * m._12 + v.y * m._22 + v.z * m._32,
			v.x * m._13 + v.y * m._23 + v.z * m._33
		);
	}

	inline float Determinant(const Matrix4& m) {
		float det = 0.0f;
		det = (m._11 * (m._22 * (m._33 * m._44 - (m._43 * m._34)) - m._23 * (m._32 * m._44 - (m._42 * m._34)) + m._24 * (m._32 * m._43 - (m._42 * m._33))));
		det -= (m._12 * (m._21 * (m._33 * m._44 - (m._43 * m._34)) - m._23 * (m._31 * m._44 - (m._41 * m._34)) + m._24 * (m._31 * m._43 - (m._41 * m._33))));
		det += (m._13 * (m._21 * (m._32 * m._44 - (m._42 * m._34)) - m._22 * (m._31 * m._44 - (m._41 * m._34)) + m._24 * (m._31 * m._42 - (m._41 * m._32))));
		det -= (m._14 * (m._21 * (m._32 * m._43 - (m._42 * m._33)) - m._22 * (m._31 * m._43 - (m._41 * m._33)) + m._23 * (m._31 * m._42 - (m._41 * m._32))));
		return det;
	}

	inline Matrix4 Transpose(const Matrix4& m) {
		return Matrix4 (
			m._11, m._21, m._31, m._41,
			m._12, m._22, m._32, m._42,
			m._13, m._23, m._33, m._43,
			m._14, m._24, m._34, m._44
		);
	}

	inline Matrix4 Adjoint(const Matrix4& m) {
		return Matrix4 (
			(m._22 * ((m._33 * m._44) - (m._43 * m._34)) - m._23 * ((m._32 * m._44) - (m._42 * m._34)) + m._24 * ((m._32 * m._43) - (m._42 * m._33))),
			-(m._12 * ((m._33 * m._44) - (m._43 * m._34)) - m._13 * ((m._32 * m._44) - (m._42 * m._34)) + m._14 * ((m._32 * m._43) - (m._42 * m._33))),
			(m._12 * ((m._23 * m._44) - (m._43 * m._24)) - m._13 * ((m._22 * m._44) - (m._42 * m._24)) + m._14 * ((m._22 * m._43) - (m._42 * m._23))),
			-(m._12 * ((m._23 * m._34) - (m._33 * m._24)) - m._13 * ((m._22 * m._34) - (m._32 * m._24)) + m._14 * ((m._22 * m._33) - (m._32 * m._23))),

			-(m._21 * ((m._33 * m._44) - (m._43 * m._34)) - m._31 * ((m._23 * m._44) - (m._24 * m._43)) + m._41 * ((m._23 * m._34) - (m._24 * m._33))),
			(m._11 * ((m._33 * m._44) - (m._43 * m._34)) - m._13 * ((m._31 * m._44) - (m._41 * m._34)) + m._14 * ((m._31 * m._43) - (m._41 * m._33))),
			-(m._11 * ((m._23 * m._44) - (m._43 * m._24)) - m._13 * ((m._21 * m._44) - (m._41 * m._24)) + m._14 * ((m._21 * m._43) - (m._41 * m._23))),
			(m._11 * ((m._23 * m._34) - (m._33 * m._24)) - m._13 * ((m._21 * m._34) - (m._31 * m._24)) + m._14 * ((m._21 * m._33) - (m._31 * m._23))),

			(m._21 * ((m._32 * m._44) - (m._42 * m._34)) - m._31 * ((m._22 * m._44) - (m._42 * m._24)) + m._41 * ((m._22 * m._34) - (m._32 * m._24))),
			-(m._11 * ((m._32 * m._44) - (m._42 * m._34)) - m._31 * ((m._12 * m._44) - (m._42 * m._14)) + m._41 * ((m._12 * m._34) - (m._32 * m._14))),
			(m._11 * ((m._22 * m._44) - (m._42 * m._24)) - m._12 * ((m._21 * m._44) - (m._41 * m._24)) + m._14 * ((m._21 * m._42) - (m._41 * m._22))),
			-(m._11 * ((m._22 * m._34) - (m._32 * m._24)) - m._21 * ((m._12 * m._34) - (m._32 * m._14)) + m._31 * ((m._12 * m._24) - (m._22 * m._14))),

			-(m._21 * ((m._32 * m._43) - (m._42 * m._33)) - m._31 * ((m._22 * m._43) - (m._42 * m._23)) + m._41 * ((m._22 * m._33) - (m._32 * m._23))),
			(m._11 * ((m._32 * m._43) - (m._42 * m._33)) - m._12 * ((m._31 * m._43) - (m._41 * m._33)) + m._13 * ((m._31 * m._42) - (m._41 * m._32))),
			-(m._11 * ((m._22 * m._43) - (m._42 * m._23)) - m._12 * ((m._21 * m._43) - (m._41 * m._23)) + m._13 * ((m._21 * m._42) - (m._41 * m._22))),
			(m._11 * ((m._22 * m._33) - (m._32 * m._23)) - m._12 * ((m._21 * m._33) - (m._31 * m._23)) + m._13 * ((m._21 * m._32) - (m._31 * m._22)))
		);
	}

	inline Matrix4 Inverse(const Matrix4& m) {
		const float determinant = Determinant(m);
		const float invDet = 1.0f / determinant;
		return Adjoint(m) * invDet;
	}

#pragma endregion

	//Quaternion
	#pragma region ---Quaternion---

	inline float Magnitude(const Quaternion& q) { return sqrt((q.w * q.w) + (q.x * q.x) + (q.y * q.y) + (q.z * q.z)); }

	inline Quaternion Normalize(const Quaternion& q) 
	{
		const float n = 1.0f / sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);

		return Quaternion{
			q.w * n,
			q.x * n,
			q.y * n,
			q.z * n
		};
	}

	inline Quaternion Slerp(Quaternion q0, Quaternion q1, float t) {

		// Find the dot product
		float dot = (q0.w * q1.w) + (q0.x * q1.x) + (q0.y * q1.y) + (q0.z * q1.z);

		// Determine the direction of the rotation.
		if (dot < 0.0f)
		{
			dot = -dot;
			q1 = -q1;
		}
		else if (dot > 0.999f)
		{
			return Normalize(Lerp(q0, q1, t));
		}

		float theta = acosf(dot);
		float sintheta = sinf(theta);
		float scale0 = sinf(theta * (1.0f - t)) / sintheta;
		float scale1 = sinf(theta * t) / sintheta;

		// Perform the slerp
		return Quaternion
		(
			(q0.w * scale0) + (q1.w * scale1),
			(q0.x * scale0) + (q1.x * scale1),
			(q0.y * scale0) + (q1.y * scale1),
			(q0.z * scale0) + (q1.z * scale1)
		);
	}

#pragma endregion

	//Rect
	#pragma region ---Rect---

	inline bool IsEmpty(const Rect& rect) { return rect.right <= rect.left || rect.bottom <= rect.top; }
	inline bool IsRectOverlap(const Rect& r1, const Rect& r2) 
	{
		// Check if one rect is on left side of the other
		if (r1.right <= r2.left || r2.right <= r1.left) {
			return false;
		}

		// Check if one rect is above the other
		if (r1.bottom <= r2.top || r2.bottom <= r1.top) {
			return false;
		}

		// If none of the sides of one rectangle are outside the other, they are overlapping
		return true;
	}

#pragma endregion

	//Other
	#pragma region ---Other---
	
	//constexpr float Pow(float num, int _pow)
	//{
	//	if (pow <= 0)
	//		return 1.0f;
	//	return num * Pow(num, _pow - 1);
	//}

	bool Intersect(const Ray& ray, const Plane& plane, float& distance);
	bool Intersect(const Ray& ray, const OBB& obb, Vector3& point, Vector3& normal);

	bool Intersect(const Vector3& point, const AABB& aabb);
	bool Intersect(const Vector3& point, const OBB& obb);

	bool Intersect(const Vector3& point, const Plane& plane, float& penetration);
	bool Intersect(const Sphere& sphere, const Plane& plane, float& penetration);
	bool Intersect(const AABB& aabb, const Plane& plane, float& penetration);

	bool Intersect(const AABB& aabb1, AABB& aabb2, Vector3& normal, float& penetration);

	bool Intersect(const AABB& aabb, OBB& obb);

#pragma endregion


}