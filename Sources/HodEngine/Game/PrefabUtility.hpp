#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Document/Document.hpp>

#include "HodEngine/Game/Entity.hpp"

#include <map>
#include <unordered_map>
#include <memory>

namespace hod::game::PrefabUtility
{
	struct HOD_GAME_API EntityDiffs
	{
		struct HOD_GAME_API Diff
		{
			enum class Type
			{
				Entity,
				Component,	
			};

			Diff(const std::string& path, ReflectionProperty* reflectionProperty, std::shared_ptr<Entity> source, std::shared_ptr<Entity> instance, void* effectiveSourceAddr, void* effectiveInstanceAddr)
			: _reflectionProperty(reflectionProperty)
			, _source(source.get())
			, _instance(instance.get())
			, _effectiveSourceAddr(effectiveSourceAddr)
			, _effectiveInstanceAddr(effectiveInstanceAddr)
			, _type(Type::Entity)
			, _path(path)
			{

			}

			Diff(const std::string& path, ReflectionProperty* reflectionProperty, std::shared_ptr<Component> source, std::shared_ptr<Component> instance, void* effectiveSourceAddr, void* effectiveInstanceAddr)
			: _reflectionProperty(reflectionProperty)
			, _source(source.get())
			, _instance(instance.get())
			, _effectiveSourceAddr(effectiveSourceAddr)
			, _effectiveInstanceAddr(effectiveInstanceAddr)
			, _type(Type::Component)
			, _path(path)
			{

			}

			ReflectionProperty* _reflectionProperty = nullptr;
			void* 				_source = nullptr;
			void* 				_instance = nullptr;
			void*				_effectiveSourceAddr = nullptr;
			void*				_effectiveInstanceAddr = nullptr;
			Type				_type;
			std::string			_path;
		};

		std::vector<Diff*> _diffs;

		void Add(const std::string& path, ReflectionProperty* reflectionProperty, std::shared_ptr<Entity> source, std::shared_ptr<Entity> instance, void* effectiveSourceAddr, void* effectiveInstanceAddr)
		{
			_diffs.push_back(new Diff(path, reflectionProperty, source, instance, effectiveSourceAddr, effectiveInstanceAddr));
		}

		void Add(const std::string& path, ReflectionProperty* reflectionProperty, std::shared_ptr<Component> source, std::shared_ptr<Component> instance, void* effectiveSourceAddr, void* effectiveInstanceAddr)
		{
			_diffs.push_back(new Diff(path, reflectionProperty, source, instance, effectiveSourceAddr, effectiveInstanceAddr));
		}
	};

	HOD_GAME_API bool CollectDiff(std::shared_ptr<Entity> entity, EntityDiffs& diffs);
	HOD_GAME_API void CollectDiff(std::shared_ptr<Component> sourceComponent, std::shared_ptr<Component> instanceComponent, EntityDiffs& diffs, const std::string& path, ReflectionDescriptor* reflectionDescriptor, void* sourceAddr, void* instanceAddr);

	HOD_GAME_API std::shared_ptr<Entity> GetPrefabInstance(std::shared_ptr<Entity> entity);
	HOD_GAME_API std::string GetRelativePath(std::shared_ptr<Entity> parent, std::shared_ptr<Entity> child);
	HOD_GAME_API std::shared_ptr<Entity> FindChildByPath(std::shared_ptr<Entity> parent, std::string_view relativePath);

	HOD_GAME_API std::shared_ptr<Component> GetCorrespondingComponent(std::shared_ptr<Component> component);
}
