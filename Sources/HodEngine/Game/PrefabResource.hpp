#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Resource.hpp>
#include "HodEngine/Game/Prefab.hpp"

namespace hod::game
{
	/// @brief 
	class HOD_GAME_API PrefabResource : public Resource
	{
		REFLECTED_CLASS(PrefabResource, Resource, HOD_GAME_API)

	public:
						PrefabResource() = default;
						PrefabResource(const PrefabResource&) = delete;
						PrefabResource(PrefabResource&&) = delete;
						~PrefabResource() override;

		PrefabResource&	operator=(const PrefabResource&) = delete;
		PrefabResource&	operator=(PrefabResource&&) = delete;

	public:

		bool			Initialize(const Document::Node& documentNode, FileSystem::Handle& fileHandle) override;

		Prefab&			GetPrefab();

	private:

		Prefab*			_prefab = nullptr;
	};
}
