#pragma once
#include "HodEngine/Core/Export.hpp"

namespace hod
{
	class HOD_CORE_API WeakableAliveController
	{
	public:

		void	MarkAsDead();
		bool	IsAlive() const;

	private:

		bool	_alive = true;
	};
}
