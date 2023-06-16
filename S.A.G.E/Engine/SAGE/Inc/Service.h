#pragma once

#include "TypeIds.h"

namespace SAGE
{
	class GameWorld;

	class Service
	{
	public:
		Service() = default;
		virtual ~Service() = default;

		Service(const Service&) = delete;
		Service& operator=(const Service&) = delete;
		Service(Service&&) = delete;
		Service& operator=(Service&&) = delete;

		virtual uint32_t GetTypeId() const = 0;

		virtual void Initialize() {}
		virtual void Terminate() {}

		virtual void Update(float deltaTime) {}
		virtual void Render() {}
		virtual void DebugUI() {}

		GameWorld& GetWorld() { return *mWorld; }
		const GameWorld& GetWorld() const { return *mWorld; }

		void SetServiceName(const char* name) { mName = name; }
		const std::string& GetServiceName() const { return mName; }

	private:
		friend class GameWorld;
		GameWorld* mWorld = nullptr;
		std::string mName = "No Service Name";
	};
}