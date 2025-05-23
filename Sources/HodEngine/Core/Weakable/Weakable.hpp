#pragma once
#include "HodEngine/Core/Export.hpp"

#include <memory>

namespace hod
{
	class WeakableAliveController;

	class HOD_CORE_API Weakable
	{
	public:
				Weakable();
		virtual	~Weakable();

		std::shared_ptr<WeakableAliveController>	GetAliveController() const;

	private:

		std::shared_ptr<WeakableAliveController>	_aliveController = nullptr;
	};
}
