#pragma once
#include "HodEngine/Physics/Export.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"

namespace hod::physics
{
	class Collider;

	/// @brief 
	class HOD_PHYSICS_API Collision
	{
	public:
		Vector2		_normal;
		Collider*	_colliderA = nullptr;
		Collider*	_colliderB = nullptr;
	};
}
