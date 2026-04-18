#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Thread/Thread.hpp"

namespace hod::inline core
{
	/// @brief
	/// @return
	Thread::Id Thread::GetId() const
	{
		return _id;
	}
}
