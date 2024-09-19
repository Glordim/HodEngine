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

	/// @brief 
	/// @param callback 
	void Body::SetMoveEventCallback(std::function<void(const Vector2& position, float rotation)> callback)
	{
		_moveCallback = callback;
	}

	/// @brief 
	/// @return 
	const std::function<void(const Vector2& position, float rotation)>& Body::GetMoveEventCallback() const
	{
		return _moveCallback;
	}

	/// @brief 
	/// @return 
	const std::function<void(const Collision& collision)>& Body::GetCollisionEnterCallback() const
	{
		return _collisionEnterCallback;
	}

	/// @brief 
	/// @param callback 
	void Body::SetCollisionEnterCallback(std::function<void(const Collision&)> callback)
	{
		_collisionEnterCallback = callback;
	}

	/// @brief 
	/// @return 
	const std::function<void(const Collision& collision)>& Body::GetCollisionExitCallback() const
	{
		return _collisionExitCallback;
	}

	/// @brief 
	/// @param callback 
	void Body::SetCollisionExitCallback(std::function<void(const Collision&)> callback)
	{
		_collisionExitCallback = callback;
	}
}
