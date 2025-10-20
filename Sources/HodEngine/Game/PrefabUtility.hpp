#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

#include "HodEngine/Game/Entity.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"
#include <map>
#include <memory>
#include <unordered_map>

namespace hod::game::PrefabUtility
{
	struct HOD_GAME_API PrefabOverride
	{
		enum class Type
		{
			Entity,
			Component,
		};

		ReflectionProperty* _reflectionProperty = nullptr;
		void*               _source = nullptr;
		void*               _instance = nullptr;
		void*               _effectiveSourceAddr = nullptr;
		void*               _effectiveInstanceAddr = nullptr;
		Type                _type;
		String              _path;
		Vector<uint64_t>    _target;
	};

	HOD_GAME_API bool CollectPrefabOverride(Entity* prefabInstance, Vector<PrefabOverride>& overrides);

	HOD_GAME_API Entity* GetPrefabInstance(Entity* entity);
	HOD_GAME_API String  GetRelativePath(Entity* parent, Entity* child);
	HOD_GAME_API Entity* FindChildByPath(Entity* parent, std::string_view relativePath);

	HOD_GAME_API Component* GetCorrespondingComponent(Component* component);
}
