#pragma once

#include <HodEngine/Core/Reflection/ReflectionMacros.h>
#include <memory>
#include <functional>

#include "HodEngine/Core/Type.h"
#include "HodEngine/Core/Object.h"

#define HOD_COMPONENT(_Component_, _ParentComponent_)			\
REFLECTED_DERIVED_CLASS(_Component_, _ParentComponent_)			\
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
ComponentDescriptor _Component_::_descriptor(_Component_::GetMetaTypeStatic(), _Component_::GetMetaTypeNameStatic(), [](std::weak_ptr<Entity> entity){ return std::make_shared<_Component_>(entity); }); // todo return null is type abstract		\


namespace hod
{
	namespace renderer
	{
		class RenderQueue;
	}

	namespace game
	{
		class Actor;
		class Entity;
		class Component;

		/// @brief 
		/// @tparam _Component_ 
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

		public:

			static const std::map<MetaType, ComponentDescriptor*>&	GetAllDescriptors() { return _descriptors; }

		private:

			CreateFunction				_createFunction;
			MetaType					_metaType;
			std::string					_displayName;

		private:

			static std::map<MetaType, ComponentDescriptor*>	_descriptors;
		};

		///@brief 
		class Component : public Object, public std::enable_shared_from_this<Component>
		{
			REFLECTED_ABSTRACT_DERIVED_CLASS(Component, Object)
			BASE_META_TYPE(Component)

			friend class Actor;

		public:

			virtual const ComponentDescriptor& GetDescriptor() = 0;	// todo base macro ?

		public:

			const std::weak_ptr<Entity>&	GetEntity() const;
			Actor*				GetActor() const;
			virtual const char*	GetType() const = 0;

			virtual void		PushToRenderQueue(renderer::RenderQueue& renderQueue);

		protected:

								Component(Actor* actor); // todo remove
								Component(const std::weak_ptr<Entity>& entity);
								Component(const Component&) = delete;
								Component(Component&&) = delete;
			virtual				~Component() = default;

			void				operator=(const Component&) = delete;
			void				operator=(Component&&) = delete;

		private:

			Actor*				_actor = nullptr;
			std::weak_ptr<Entity>	_entity;
		};
	}
}

#include "Component.inl"
