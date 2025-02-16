#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Component.hpp"

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
		AddPropertyT(reflectionDescriptor, &Component::_enable, "_enable")->AddTrait<ReflectionTraitHide>();
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
	void Component::SetEnable(bool enable)
	{
		if (_enable != enable)
		{
			_enable = enable;
			if (_wasEnable != false && _enable == true)
			{
				OnEnable();
				_wasEnable = true;
			}
			else if (_wasEnable != true && _enable == false)
			{
				OnDisable();
				_wasEnable = false;
			}
		}
	}

	/// @brief 
	/// @return 
	bool Component::GetEnable() const
	{
		return _enable;
	}
}
