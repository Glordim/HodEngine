#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/WorldSystem.hpp"
#include "HodEngine/Game/World.hpp"

#include <algorithm>

namespace hod::inline game
{
	namespace
	{
		Vector<const WorldSystemType*>& WorldSystemTypes()
		{
			static Vector<const WorldSystemType*> types;
			return types;
		}

		Vector<World*>& LiveWorlds()
		{
			static Vector<World*> worlds;
			return worlds;
		}
	}

	/// @brief
	/// @param world
	WorldSystem::WorldSystem(World& world)
	: _world(world)
	{
	}

	/// @brief
	/// @param fixedDeltaTime
	void WorldSystem::OnFixedUpdate(float fixedDeltaTime)
	{
		(void)fixedDeltaTime;
	}

	/// @brief
	/// @param deltaTime
	void WorldSystem::OnUpdate(float deltaTime)
	{
		(void)deltaTime;
	}

	/// @brief
	/// @return
	World& WorldSystem::GetWorld() const
	{
		return _world;
	}

	/// @brief
	/// @param type
	/// @return
	bool WorldSystemRegistry::Register(const WorldSystemType& type)
	{
		if (std::find(WorldSystemTypes().Begin(), WorldSystemTypes().End(), &type) != WorldSystemTypes().End())
		{
			OUTPUT_ERROR("WorldSystemRegistry: '{}' is registered already", type._name);
			return false;
		}
		WorldSystemTypes().PushBack(&type);
		for (World* world : LiveWorlds())
		{
			world->AddSystem(type);
		}
		return true;
	}

	/// @brief
	/// @param type
	/// @return
	bool WorldSystemRegistry::Unregister(const WorldSystemType& type)
	{
		auto it = std::find(WorldSystemTypes().Begin(), WorldSystemTypes().End(), &type);
		if (it == WorldSystemTypes().End())
		{
			return false;
		}
		for (World* world : LiveWorlds())
		{
			world->RemoveSystem(type);
		}
		WorldSystemTypes().Erase(it);
		return true;
	}

	/// @brief
	/// @return
	const Vector<const WorldSystemType*>& WorldSystemRegistry::GetTypes()
	{
		return WorldSystemTypes();
	}

	/// @brief
	/// @param world
	void WorldSystemRegistry::AddWorld(World& world)
	{
		LiveWorlds().PushBack(&world);
	}

	/// @brief
	/// @param world
	void WorldSystemRegistry::RemoveWorld(World& world)
	{
		auto it = std::find(LiveWorlds().Begin(), LiveWorlds().End(), &world);
		if (it != LiveWorlds().End())
		{
			LiveWorlds().Erase(it);
		}
	}
}
