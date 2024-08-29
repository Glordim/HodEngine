#pragma once
#include "HodEngine/Physics/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>

#include <vector>
#include <cstdint>

namespace hod
{
	class Vector2;
}

namespace hod::physics
{
	class Body;
	//class DebugDrawer;

	/// @brief 
	struct HOD_PHYSICS_API RaycastResult
	{
		Body* _bodyCollided = nullptr;
	};

	/// @brief 
	class HOD_PHYSICS_API Physics
	{
		_SingletonAbstract(Physics)

	public:

		enum DebugDrawFlag
		{
			Shape = 0,
			Join,
			AABB,
			Pair,
			CenterOfMass
		};

	protected:

		virtual				~Physics();

	public:

		virtual bool		Init() = 0;
		void				Clear();

		virtual Body*		CreateBody() = 0;
		virtual void		DeleteBody(Body* body) = 0;

		virtual void		Update(float dt) = 0;

		virtual bool		Raycast(const Vector2& origin, const Vector2& dir, float distance, physics::RaycastResult& result) = 0;

		//void				SetDebugDrawer(b2Draw* debugDrawer);
		//void				SetDebugDrawFlags(DebugDrawFlag flag, bool enabled);

	private:

		std::vector<Body*>	_bodies;
	};
}
