#pragma once
#include "HodEngine/Core/Export.hpp"

#include <functional>

namespace hod::inline core
{
	class HOD_CORE_API ScopedGuard
	{
	public:
		template<typename F>
		ScopedGuard(F&& function) : _function(std::move(function)) {}
		ScopedGuard(const ScopedGuard&) = delete;
		ScopedGuard(ScopedGuard&&) = delete;
		~ScopedGuard() { if (_function) _function(); }

		ScopedGuard& operator=(const ScopedGuard&) = delete;
		ScopedGuard& operator=(ScopedGuard&&) = delete;

		void Disable() { _function = nullptr; }

	private:
		std::function<void()> _function;
	};
}
