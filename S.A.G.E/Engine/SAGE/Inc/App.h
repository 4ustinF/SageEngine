#pragma once

namespace SAGE
{
	class AppState;

	struct AppConfig
	{
		std::wstring appName = L"SAGE";
		uint32_t winWidth = 1280;
		uint32_t winHeight = 720;
		uint32_t debugDrawLimit = 1000000;
		std::filesystem::path textureRoot = "../../Assets/Images";
		std::filesystem::path audioRoot = "../../Assets/Audio";
	};

	class App final
	{
	public:
		template<class StateType>
		void AddState(std::string stateName)
		{
			static_assert(std::is_base_of_v<AppState, StateType>,
				"App -- 'StateType' must be derived from 'AppState'");

			auto [iter, success] = mAppStates.try_emplace(std::move(stateName), nullptr); //Structerd binding
			if (success)
			{
				auto& ptr = iter->second;
				ptr = std::make_unique<StateType>();
				if (mCurrentState == nullptr)
				{
					LOG("App -- Current State: %s", iter->first.c_str());
					mCurrentState = ptr.get();
				}
			}
		}

		void ChangeState(const std::string& stateName);

		void Run(AppConfig appConfig);
		void Quit();

	private:
		using AppStateMap = std::map<std::string, std::unique_ptr<AppState>>;

		AppStateMap mAppStates;
		AppState* mCurrentState = nullptr;
		AppState* mNextState = nullptr;

		bool mRunning = false;
	};
}