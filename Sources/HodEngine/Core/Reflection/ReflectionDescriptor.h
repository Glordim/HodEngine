#pragma once

#include <vector>

//#include <rapidjson/document.h>

#include "HodEngine/Core/Document/Document.h"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.h"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.h"

namespace hod
{
	namespace core
	{
		class ReflectionTrait;
		class ReflectionProperty;

		///@brief 
		class ReflectionDescriptor
		{
		public:

													ReflectionDescriptor(const char* typeName, ReflectionDescriptor* parent);
													ReflectionDescriptor(const ReflectionDescriptor& copy) = default;
													ReflectionDescriptor(ReflectionDescriptor&& move) = default;
													~ReflectionDescriptor() = default;

			ReflectionDescriptor&					operator = (const ReflectionDescriptor& copy) = default;
			ReflectionDescriptor&					operator = (ReflectionDescriptor&& move) = default;

		public:

			//rapidjson::Value						Serialize(rapidjson::Document::AllocatorType& allocator) const;
			//void									Deserialize();

			template<typename T>
			bool									SerializeInDocument(const T& instance, Document::Node& documentNode);
			template<typename T>
			bool									DeserializeFromDocument(T& instance, const Document::Node& documentNode);

			const std::vector<ReflectionTrait*>&	GetTraits() const;
			const std::vector<ReflectionProperty*>&	GetProperties() const;

		protected:

			template<typename __TRAIT_TYPE__, typename... Args>
			void									AddTrait(Args&&... args);
			void									AddTrait(ReflectionTrait* trait);

			template<typename __PROPERTY_TYPE__, typename... Args>
			void									AddProperty(Args&&... args);
			void									AddProperty(ReflectionProperty* property);

		private:

			const char*								_typeName;
			ReflectionDescriptor*					_parent;

			std::vector<ReflectionTrait*>			_traits;
			std::vector<ReflectionProperty*>		_properties;
		};
	}
}

#include "ReflectionDescriptor.inl"