#pragma once

#include <vector>

#include <rapidjson/document.h>

namespace HOD
{
	namespace CORE
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

			virtual rapidjson::Value				Serialize(rapidjson::Document::AllocatorType& allocator) const = 0;
			//virtual void							Deserialize() = 0;

			const std::vector<ReflectionTrait*>&	GetTraits() const;

			template<typename __TRAIT_TYPE__, typename... Args>
			void									AddTrait(Args&&... args);
			void									AddTrait(ReflectionTrait* trait);

		protected:

			const char*								_typeName;
			std::vector<ReflectionTrait*>			_traits;
		};
	}
}

#include "ReflectionProperty.inl"
