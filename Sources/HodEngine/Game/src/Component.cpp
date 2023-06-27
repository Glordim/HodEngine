#include "Component.h"

#include "Traits/Type.h"

namespace hod
{
	namespace game
	{
		std::map<MetaType, ComponentDescriptor*>	ComponentDescriptor::_descriptors;

		/// @brief 
		/// @param metaType 
		/// @param createFunction 
		ComponentDescriptor::ComponentDescriptor(MetaType metaType, const std::string_view& displayName, const CreateFunction& createFunction)
		: _createFunction(createFunction)
		, _metaType(metaType)
		, _displayName(displayName)
		{
			_descriptors[metaType] = this;
		}

		DESCRIBE_REFLECTED_DERIVED_ABSTRACT_CLASS(Component, Object)
		{
			AddTrait<Traits::Type>(Traits::Type::Component);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Component::Component(Actor* actor)
			: _actor(actor)
		{
		}

		/// @brief 
		/// @param entity 
		Component::Component(const std::weak_ptr<Entity>& entity)
		: _entity(entity)
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Component::PushToRenderQueue(renderer::RenderQueue& renderQueue)
		{

		}

		/// @brief 
		/// @return 
		const std::weak_ptr<Entity>& Component::GetEntity() const
		{
			return _entity;
		}
	}
}
