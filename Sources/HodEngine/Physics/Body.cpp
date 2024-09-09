#include "HodEngine/Physics/Pch.hpp"
#include "HodEngine/Physics/Body.hpp"
#include "HodEngine/Physics/Collider.hpp"

namespace hod::physics
{
	Body::~Body()
	{
		for (Collider* collider : _colliders)
		{
			delete collider;
		}
	}
}
