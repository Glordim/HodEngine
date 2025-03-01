#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/Entity.hpp"

#include <HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitNoSerialization.hpp>

namespace hod::game
{
	std::atomic<uint64_t> Component::_nextInstanceId = 0;

	DESCRIBE_REFLECTED_CLASS(Component, reflectionDescriptor)
	{
		ReflectionProperty* instanceIdProperty = AddPropertyT(reflectionDescriptor, &Component::_instanceId, "_instanceId");
		instanceIdProperty->AddTrait<ReflectionTraitNoSerialization>();
		instanceIdProperty->AddTrait<ReflectionTraitHide>();

		AddPropertyT(reflectionDescriptor, &Component::_localId, "_localId")->AddTrait<ReflectionTraitHide>();
		AddPropertyT(reflectionDescriptor, &Component::_enabled, "_enabled")->AddTrait<ReflectionTraitHide>();
	}

	/// @brief 
	Component::Component()
	{
		_instanceId = _nextInstanceId.fetch_add(1);
	}

	/// @brief 
	/// @return 
	uint64_t Component::GetInstanceId() const
	{
		return _instanceId;
	}

	/// @brief 
	/// @return 
	uint64_t Component::GetLocalId() const
	{
		return _localId;
	}

	/// @brief 
	/// @param localId 
	void Component::SetLocalId(uint64_t localId)
	{
		_localId = localId;
	}

	/// @brief 
	/// @return 
	std::shared_ptr<Entity> Component::GetOwner() const
	{
		return _entity.lock();
	}

	/// @brief 
	/// @param entity 
	void Component::AttachTo(const std::shared_ptr<Entity>& entity)
	{
		_entity = entity;
	}

	/// @brief 
	/// @param entity 
	void Component::SetEntity(const std::shared_ptr<Entity>& entity)
	{
		AttachTo(entity);
	}

	/// @brief 
	/// @return 
	std::shared_ptr<Entity> Component::GetEntity() const
	{
		return GetOwner();
	}

	/// @brief 
	/// @param enable 
	void Component::SetEnabled(bool enabled)
	{
		if (_enabled != enabled)
		{
			_enabled = enabled;
			if (_internalState == InternalState::Started && _enabled != _enabledInHierarchy && GetEntity()->IsActive())
			{
				if (_enabled)
				{
					Enable();
				}
				else
				{
					Disable();
				}
			}
		}
	}

	/// @brief 
	/// @return 
	bool Component::GetEnabled() const
	{
		return _enabled;
	}

	/// @brief 
	/// @return 
	bool Component::IsEnabledInHierarchy() const
	{
		return _enabledInHierarchy;
	}

	/// @brief 
	/// @return 
	Component::InternalState Component::GetInternalState() const
	{
		return _internalState;
	}

	/// @brief 
	void Component::Construct()
	{
		assert(_internalState == InternalState::None);
		_internalState = InternalState::Constructed;
		OnConstruct();
	}

	/// @brief 
	void Component::Awake()
	{
		assert(_internalState == InternalState::Constructed);
		_internalState = InternalState::Awaked;
		OnAwake();
	}

	/// @brief 
	void Component::Enable()
	{
		assert(_enabled == true && (_internalState == InternalState::Awaked || _internalState == InternalState::Started));
		_enabledInHierarchy = true;
		OnEnable();
	}

	/// @brief 
	void Component::Start()
	{
		assert(_internalState == InternalState::Awaked);
		_internalState = InternalState::Started;
		OnStart();
	}

	/// @brief 
	void Component::Disable()
	{
		assert(_internalState == InternalState::Started);
		_enabledInHierarchy = false;
		OnDisable();
	}

	/// @brief 
	void Component::Destruct()
	{
		assert(_internalState == InternalState::Constructed || _internalState == InternalState::Started);
		_internalState = InternalState::Destructed;
		OnDestruct();
	}

	/// @brief 
	void Component::OnConstruct()
	{

	}

	/// @brief 
	void Component::OnAwake()
	{

	}

	/// @brief 
	void Component::OnEnable()
	{

	}

	/// @brief 
	void Component::OnStart()
	{

	}

	/// @brief 
	void Component::OnUpdate()
	{

	}

	/// @brief 
	void Component::OnDisable()
	{

	}

	/// @brief 
	void Component::OnDestruct()
	{

	}
}
