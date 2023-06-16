#pragma once

namespace SAGE
{
	class GameObject;

	using MakeOverride = std::function<bool(const char*, const rapidjson::Value&, GameObject&)>;

	namespace GameObjectFactory
	{
		void SetMakeOverride(MakeOverride makeOverride);
		void Make(std::filesystem::path templateFile, GameObject& gameObject);
	};
}