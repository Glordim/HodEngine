#pragma once

#include <HodEngine/Core/Reflection/ReflectionMacros.h>
#include <memory>
#include <functional>

#include "HodEngine/Core/Type.h"
#include "HodEngine/Core/Object.h"

namespace hod
{
	namespace game
	{
		class Entity;
		class Component;

		/// @brief 
		class ComponentDescriptor
		{
		public:

			using CreateFunction = std::function<std::shared_ptr<Component>(std::weak_ptr<Entity> entity)>;

		public:

									ComponentDescriptor(MetaType metaType, const std::string_view& displayName, const CreateFunction& createFunction);
									ComponentDescriptor(const ComponentDescriptor&) = delete;
									ComponentDescriptor(ComponentDescriptor&&) = delete;
									~ComponentDescriptor() = default;

			ComponentDescriptor&	operator = (const ComponentDescriptor&) = delete;
			ComponentDescriptor&	operator = (ComponentDescriptor&&) = delete;

		public:

			std::shared_ptr<Component>	CreateInstance(std::weak_ptr<Entity> entity) const { return _createFunction(entity); } 
			MetaType					GetMetaType() const { return _metaType; }
			const std::string&			GetDisplayName() const { return _displayName; }

		private:

			CreateFunction				_createFunction;
			MetaType					_metaType;
			std::string					_displayName;
		};
	}
}

#define HOD_COMPONENT(_Component_)			\
REFLECTED_CLASS(_Component_)			\
META_TYPE(_Component_)											\
private:														\
	static ComponentDescriptor _descriptor;						\
public:															\
	const ComponentDescriptor& GetDescriptor() override			\
	{															\
		return _Component_::GetDescriptorStatic();				\
	}															\
	static const ComponentDescriptor& GetDescriptorStatic()		\
	{															\
		return _Component_::_descriptor;						\
	}															\
private:

#define DECLARE_HOD_COMPONENT(_Component_, _ParentComponent_)																											\
ComponentDescriptor _Component_::_descriptor(_Component_::GetMetaTypeStatic(), _Component_::GetMetaTypeNameStatic(), [](std::weak_ptr<Entity> entity){ return nullptr;/*std::make_shared<_Component_>(entity);*/ }); // todo return null is type abstract		\

