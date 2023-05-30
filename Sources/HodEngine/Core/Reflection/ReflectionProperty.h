#pragma once

#include <vector>
#include "HodEngine/Core/Document/Document.h"
//#include <rapidjson/document.h>

namespace hod
{
	namespace core
	{
		class ReflectionTrait;

		///@brief 
		class ReflectionProperty
		{
		public:

													ReflectionProperty(const char* typeName);
													ReflectionProperty(const ReflectionProperty& copy) = default;
													ReflectionProperty(ReflectionProperty&& move) = default;
													~ReflectionProperty() = default;

			ReflectionProperty&						operator = (const ReflectionProperty& copy) = default;
			ReflectionProperty&						operator = (ReflectionProperty&& move) = default;

		public:

			virtual void							Serialize(const void* instance, Document::Node& node) = 0;
			virtual void							Deserialize(void* instance, const Document::Node& node) = 0;

			const std::vector<ReflectionTrait*>&	GetTraits() const;

			template<typename __TRAIT_TYPE__, typename... Args>
			void									AddTrait(Args&&... args);
			void									AddTrait(ReflectionTrait* trait);

			const char*								GetTypeName() const { return _typeName; }

		protected:

			const char*								_typeName;
			std::vector<ReflectionTrait*>			_traits;
		};
	}
}

#include "ReflectionProperty.inl"
