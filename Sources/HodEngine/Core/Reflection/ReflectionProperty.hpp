#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Core/Type.hpp"

namespace hod
{
	class ReflectionTrait;

	///@brief
	class HOD_CORE_API ReflectionProperty
	{
		BASE_META_TYPE(ReflectionProperty)

	public:
		ReflectionProperty(uint32_t offset, const char* name);
		ReflectionProperty(const ReflectionProperty& copy) = default;
		ReflectionProperty(ReflectionProperty&& move) = default;
		virtual ~ReflectionProperty();

		ReflectionProperty& operator=(const ReflectionProperty& copy) = default;
		ReflectionProperty& operator=(ReflectionProperty&& move) = default;

	public:
		const Vector<ReflectionTrait*>& GetTraits() const;
		template<typename _Trait_>
		_Trait_*         FindTrait() const;
		ReflectionTrait* FindTrait(MetaType metaType) const;

		template<typename __TRAIT_TYPE__, typename... Args>
		__TRAIT_TYPE__* AddTrait(Args&&... args);
		void            AddTrait(ReflectionTrait* trait);

		uint32_t    GetOffset() const;
		const char* GetName() const;

		virtual void Copy(const void* sourceInstance, void* destinationInstance) const = 0;

		const String& GetDisplayName() const;

	protected:
		Vector<ReflectionTrait*> _traits;

		uint32_t       _offset = 0;
		const char*    _name = nullptr;
		mutable String _displayName;
	};
}

#include "ReflectionProperty.inl"
