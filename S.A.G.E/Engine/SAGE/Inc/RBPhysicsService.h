#pragma once

#include "Service.h"

namespace SAGE
{
	class RBPhysicsService final : public Service
	{
	public:
		SET_TYPE_ID(ServiceId::RBPhysics)

		void Initialize() override;
		void Terminate() override;
		void Update(float deltaTime) override;
		void Render() override;
		void DebugUI() override;

		void DebugInput(); // TODO: Remove

		// TODO: Game Objects will need to sub to this service and pass on there PhysicsObjectComp
		// This comp will contain a physics object and do what it needs to accordingly.

		// For now lets create a physics object here and just do a debug draw of its location.
		// TODO: Remove
		// TODO: When this is in the comp it should be a ptr
		//SAGE::RBPhysics::RBPhysicsObject* mPhysicsObject1;
		//SAGE::RBPhysics::RBPhysicsObject* mPhysicsObject2;

		SAGE::RBPhysics::RBPhysicsWorld& GetPhysicsWorld() { return mPhysicsWorld; }

	private:

		SAGE::RBPhysics::RBPhysicsWorld mPhysicsWorld;
		bool mRenderDebugUI = false;
	};
}