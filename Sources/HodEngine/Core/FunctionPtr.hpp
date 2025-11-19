#pragma once

#include <type_traits>

namespace hod
{
	template<typename>
	class FunctionPtr;

	template<typename Return, typename... Args>
	class FunctionPtr<Return(Args...)>
	{
	public:
		using FunctionPointerType = Return (*)(Args...);

	public:
		constexpr FunctionPtr() noexcept = default;
		constexpr FunctionPtr(std::nullptr_t) noexcept;
		constexpr FunctionPtr(FunctionPointerType f) noexcept;

		template<typename Lambda>
			requires std::is_convertible_v<Lambda, FunctionPointerType>
		constexpr FunctionPtr(Lambda&& lambda) noexcept;

		constexpr FunctionPtr& operator=(FunctionPointerType f) noexcept;
		constexpr FunctionPtr& operator=(std::nullptr_t) noexcept;

		template<typename Lambda>
			requires std::is_convertible_v<Lambda, FunctionPointerType>
		constexpr FunctionPtr& operator=(Lambda&& lambda) noexcept;

		Return             operator()(Args... args) const;
		constexpr explicit operator bool() const noexcept;
		constexpr bool     operator!() const noexcept;

		constexpr bool operator==(const FunctionPtr& other) const noexcept;
		constexpr bool operator!=(const FunctionPtr& other) const noexcept;
		constexpr bool operator==(std::nullptr_t) const noexcept;
		constexpr bool operator!=(std::nullptr_t) const noexcept;

		constexpr FunctionPointerType Get() const noexcept;

	private:
		FunctionPointerType _ptr = nullptr;
	};
}

#include "FunctionPtr.tpp"