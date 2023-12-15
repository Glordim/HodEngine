#pragma once

#include <vector>
#include "HodEngine/Core/Document/Document.h"

#include "HodEngine/Core/Type.h"

namespace hod
{
	class ReflectionTrait;

	///@brief 
	class ReflectionProperty
	{
		BASE_META_TYPE(ReflectionProperty)

	public:

												ReflectionProperty(uint32_t offset, const char* name);
												ReflectionProperty(const ReflectionProperty& copy) = default;
												ReflectionProperty(ReflectionProperty&& move) = default;
												~ReflectionProperty() = default;

		ReflectionProperty&						operator = (const ReflectionProperty& copy) = default;
		ReflectionProperty&						operator = (ReflectionProperty&& move) = default;

	public:

		const std::vector<ReflectionTrait*>&	GetTraits() const;
		template<typename _Trait_>
		_Trait_*								FindTrait() const;
		ReflectionTrait*						FindTrait(MetaType metaType) const;

		template<typename __TRAIT_TYPE__, typename... Args>
		__TRAIT_TYPE__*							AddTrait(Args&&... args);
		void									AddTrait(ReflectionTrait* trait);

		uint32_t								GetOffset() const;
		const char*								GetName() const;

	protected:

		std::vector<ReflectionTrait*>			_traits;

		uint32_t								_offset = 0;
		const char*								_name = nullptr;
	};
}

#include "ReflectionProperty.inl"
