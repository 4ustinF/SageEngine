#pragma once

//Framework headers
#include <Core/Inc/Core.h>
#include <Coroutine/Inc/Couroutine.h>
#include <Graphics/Inc/Graphics.h>
#include <Input/Inc/Input.h>
#include <Math/Inc/SAGEmath.h>
#include <Physics/Inc/Physics.h>

// External Headers
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <Bullet/btBulletCollisionCommon.h>
#include <Bullet/btBulletDynamicsCommon.h>

inline btVector3 ConvertToBtvector3(const SAGE::Math::Vector3& vec)
{
	return btVector3(vec.x, vec.y, vec.z);
}

inline SAGE::Math::Vector3 ConvertToVector3(const btVector3& vec)
{
	return { vec.x(), vec.y(), vec.z() };
}

inline SAGE::Graphics::Color ConvertToColor(const btVector3& color)
{
	return SAGE::Graphics::Color(color.x(), color.y(), color.z(), 1.0f);
}

inline btTransform ConvertToBtTransform(const SAGE::Graphics::Transform& t)
{
	return btTransform(btQuaternion(t.rotation.w, t.rotation.x, t.rotation.y, t.rotation.z), btVector3(t.position.x, t.position.y, t.position.z));
}

inline SAGE::Graphics::Transform ConvertToTransform(const btTransform& t, const SAGE::Math::Vector3& center)
{
	const auto& origin = t.getOrigin();
	const auto& rotation = t.getRotation();
	SAGE::Graphics::Transform retTrans;
	retTrans.position = SAGE::Math::Vector3(origin.x() - center.x, origin.y() - center.y, origin.z() - center.z);
	retTrans.rotation = SAGE::Math::Quaternion(rotation.w(), rotation.x(), rotation.y(), rotation.z());
	//retTrans.rotation = SAGE::Math::Quaternion(rotation.x(), rotation.y(), rotation.z(), rotation.w());
	return retTrans;
}

inline void ConvertToTransform(const btTransform& t, SAGE::Graphics::Transform& transform, const SAGE::Math::Vector3& center)
{
	const auto& origin = t.getOrigin();
	const auto& rotation = t.getRotation();
	transform.position = SAGE::Math::Vector3(origin.x() - center.x, origin.y() - center.y, origin.z() - center.z);
	transform.rotation = SAGE::Math::Quaternion(rotation.w(), rotation.x(), rotation.y(), rotation.z());
	//transform.rotation = SAGE::Math::Quaternion(rotation.x(), rotation.y(), rotation.z(), rotation.w());
}

