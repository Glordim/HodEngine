#pragma once
#include "HodEngine/Physics/Export.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"

namespace hod::physics
{
	class Collider;

	/// @brief 
	struct HOD_PHYSICS_API Collision
	{
		Collider&	_colliderA;
		Collider&	_colliderB;
		Vector2		_normal;
	};
}
