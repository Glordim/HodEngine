#pragma once
#include "HodEngine/Application/Export.hpp"

#include <HodEngine/Core/Vector.hpp>

#include <functional>

namespace hod::inline application
{
	class HOD_APPLICATION_API InitGuard
	{
	public:
		~InitGuard() { Unwind(); }

		bool Push(std::function<bool()> init, std::function<void()> terminate)
		{
			if (init() == false)
			{
				Unwind();
				return false;
			}
			_layers.push_back(std::move(terminate));
			return true;
		}

	private:
		void Unwind()
		{
			for (auto it = _layers.RBegin(); it != _layers.REnd(); ++it)
				(*it)();
			_layers.Clear();
		}

		Vector<std::function<void()>> _layers;
	};
}
