#pragma once
#include <cstdint>
#include <functional>

template <auto Func>
struct MemberFuncT
{
	explicit MemberFuncT() = default;
};

template <auto Func>
constexpr MemberFuncT<Func> MemberFunc{};

#ifndef FUNCTION_NO_ALLOC
struct VirtualClosure
{
	virtual ~VirtualClosure() {}
};

template <class T, class Result, class... Args>
struct CapturingClosure;

template <class Result, class... Args>
class Function;

template <class Result, class... Args>
class Function<Result(Args...)>
{
	static constexpr uintptr_t DYNAMIC = ~0;

public:
	using Func = Result(Args...);
	using Wrap = Result(void*, Args...);

	Function() : wrap(nullptr), param(nullptr)
	{
	}

	~Function()
	{
		if (reinterpret_cast<uintptr_t>(wrap) == DYNAMIC)
			delete reinterpret_cast<VirtualClosure*>(param);
	}

	Function(const Function& other)
	{
		wrap = other.wrap;
		param = other.param;

		if (reinterpret_cast<uintptr_t>(wrap) == DYNAMIC)
		{
			auto* closure = reinterpret_cast<CapturingClosure<void, Result, Args...>*>(param);
			param = closure->Copy();
		}
	}

	Function(Function&& other)
	{
		wrap = other.wrap;
		param = other.param;
		other.wrap = nullptr;
		other.param = nullptr;
	}

	template <class Fp>
	Function(Fp func)
	{
		if constexpr (std::is_pointer_v<Fp>)
		{
			param = reinterpret_cast<void*>(func);
			wrap = [](void* param, Args... args)
				{
					auto func = reinterpret_cast<Func*>(param);
					return std::invoke(func, std::forward<Args>(args)...);
				};
		}
		else if constexpr (std::is_class_v<Fp> && std::is_copy_constructible_v<Fp>)
		{
			param = new CapturingClosure<Fp, Result, Args...>(func);
			wrap = reinterpret_cast<Wrap*>(DYNAMIC);
		}
		else
			static_assert(false, "Function supports only non/member functions and non/capturing lambdas");
	}

	template <class FpT, FpT Fp, class Ip>
	Function(MemberFuncT<Fp>, Ip* inst)
	{
		static_assert(std::is_member_function_pointer_v<FpT>, "Member function pointer is ill-formed");
		static_assert(std::is_class_v<Ip>, "Object pointer is ill-formed");

		param = inst;
		wrap = [](void* param, Args... args)
			{
				auto This = reinterpret_cast<Ip*>(param);
				return std::invoke(Fp, This, std::forward<Args>(args)...);
			};
	}

	void reset()
	{
		if (reinterpret_cast<uintptr_t>(wrap) == DYNAMIC)
			delete reinterpret_cast<VirtualClosure*>(param);

		wrap = nullptr;
		param = nullptr;
	}

	explicit operator bool() const
	{
		return wrap;
	}

	Result operator()(Args... args) const
	{
		if (reinterpret_cast<uintptr_t>(wrap) == DYNAMIC)
		{
			auto* closure = reinterpret_cast<CapturingClosure<void, Result, Args...>*>(param);
			auto fn = closure->GetFunction();
			return fn(std::forward<Args>(args)...);
		}
		return wrap(param, std::forward<Args>(args)...);
	}

	//unofficial amendments
	Function& operator=(const Function& other)
	{
		if (reinterpret_cast<uintptr_t>(wrap) == DYNAMIC)
			delete reinterpret_cast<VirtualClosure*>(param);

		wrap = other.wrap;
		param = other.param;

		if (reinterpret_cast<uintptr_t>(wrap) == DYNAMIC)
		{
			auto* closure = reinterpret_cast<CapturingClosure<void, Result, Args...>*>(param);
			param = closure->Copy();
		}

		return *this;
	}

	Function& operator=(Function&& other)
	{
		if (reinterpret_cast<uintptr_t>(wrap) == DYNAMIC)
			delete reinterpret_cast<VirtualClosure*>(param);

		wrap = other.wrap;
		param = other.param;
		other.wrap = nullptr;
		other.param = nullptr;

		return *this;
	}

private:
	Wrap* wrap;
	void* param;
};
#else
#define Function FunctionS
#endif

template <class Result, class... Args>
class FunctionS;

template <class Result, class... Args>
class FunctionS<Result(Args...)>
{
public:
	using Func = Result(Args...);
	using Wrap = Result(void*, Args...);

	FunctionS() : wrap(nullptr), param(nullptr)
	{
	}

	template <class Fp>
	FunctionS(Fp func)
	{
		if constexpr (std::is_convertible_v<Fp, void*>)
		{
			param = func;
			wrap = [](void* param, Args... args)
				{
					auto func = reinterpret_cast<Func*>(param);
					return std::invoke(func, std::forward<Args>(args)...);
				};
		}
		else
			static_assert(false, "FunctionS supports only non/member functions and non-capturing lambdas");
	}

	template <class FpT, FpT Fp, class Ip>
	FunctionS(MemberFuncT<Fp>, Ip* inst)
	{
		static_assert(std::is_member_function_pointer_v<FpT>, "Member function pointer is ill-formed");
		static_assert(std::is_class_v<Ip>, "Object pointer is ill-formed");

		param = inst;
		wrap = [](void* param, Args... args)
			{
				auto This = reinterpret_cast<Ip*>(param);
				return std::invoke(Fp, This, std::forward<Args>(args)...);
			};
	}

	void reset()
	{
		wrap = nullptr;
		param = nullptr;
	}

	explicit operator bool() const
	{
		return wrap;
	}

	Result operator()(Args... args) const
	{
		return wrap(param, std::forward<Args>(args)...);
	}

	bool operator==(const FunctionS& other) const
	{
		return wrap == other.wrap && param == other.param;
	}

private:
	Wrap* wrap;
	void* param;
};

#ifndef FUNCTION_NO_ALLOC
template <class T, class Result, class... Args>
struct CapturingClosure : VirtualClosure
{
	T* closure;

	CapturingClosure(T& closure)
	{
		this->closure = new T(closure);
	}

	~CapturingClosure() override
	{
		delete closure;
	}

	virtual Function<Result(Args...)> GetFunction()
	{
		return { MemberFunc<&T::operator()>, closure };
	}

	virtual CapturingClosure* Copy()
	{
		return new CapturingClosure(*closure);
	}

	CapturingClosure(CapturingClosure&) = delete;
	CapturingClosure(CapturingClosure&&) = delete;
	CapturingClosure& operator=(CapturingClosure&) = delete;
	CapturingClosure& operator=(CapturingClosure&&) = delete;
};

template <class Result, class... Args>
struct CapturingClosure<void, Result, Args...> : VirtualClosure
{
	void* closure;

	CapturingClosure() : closure(nullptr)
	{
	}

	virtual ~CapturingClosure() = 0;
	virtual Function<Result(Args...)> GetFunction() = 0;
	virtual CapturingClosure* Copy() = 0;

	CapturingClosure(CapturingClosure&) = delete;
	CapturingClosure(CapturingClosure&&) = delete;
	CapturingClosure& operator=(CapturingClosure&) = delete;
	CapturingClosure& operator=(CapturingClosure&&) = delete;
};
#endif
