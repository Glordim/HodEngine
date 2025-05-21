#pragma once
#include "HodEngine/Physics/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>

#include <vector>
#include <cstdint>

namespace hod::physics
{
	class World;

	/// @brief 
	class HOD_PHYSICS_API Physics
	{
		_SingletonAbstract(Physics)

	public:

		virtual				~Physics();

	public:

		virtual bool		Init() = 0;
		void				Clear();

		virtual World*		CreateWorld() = 0;

	public:

		static Physics*		CreatePhysicsInstance();
		static void			DestroyPhysicsInstance();

	protected:

		std::vector<World*>	_worlds;
	};
}
