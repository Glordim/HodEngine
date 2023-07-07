#pragma once

#include <vector>
#include <functional>
#include <string_view>
#include <memory>

#include "HodEngine/Core/Document/Document.h"
#include "HodEngine/Core/Reflection/ReflectionProperty.h"

namespace hod::core
{
	class ReflectionTrait;
	class ReflectionProperty;

	///@brief 
	class ReflectionDescriptor
	{
	public:

		struct Data
		{
			std::string_view		_name;
			ReflectionDescriptor*	_parent = nullptr;
			std::function<void*()>	_allocateFunction = nullptr;
			std::function<std::shared_ptr<void>()>	_sharedAllocateFunction = nullptr;
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

		bool									SerializeInDocument(const void* instance, Document::Node& documentNode);
		bool									DeserializeFromDocument(void* instance, const Document::Node& documentNode);

		template<typename T>
		bool									SerializeInDocument(const T& instance, Document::Node& documentNode);
		template<typename T>
		bool									DeserializeFromDocument(T& instance, const Document::Node& documentNode);

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

		void*									CreateInstance() const;
		std::shared_ptr<void>					CreateSharedInstance() const;
		MetaType								GetMetaType() const;
		const std::string&						GetTypeName() const;

	private:

		std::string								_typeName;
		ReflectionDescriptor*					_parent;
		std::function<void*()>					_allocateFunction = nullptr;
		std::function<std::shared_ptr<void>()>	_sharedAllocateFunction = nullptr;
		MetaType								_metaType;

		std::vector<ReflectionTrait*>			_traits;
		std::vector<ReflectionProperty*>		_properties;
	};
}

#include "ReflectionDescriptor.inl"
