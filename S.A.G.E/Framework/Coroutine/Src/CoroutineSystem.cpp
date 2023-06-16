#include "Precompiled.h"
#include "CoroutineSystem.h"

using namespace SAGE;
using namespace SAGE::Coroutine;

namespace
{
	std::unique_ptr<CoroutineSystem> sCoroutineSystem;
}

void CoroutineSystem::StaticInitialize()
{
	ASSERT(sCoroutineSystem == nullptr, "CoroutineSystem -- System already initialized!");
	sCoroutineSystem = std::make_unique<CoroutineSystem>();
	sCoroutineSystem->Initialize();
}

void CoroutineSystem::StaticTerminate()
{
	if (sCoroutineSystem != nullptr)
	{
		sCoroutineSystem->Terminate();
		sCoroutineSystem.reset();
	}
}

CoroutineSystem* CoroutineSystem::Get()
{
	ASSERT(sCoroutineSystem != nullptr, "CoroutineSystem -- No system registered.");
	return sCoroutineSystem.get();
}

CoroutineSystem::~CoroutineSystem()
{
	StopAllCoroutines();
	ResumeCoroutines();
}

void CoroutineSystem::Initialize()
{
}

void CoroutineSystem::Terminate()
{
	LOG("CoroutineSystem -- Terminating...");
	LOG("CoroutineSystem -- System terminated.");
}

void CoroutineSystem::Update()
{
	if (sCoroutineSystem->HasCoroutines())
	{
		sCoroutineSystem->ResumeCoroutines();
	}
}

CooperativeRoutines* CoroutineSystem::StartCoroutine(Enumerator InEnumerator)
{
	CoroPull Pull(InEnumerator);
	if (Pull)
	{
		return PushCoroutine(new CooperativeRoutines(Pull));
	}
	return nullptr;
}

void CoroutineSystem::StopCoroutine(CooperativeRoutines* CoroutinePtr)
{
	YieldInstruction* YieldReturnPtr = nullptr;
	while (CoroutinePtr != nullptr)
	{
		YieldReturnPtr = CoroutinePtr->GetYieldReturn();
		CoroutinePtr->End();
		CoroutinePtr = CastAsCoroutine(YieldReturnPtr);
	}

	if (YieldReturnPtr != nullptr)
	{
		YieldReturnPtr->End();
	}
}

void CoroutineSystem::StopAllCoroutines()
{
	for (CooperativeRoutines* CoroutinePtr : Coroutines)
	{
		StopCoroutine(CoroutinePtr);
	}
}

bool CoroutineSystem::HasCoroutines()
{
	return !Coroutines.empty();
}

CooperativeRoutines* CoroutineSystem::CastAsCoroutine(YieldInstruction* YieldInstructionPtr)
{
	if (YieldInstructionPtr == nullptr || YieldInstructionPtr->Type != TYPEID(CooperativeRoutines))
	{
		return nullptr;
	}
	return static_cast<CooperativeRoutines*>(YieldInstructionPtr);
}

bool CoroutineSystem::YieldCoroutine(CooperativeRoutines* CoroutinePtr)
{
	YieldInstruction* YieldReturnPtr = CoroutinePtr->GetYieldReturn();
	if (YieldReturnPtr != nullptr)
	{
		if (YieldReturnPtr->IsEnded())
		{
			delete YieldReturnPtr;
		}
		else
		{
			Coroutines.insert(CoroutinePtr);
			return true;
		}
	}
	return false;
}

CooperativeRoutines* CoroutineSystem::PushCoroutine(CooperativeRoutines* CoroutinePtr)
{
	CooperativeRoutines* YieldReturnPtr = CastAsCoroutine(CoroutinePtr->GetYieldReturn());
	if (YieldReturnPtr == nullptr)
	{
		Coroutines.insert(CoroutinePtr);
	}
	else
	{
		YieldReturnPtr->NextPtr = CoroutinePtr;
	}
	return CoroutinePtr;
}

CooperativeRoutines* CoroutineSystem::PopCoroutine(CooperativeRoutines* CoroutinePtr)
{
	CooperativeRoutines* NextPtr = CoroutinePtr->NextPtr;
	delete CoroutinePtr;
	return NextPtr;
}

void CoroutineSystem::ResumeCoroutines()
{
	std::set<CooperativeRoutines*> CoroutinesCopy;
	CoroutinesCopy.swap(Coroutines);

	for (CooperativeRoutines* CoroutinePtr : CoroutinesCopy)
	{
		if (YieldCoroutine(CoroutinePtr))
		{
			continue;
		}

		while (CoroutinePtr != nullptr)
		{
			CoroutinePtr->Resume();
			if (!CoroutinePtr->IsEnded())
			{
				PushCoroutine(CoroutinePtr);
				break;
			}
			CoroutinePtr = PopCoroutine(CoroutinePtr);
		}
	}
}