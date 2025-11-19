#include "HodEngine/Physics/Pch.hpp"
#include "HodEngine/Physics/Collider.hpp"

namespace hod::physics
{
	/// @brief 
	/// @param body 
	Collider::Collider(Body* body)
	: _body(body)
	{
	}

	/// @brief 
	/// @return 
	Body* Collider::GetBody() const
	{
		return _body;
	}

	/// @brief 
	/// @param userData 
	void Collider::SetUserData(void* userData)
	{
		_userData = userData;
	}

	/// @brief 
	/// @return 
	void* Collider::GetUserData() const
	{
		return _userData;
	}
}
