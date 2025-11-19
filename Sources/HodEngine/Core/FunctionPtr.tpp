#include <utility>

namespace hod
{
	template<typename Return, typename... Args>
	constexpr FunctionPtr<Return(Args...)>::FunctionPtr(std::nullptr_t) noexcept
	: _ptr(nullptr)
	{
	}

	template<typename Return, typename... Args>
	constexpr FunctionPtr<Return(Args...)>::FunctionPtr(FunctionPointerType f) noexcept
	: _ptr(f)
	{
	}

	template<typename Return, typename... Args>
	template<typename Lambda>
		requires std::is_convertible_v<Lambda, Return (*)(Args...)>
	constexpr FunctionPtr<Return(Args...)>::FunctionPtr(Lambda&& lambda) noexcept
	: _ptr(+std::forward<Lambda>(lambda))
	{
	}

	template<typename Return, typename... Args>
	constexpr FunctionPtr<Return(Args...)>& FunctionPtr<Return(Args...)>::operator=(FunctionPointerType f) noexcept
	{
		_ptr = f;
		return *this;
	}

	template<typename Return, typename... Args>
	constexpr FunctionPtr<Return(Args...)>& FunctionPtr<Return(Args...)>::operator=(std::nullptr_t) noexcept
	{
		_ptr = nullptr;
		return *this;
	}

	template<typename Return, typename... Args>
	template<typename Lambda>
		requires std::is_convertible_v<Lambda, Return (*)(Args...)>
	constexpr FunctionPtr<Return(Args...)>& FunctionPtr<Return(Args...)>::operator=(Lambda&& lambda) noexcept
	{
		_ptr = +std::forward<Lambda>(lambda);
		return *this;
	}

	template<typename Return, typename... Args>
	Return FunctionPtr<Return(Args...)>::operator()(Args... args) const
	{
		return _ptr(std::forward<Args>(args)...);
	}

	template<typename Return, typename... Args>
	constexpr FunctionPtr<Return(Args...)>::operator bool() const noexcept
	{
		return _ptr != nullptr;
	}

	template<typename Return, typename... Args>
	constexpr bool FunctionPtr<Return(Args...)>::operator!() const noexcept
	{
		return _ptr == nullptr;
	}

	template<typename Return, typename... Args>
	constexpr bool FunctionPtr<Return(Args...)>::operator==(const FunctionPtr& other) const noexcept
	{
		return _ptr == other._ptr;
	}

	template<typename Return, typename... Args>
	constexpr bool FunctionPtr<Return(Args...)>::operator!=(const FunctionPtr& other) const noexcept
	{
		return _ptr != other._ptr;
	}

	template<typename Return, typename... Args>
	constexpr bool FunctionPtr<Return(Args...)>::operator==(std::nullptr_t) const noexcept
	{
		return _ptr == nullptr;
	}

	template<typename Return, typename... Args>
	constexpr bool FunctionPtr<Return(Args...)>::operator!=(std::nullptr_t) const noexcept
	{
		return _ptr != nullptr;
	}

	template<typename Return, typename... Args>
	constexpr Return (*FunctionPtr<Return(Args...)>::Get() const noexcept)(Args...)
	{
		return _ptr;
	}

	template<typename Return, typename... Args>
	constexpr bool operator==(std::nullptr_t, const FunctionPtr<Return(Args...)>& fp) noexcept
	{
		return fp == nullptr;
	}

	template<typename Return, typename... Args>
	constexpr bool operator!=(std::nullptr_t, const FunctionPtr<Return(Args...)>& fp) noexcept
	{
		return fp != nullptr;
	}
}