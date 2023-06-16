#pragma once

namespace DirectX { class SoundEffect; class SoundEffectInstance; }

namespace SAGE::Graphics
{
	using SoundId = std::size_t;
	class SoundEffectManager
	{
	public:
		static void StaticInitialize(const char* root);
		static void StaticTerminate();
		static SoundEffectManager* Get();

	public:
		SoundEffectManager();
		~SoundEffectManager();

		SoundEffectManager(const SoundEffectManager&) = delete;
		SoundEffectManager& operator=(const SoundEffectManager&) = delete;

		void SetRootPath(const char* path);

		SoundId Load(const char* fileName);
		void Clear();

		void Play(SoundId id, bool loop = false, float volume = 1.0f, float pitch = 0.0f, float pan = 0.0f);
		void Stop(SoundId id);

	private:
		struct Entry
		{
			std::unique_ptr<DirectX::SoundEffect> effect;
			std::unique_ptr<DirectX::SoundEffectInstance> instance;
		};

		std::string mRoot;
		std::unordered_map<std::size_t, std::unique_ptr<Entry>> mInventory;
	};
}
