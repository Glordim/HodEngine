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
			std::function<void(void*)> _deleteFunction = nullptr;
			std::function<std::shared_ptr<void>()>	_sharedAllocateFunction = nullptr;
			std::function<bool(const void*, const void*)>	_compareFunction = nullptr;
			MetaType				_metaType;
		};

		template<typename _Class_, typename _ParentClass_ = void>
		static Data GenerateReflectionData(const std::string_view& name);

	public:

												ReflectionDescriptor() = default;
												ReflectionDescriptor(const Data& data);
												ReflectionDescriptor(const char* typeName, ReflectionDescriptor* parent);
												ReflectionDescriptor(const ReflectionDescriptor& copy) = default;
												ReflectionDescriptor(ReflectionDescriptor&& move) = default;
												~ReflectionDescriptor();

		ReflectionDescriptor&					operator = (const ReflectionDescriptor& copy) = default;
		ReflectionDescriptor&					operator = (ReflectionDescriptor&& move) = default;

	public:

		void									Init(const Data& data);

		const std::vector<ReflectionTrait*>&	GetTraits() const;
		const std::vector<ReflectionProperty*>&	GetProperties() const;

		template<typename _Trait_, typename... Args>
		_Trait_*								AddTrait(Args&&... args);
		void									AddTrait(ReflectionTrait* trait);

		template<typename _Property_, typename... Args>
		_Property_*								AddProperty(Args&&... args);
		void									AddProperty(ReflectionProperty* property);

		template<typename _Trait_>
		_Trait_*								FindTrait(bool fallbackOnParent = true) const;
		ReflectionTrait*						FindTrait(MetaType metaType, bool fallbackOnParent = true) const;

		template<typename _Property_>
		_Property_*								FindProperty(const std::string_view& name, bool fallbackOnParent = true) const;
		ReflectionProperty*						FindProperty(const std::string_view& name, bool fallbackOnParent = true) const;

		template<typename _ObjectType_>
		void									CollectObjectProperties(std::vector<_ObjectType_*>& collectedInstances, void* instance);
		void									CollectObjectProperties(const ReflectionDescriptor& reflectionDescriptor, std::vector<void*>& collectedInstances, void* instance);

		template<typename _Trait_>
		void									RemoveTrait();
		void									RemoveTrait(MetaType metaType);

		void*									CreateInstance() const;
		void									DeleteInstance(void* instance);

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
		bool									IsCompatible(const ReflectionDescriptor& descriptor) const;

		const std::string&						GetDisplayName() const;

	private:

		std::string								_typeName;
		mutable std::string						_displayName;

		ReflectionDescriptor*					_parent = nullptr;
		std::function<void*()>					_allocateFunction = nullptr;
		std::function<void(void*)>				_deleteFunction = nullptr;
		std::function<std::shared_ptr<void>()>	_sharedAllocateFunction = nullptr;
		std::function<bool(const void*, const void*)> _compareFunction = nullptr;
		MetaType								_metaType;

		std::vector<ReflectionTrait*>			_traits;
		std::vector<ReflectionProperty*>		_properties;
	};
}

#include "ReflectionDescriptor.inl"
