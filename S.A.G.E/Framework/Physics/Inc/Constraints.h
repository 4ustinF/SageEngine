#pragma once

#include "Particle.h"

namespace SAGE::Physics
{
	class Constraints
	{
	public:
		virtual ~Constraints() = default;

		virtual void Apply() const = 0;
		virtual void DebugDraw() const {}
	};

	class Fixed : public Constraints
	{
	public:
		Fixed(Particle* p);
		Fixed(Particle* p, const Math::Vector3& position);

		void Apply() const override;
		void DebugDraw() const override;
	protected:
		Particle* mParticle;
		Math::Vector3 mPosition;
	};

	class Spring : public Constraints
	{
	public:
		Spring(Particle* a, Particle* b, float restLength = 0.0f);

		void Apply() const override;
		void DebugDraw() const override;
	protected:
		Particle* mParticleA;
		Particle* mParticleB;
		float mRestLength;
	};
}