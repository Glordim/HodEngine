#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Vector.hpp>

namespace hod::inline game
{
	class World;
	class WorldSystem;

	/// @brief A kind of WorldSystem, registered by a runtime module (see RuntimeModuleRegistration::RegisterWorldSystem): each World gets one
	/// instance of every registered type, the existing worlds included, in the registration order (the modules' order: dependencies first),
	/// destroyed in the reverse order. Identified by its address: define it once, in the module's code (a static of a function).
	struct HOD_GAME_API WorldSystemType
	{
		const char* _name = nullptr;
		WorldSystem* (*_create)(World& world) = nullptr;
	};

	/// @brief What a module adds to every World (the physics simulation...), without the World knowing it (see WorldSystemType)
	class HOD_GAME_API WorldSystem
	{
	public:
		explicit WorldSystem(World& world);
		WorldSystem(const WorldSystem&) = delete;
		WorldSystem(WorldSystem&&) = delete;
		virtual ~WorldSystem() = default;

		WorldSystem& operator=(const WorldSystem&) = delete;
		WorldSystem& operator=(WorldSystem&&) = delete;

	public:
		/// @brief Each fixed step (see World::GetFixedTimestep), after the scenes' FixedUpdate
		virtual void OnFixedUpdate(float fixedDeltaTime);

		/// @brief Each frame the world is updated, after the scenes' Update
		virtual void OnUpdate(float deltaTime);

		World& GetWorld() const;

	private:
		World& _world;
	};

	/// @brief The registered WorldSystemType, and the live worlds they are instantiated in
	class HOD_GAME_API WorldSystemRegistry
	{
	public:
		/// @brief Instantiated in the existing worlds right away
		/// @return false (with an error) if registered already
		static bool Register(const WorldSystemType& type);

		/// @brief Its instances are destroyed first, in every world
		static bool Unregister(const WorldSystemType& type);

		static const Vector<const WorldSystemType*>& GetTypes();

	private:
		friend class World;

		static void AddWorld(World& world);
		static void RemoveWorld(World& world);
	};
}
