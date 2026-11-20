#include "HodEngine/GameSystems/Pch.hpp"
#include "HodEngine/GameSystems/Resource/ResourceRef.hpp"

#include "HodEngine/GameSystems/Resource/Resource.hpp"
#include "HodEngine/GameSystems/Resource/ResourceManager.hpp"

namespace hod::inline gamesystems
{
	DESCRIBE_REFLECTED_CLASS(ResourceRefBase)
	{
		HOD_PROPERTY(_uid);
	}

	/// @brief
	/// @param pointer
	ResourceRefBase::ResourceRefBase(ReflectionDescriptor* resourceDescriptor)
	: _resourceDescriptor(resourceDescriptor)
	, _uid(UID::INVALID_UID)
	{
	}

	/// @brief
	/// @param pointer
	ResourceRefBase::ResourceRefBase(ReflectionDescriptor* resourceDescriptor, const std::shared_ptr<Resource>& pointer)
	: _resourceDescriptor(resourceDescriptor)
	, _uid(UID::INVALID_UID)
	, _pointer(pointer)
	{
	}

	ResourceRefBase::ResourceRefBase(const ResourceRefBase& copy)
	{
		_uid = copy._uid;
		_pointer = copy._pointer;
		_resourceDescriptor = copy._resourceDescriptor;
	}

	/// @brief
	ResourceRefBase::~ResourceRefBase() {}

	/// @brief
	/// @param pointer
	ResourceRefBase& ResourceRefBase::operator=(const ResourceRefBase& copy)
	{
		_uid = copy._uid;
		_pointer = copy._pointer;
		_resourceDescriptor = copy._resourceDescriptor;
		return *this;
	}

	/// @brief
	/// @param pointer
	ResourceRefBase& ResourceRefBase::operator=(const std::weak_ptr<Resource>& pointer)
	{
		_pointer = pointer.lock();
		return *this;
	}

	/// @brief
	/// @param other
	/// @return
	bool ResourceRefBase::operator==(const ResourceRefBase& other) const
	{
		return _uid == other._uid;
	}

	/// @brief
	/// @return
	std::shared_ptr<Resource> ResourceRefBase::GetResource() const
	{
		if (_uid == UID::INVALID_UID)
		{
			_pointer = nullptr;
			return _pointer;
		}
		else if (_pointer == nullptr || _pointer->GetUid() != _uid)
		{
			_pointer = ResourceManager::GetInstance()->GetResource(*_resourceDescriptor, _uid);
		}
		return _pointer;
	}

	/// @brief
	/// @return
	const UID& ResourceRefBase::GetUid() const
	{
		return _uid;
	}

	void ResourceRefBase::SetUid(const UID& uid)
	{
		_uid = uid;
	}

	ReflectionDescriptor* ResourceRefBase::GetResourceDescriptor() const
	{
		return _resourceDescriptor;
	}

	/// @brief
	RttiType ResourceRefBase::GetExpectedSubType() const
	{
		return 0;
	}
}
