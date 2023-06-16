#pragma once

#include "CoroDefinition.h"
#include <set>

namespace SAGE::Coroutine
{
	class CoroutineSystem final
	{
	public:
		static void StaticInitialize();
		static void StaticTerminate();
		static CoroutineSystem* Get();

	public:
		CoroutineSystem() = default;
		~CoroutineSystem();

		CoroutineSystem(const CoroutineSystem&) = delete;
		CoroutineSystem& operator=(const CoroutineSystem&) = delete;

		void Initialize();
		void Terminate();

		void Update();

		CooperativeRoutines* StartCoroutine(Enumerator InEnumerator);
		void StopCoroutine(CooperativeRoutines* CoroutinePtr);
		void StopAllCoroutines();
		bool HasCoroutines();

	private:
		std::set<CooperativeRoutines*> Coroutines;

		CooperativeRoutines* CastAsCoroutine(YieldInstruction* YieldInstructionPtr);
		bool YieldCoroutine(CooperativeRoutines* CoroutinePtr);
		CooperativeRoutines* PushCoroutine(CooperativeRoutines* CoroutinePtr);
		CooperativeRoutines* PopCoroutine(CooperativeRoutines* CoroutinePtr);

	private: // public
		void ResumeCoroutines();
	};
}