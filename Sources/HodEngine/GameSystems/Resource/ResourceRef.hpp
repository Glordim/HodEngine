#pragma once
#include "HodEngine/GameSystems/Export.hpp"
#include <memory>

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Reflection/Rtti.hpp>
#include <HodEngine/Core/UID.hpp>

namespace hod::inline gamesystems
{
	class Resource;

	/// @brief A reference to a resource by its asset's UID (serialized as it), loaded on its first GetResource through the ResourceManager.
	/// Not weak: once resolved, it keeps the resource alive (a shared_ptr) as long as it references it
	class HOD_GAME_SYSTEMS_API ResourceRefBase
	{
		REFLECTED_CLASS_NO_PARENT(ResourceRefBase)

	public:
		ResourceRefBase(ReflectionDescriptor* resourceDescriptor);
		ResourceRefBase(ReflectionDescriptor* resourceDescriptor, const std::shared_ptr<Resource>& pointer);
		ResourceRefBase(const ResourceRefBase& copy);
		virtual ~ResourceRefBase();

		ResourceRefBase& operator=(const ResourceRefBase& copy);
		ResourceRefBase& operator=(const std::weak_ptr<Resource>& pointer);

		bool operator==(const ResourceRefBase& other) const;

	public:
		/// @return Its resource, loaded on the first call then kept; null without a UID
		std::shared_ptr<Resource> GetResource() const;
		const UID&                GetUid() const;
		const UID&                GetForSerialization() const;

		ReflectionDescriptor* GetResourceDescriptor() const;

		/// @brief The type of the data the referenced asset must hold (Asset::GetSubType: a data item's struct, a data table's
		/// rows), 0 for any: the asset pickers only offer those. DataItemRef<T> and DataTableRef<T> give their T
		virtual RttiType GetExpectedSubType() const;

		void SetUid(const UID& uid);

	private:
		ReflectionDescriptor* _resourceDescriptor = nullptr;

		UID                               _uid;
		mutable std::shared_ptr<Resource> _pointer;
	};

	/// @brief
	/// @tparam _Resource_
	template<typename _Resource_>
	class ResourceRef : public ResourceRefBase
	{
		REFLECTED_CLASS(ResourceRef<_Resource_>, ResourceRefBase)

	public:
		ResourceRef()
		: ResourceRefBase(&_Resource_::GetReflectionDescriptor())
		{
		}

		/// @brief
		/// @param pointer
		ResourceRef(const std::shared_ptr<_Resource_>& pointer)
		: ResourceRefBase(_Resource_::GetReflectionDescriptor(), pointer)
		{
		}

		/// @brief
		~ResourceRef() {}

		/// @return Its resource, loaded on the first call then kept; null without a UID
		std::shared_ptr<_Resource_> GetResource() const
		{
			return std::static_pointer_cast<_Resource_>(ResourceRefBase::GetResource());
		}
	};

	template<typename _Resource_>
	void ResourceRef<_Resource_>::FillReflectionDescriptorUser([[maybe_unused]] ReflectionDescriptor& reflectionDescriptor)
	{}
}
