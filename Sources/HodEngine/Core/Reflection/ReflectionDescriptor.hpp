#pragma once
#include "HodEngine/Core/Export.hpp"

#include <vector>
#include <functional>
#include <string_view>
#include <memory>

#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/Core/Reflection/TypeTrait.hpp"

namespace hod
{
	class ReflectionTrait;
	class ReflectionProperty;

	///@brief 
	class HOD_CORE_API ReflectionDescriptor
	{
	public:

		struct Data
		{
			std::string_view		_name;
			ReflectionDescriptor*	_parent = nullptr;
			std::function<void*()>	_allocateFunction = nullptr;
			std::function<std::shared_ptr<void>()>	_sharedAllocateFunction = nullptr;
			std::function<bool(const void*, const void*)>	_compareFunction = nullptr;
			MetaType				_metaType;
		};

		template<typename _Class_, typename _ParentClass_>
		static Data GenerateReflectionData(const std::string_view& name);

	public:

												ReflectionDescriptor(const Data& data);
												ReflectionDescriptor(const char* typeName, ReflectionDescriptor* parent);
												ReflectionDescriptor(const ReflectionDescriptor& copy) = default;
												ReflectionDescriptor(ReflectionDescriptor&& move) = default;
												~ReflectionDescriptor() = default;

		ReflectionDescriptor&					operator = (const ReflectionDescriptor& copy) = default;
		ReflectionDescriptor&					operator = (ReflectionDescriptor&& move) = default;

	public:

		const std::vector<ReflectionTrait*>&	GetTraits() const;
		const std::vector<ReflectionProperty*>&	GetProperties() const;

		template<typename __TRAIT_TYPE__, typename... Args>
		__TRAIT_TYPE__*							AddTrait(Args&&... args);
		void									AddTrait(ReflectionTrait* trait);

		template<typename __PROPERTY_TYPE__, typename... Args>
		__PROPERTY_TYPE__*						AddProperty(Args&&... args);
		void									AddProperty(ReflectionProperty* property);

		template<typename _Trait_>
		_Trait_*								FindTrait() const;
		ReflectionTrait*						FindTrait(MetaType metaType) const;

		template<typename _Property_>
		_Property_*								FindProperty(const std::string_view& name, bool fallbackOnParent = true) const;
		ReflectionProperty*						FindProperty(const std::string_view& name, bool fallbackOnParent = true) const;

		void									SetFallbackTraitOnParent(bool fallbackOnParent);

		void*									CreateInstance() const;
		std::shared_ptr<void>					CreateSharedInstance() const;

		template<typename _Type_>
		_Type_*									CreateInstance() const;

		template<typename _Type_>
		std::shared_ptr<_Type_>					CreateSharedInstance() const;

		void									Copy(const void* sourceInstance, void* destinationInstance);

		bool									Compare(const void* left, const void* right) const;

		MetaType								GetMetaType() const;
		const std::string&						GetTypeName() const;

		ReflectionDescriptor*					GetParent() const;

	private:

		std::string								_typeName;
		ReflectionDescriptor*					_parent = nullptr;
		std::function<void*()>					_allocateFunction = nullptr;
		std::function<std::shared_ptr<void>()>	_sharedAllocateFunction = nullptr;
		std::function<bool(const void*, const void*)> _compareFunction = nullptr;
		MetaType								_metaType;

		std::vector<ReflectionTrait*>			_traits;
		std::vector<ReflectionProperty*>		_properties;

		bool									_fallbackTraitOnParent = false;
	};
}

#include "ReflectionDescriptor.inl"
