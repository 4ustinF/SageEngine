#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

namespace SAGE::Core::Delegate // TODO: Maybe move to their own framework?
{
	struct FDelegateHandle {
	private:
		uint64_t ID = 0;
		static uint64_t GenerateNewID() {
			static uint64_t CurrentID = 0;
			return ++CurrentID;
		}

	public:
		FDelegateHandle() : ID(0) {}
		void AssignNewID() { ID = GenerateNewID(); }
		bool IsValid() const { return ID != 0; }
		uint64_t GetID() const { return ID; }

		bool operator==(const FDelegateHandle& Other) const { return ID == Other.ID; }
	};

	// Base interface for callable targets (hides the underlying object type)
	template<typename... Args>
	class IDelegateInstance {
	public:
		virtual ~IDelegateInstance() = default;
		virtual void Execute(Args... args) = 0;
		virtual bool IsValid() const = 0;
		virtual const void* GetOwner() const = 0; // Used to identify and unbind objects
		virtual FDelegateHandle GetHandle() const = 0;
	};

	template<typename T, typename... Args>
	class TRawDelegateInstance : public IDelegateInstance<Args...> {
	private:
		using MemberFunc = void (T::*)(Args...);
		T* ObjectRawPtr;
		MemberFunc Method;
		FDelegateHandle Handle;

	public:
		TRawDelegateInstance(T* InObj, MemberFunc InMethod)
			: ObjectRawPtr(InObj), Method(InMethod) {
			Handle.AssignNewID();
		}

		void Execute(Args... args) override {
			if (ObjectRawPtr) {
				(ObjectRawPtr->*Method)(args...);
			}
		}

		// Always valid because a raw pointer cannot automatically detect its own death
		bool IsValid() const override { return ObjectRawPtr != nullptr; }
		const void* GetOwner() const override { return static_cast<const void*>(ObjectRawPtr); }
		FDelegateHandle GetHandle() const override { return Handle; }
	};

	// Concrete implementation tracking a specific Object type
	template<typename T, typename... Args>
	class TMemberDelegateInstance : public IDelegateInstance<Args...> {
	private:
		using MemberFunc = void (T::*)(Args...);
		std::weak_ptr<T> ObjectPtr; // Prevents crashing if the object dies!
		MemberFunc Method;
		FDelegateHandle Handle;

	public:
		TMemberDelegateInstance(std::shared_ptr<T> InObj, MemberFunc InMethod)
			: ObjectPtr(InObj), Method(InMethod)
		{
			Handle.AssignNewID();
		}

		void Execute(Args... args) override {
			if (auto pinned = ObjectPtr.lock()) {
				(pinned.get()->*Method)(args...);
			}
		}

		bool IsValid() const override { return !ObjectPtr.expired(); }
		const void* GetOwner() const override { return ObjectPtr.lock().get(); }
		FDelegateHandle GetHandle() const override { return Handle; }
	};

	template<typename RetType, typename... Args>
	class TDelegate {
	private:
		std::unique_ptr<IDelegateInstance<RetType, Args...>> Instance;

	public:
		TDelegate() = default;

		// --- SMART POINTER BINDING ---
		template<typename T>
		FDelegateHandle BindSP(std::shared_ptr<T> InObj, RetType(T::* InMethod)(Args...)) {
			auto NewInstance = std::make_unique<TMemberDelegateInstance<T, RetType, Args...>>(InObj, InMethod);
			FDelegateHandle Handle = NewInstance->GetHandle();
			Instance = std::move(NewInstance);
			return Handle;
		}

		// --- RAW POINTER BINDING
		template<typename T>
		FDelegateHandle BindRaw(T* InObj, RetType(T::* InMethod)(Args...)) {
			auto NewInstance = std::make_unique<TRawDelegateInstance<T, RetType, Args...>>(InObj, InMethod);
			FDelegateHandle Handle = NewInstance->GetHandle();
			Instance = std::move(NewInstance);
			return Handle;
		}

		// --- LIFETIME & VALIDITY CHECKS ---
		void Unbind() { Instance.reset(); }

		bool IsBound() const {
			return Instance && Instance->IsValid();
		}

		// --- EXECUTION (With default return value if unbound) ---
		RetType Execute(Args... args) const {
			// Enforce a hard crash or assertion here if called while unbound, 
			// matching Unreal Engine behavior.
			return Instance->Execute(args...);
		}

		// Safely executes if valid; returns a default-constructed RetType if empty
		RetType ExecuteIfBound(Args... args) const {
			if (IsBound()) {
				return Instance->Execute(args...);
			}
			return RetType(); // Returns 0, false, nullptr, etc.
		}
	};

	template<typename... Args>
	class TMulticastDelegate {
	private:
		std::vector<std::unique_ptr<IDelegateInstance<Args...>>> InvocationList;
	public:
		template<typename T>
		FDelegateHandle AddSP(std::shared_ptr<T> InObj, void (T::* InMethod)(Args...)) {
			auto NewInstance = std::make_unique<TMemberDelegateInstance<T, Args...>>(InObj, InMethod);
			FDelegateHandle Handle = NewInstance->GetHandle();
			InvocationList.push_back(std::move(NewInstance));
			return Handle;
		}

		template<typename T>
		FDelegateHandle AddRaw(T* InObj, void (T::* InMethod)(Args...)) {
			auto NewInstance = std::make_unique<TRawDelegateInstance<T, Args...>>(InObj, InMethod);
			FDelegateHandle Handle = NewInstance->GetHandle();
			InvocationList.push_back(std::move(NewInstance));
			return Handle;
		}

		// REMOVE BY HANDLE
		void Remove(FDelegateHandle Handle) {
			if (!Handle.IsValid()) return;

			InvocationList.erase(
				std::remove_if(InvocationList.begin(), InvocationList.end(),
					[Handle](const auto& item) { return item->GetHandle() == Handle; }),
				InvocationList.end());
		}

		void Broadcast(Args... args) {
			// 1. Automatically prune any dead weak_ptrs from the invocation list
			InvocationList.erase(
				std::remove_if(InvocationList.begin(), InvocationList.end(),
					[](const auto& item) { return !item->IsValid(); }),
				InvocationList.end());

			// 2. Safely fire off all remaining active listeners
			for (auto& listener : InvocationList) {
				listener->Execute(args...);
			}
		}
	};

	// Macros mapping to your template implementation
#define DECLARE_DELEGATE_OneParam(DelegateName, ParamType1) \
    using DelegateName = SAGE::Core::Delegate::TDelegate<ParamType1>;

#define DECLARE_MULTICAST_DELEGATE_OneParam(DelegateName, ParamType1) \
    using DelegateName = SAGE::Core::Delegate::TMulticastDelegate<ParamType1>;
}