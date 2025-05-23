#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Document/Document.hpp>

#include "HodEngine/Game/Entity.hpp"

#include <map>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

namespace hod::game::PrefabUtility
{
	struct HOD_GAME_API PrefabOverride
	{
		enum class Type
		{
			Entity,
			Component,
		};

		ReflectionProperty*		_reflectionProperty = nullptr;
		void*					_source = nullptr;
		void*					_instance = nullptr;
		void*					_effectiveSourceAddr = nullptr;
		void*					_effectiveInstanceAddr = nullptr;
		Type					_type;
		std::string				_path;
		std::vector<uint64_t>	_target;
	};

	HOD_GAME_API bool CollectPrefabOverride(Entity* prefabInstance, std::vector<PrefabOverride>& overrides);

	HOD_GAME_API Entity* GetPrefabInstance(Entity* entity);
	HOD_GAME_API std::string GetRelativePath(Entity* parent, Entity* child);
	HOD_GAME_API Entity* FindChildByPath(Entity* parent, std::string_view relativePath);

	HOD_GAME_API Component* GetCorrespondingComponent(Component* component);
}
