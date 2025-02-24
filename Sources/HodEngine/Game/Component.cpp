#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/Entity.hpp"

#include <HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitNoSerialization.hpp>

namespace hod::game
{
	uint64_t Component::_nextInstanceId = 0;

	DESCRIBE_REFLECTED_CLASS(Component, reflectionDescriptor)
	{
		ReflectionProperty* instanceIdProperty = AddPropertyT(reflectionDescriptor, &Component::_instanceId, "_instanceId");
		instanceIdProperty->AddTrait<ReflectionTraitNoSerialization>();
		instanceIdProperty->AddTrait<ReflectionTraitHide>();

		AddPropertyT(reflectionDescriptor, &Component::_localId, "_localId")->AddTrait<ReflectionTraitHide>();
		AddPropertyT(reflectionDescriptor, &Component::_enabledSelf, "_enabledSelf")->AddTrait<ReflectionTraitHide>();
	}

	/// @brief 
	Component::Component()
	{
		++_nextInstanceId;
		_instanceId = _nextInstanceId;
	}

	/// @brief 
	/// @param entity 
	void Component::SetEntity(const std::shared_ptr<Entity>& entity)
	{
		_entity = entity;
	}

	/// @brief 
	/// @return 
	std::shared_ptr<Entity> Component::GetEntity() const
	{
		return _entity.lock();
	}

	/// @brief 
	/// @param enable 
	void Component::SetEnableSelf(bool enableSelf)
	{
		if (_enabledSelf != enableSelf)
		{
			_enabledSelf = enableSelf;
			RefreshEnabled();
		}
	}

	/// @brief 
	/// @return 
	bool Component::GetEnableSelf() const
	{
		return _enabledSelf;
	}

	/// @brief 
	/// @return 
	bool Component::IsEnabled() const
	{
		return _enabled;
	}

	/// @brief 
	void Component::RefreshEnabled()
	{
		bool enabled = _enabledSelf && GetEntity()->IsActive();
		if (_enabled != enabled)
		{
			_enabled = enabled;
			if (_enabled == true)
			{
				if (_awaked == false)
				{
					_awaked = true;
					OnAwake();
				}
				if (_started == false)
				{
					_started = true;
					OnStart();
				}
				OnEnable();
			}
			else
			{
				OnDisable();
			}
		}
	}
}
