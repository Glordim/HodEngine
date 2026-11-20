#pragma once
#include "HodEngine/Core/Export.hpp"

#include <memory>

namespace hod::inline core
{
	class WeakableAliveController;

	class HOD_CORE_API Weakable
	{
	public:
		Weakable();
		/// @brief A copy is another object: its own alive controller, the references to the original stay the original's
		Weakable(const Weakable& copy);
		virtual ~Weakable();

		/// @brief Keeps its own alive controller: the references to it stay valid
		Weakable& operator=(const Weakable& copy);

		std::shared_ptr<WeakableAliveController> GetAliveController() const;

	private:
		std::shared_ptr<WeakableAliveController> _aliveController = nullptr;
	};
}
