#include "Precompiled.h"
#include "PhysicsDebugDraw.h"

#include "GameWorld.h"
#include "CameraService.h"
#include "Graphics/Inc/SimpleDraw.h"

using namespace SAGE;
using namespace SAGE::Graphics;

void PhysicsDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	SimpleDraw::AddLine(ConvertToVector3(from), ConvertToVector3(to), ConvertToColor(color));
}

void PhysicsDebugDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	// TODO: 
}

void PhysicsDebugDraw::reportErrorWarning(const char* warningString)
{
	LOG("[Physics Warning] %s", warningString);
}

void PhysicsDebugDraw::draw3dText(const btVector3& location, const char* textString)
{
	// TODO: 
}

void PhysicsDebugDraw::setDebugMode(int debugMode)
{
	mDebugMode = debugMode;
}

int PhysicsDebugDraw::getDebugMode() const
{
	return mDebugMode;
}

void PhysicsDebugDraw::Render()
{
	SimpleDraw::Render(mGameWorld->GetService<CameraService>()->GetCamera());
}