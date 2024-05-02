#pragma once

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Document/Document.hpp>

#include "HodEngine/Game/Entity.hpp"

#include <map>
#include <unordered_map>
#include <memory>

namespace hod::game::PrefabUtility
{
	struct EntityDiffs
	{
		struct Diff
		{
			enum class Type
			{
				Entity,
				Component,	
			};

			Diff(ReflectionProperty* reflectionProperty, std::shared_ptr<Entity> source, std::shared_ptr<Entity> instance)
			: _reflectionProperty(reflectionProperty)
			, _source(source.get())
			, _instance(instance.get())
			, _type(Type::Entity)
			{

			}

			Diff(ReflectionProperty* reflectionProperty, std::shared_ptr<Component> source, std::shared_ptr<Component> instance)
			: _reflectionProperty(reflectionProperty)
			, _source(source.get())
			, _instance(instance.get())
			, _type(Type::Component)
			{

			}

			ReflectionProperty* _reflectionProperty = nullptr;
			void* 				_source = nullptr;
			void* 				_instance = nullptr;
			Type				_type;
		};

		std::vector<Diff*> _diffs;

		void Add(ReflectionProperty* reflectionProperty, std::shared_ptr<Entity> source, std::shared_ptr<Entity> instance)
		{
			_diffs.push_back(new Diff(reflectionProperty, source, instance));
		}

		void Add(ReflectionProperty* reflectionProperty, std::shared_ptr<Component> source, std::shared_ptr<Component> instance)
		{
			_diffs.push_back(new Diff(reflectionProperty, source, instance));
		}
	};

	bool CollectDiff(std::shared_ptr<Entity> entity, EntityDiffs& diffs);
}
