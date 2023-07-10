#pragma once

#include <vector>

#include "HodEngine/Core/Reflection/ReflectionProperty.h"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.h"
#include "HodEngine/Core/Document/Document.h"

namespace hod
{
	class Object;

	namespace Reflection
	{
		namespace Property
		{
			///@brief 
			class Object : public ReflectionProperty // todo rename remove namespace
			{
				META_TYPE(Object, ReflectionProperty)

			public:

														Object(uint32_t offset, const char* name, ReflectionDescriptor* reflectionDesceriptor);
														Object(const Object&) = default;
														Object(Object&&) = default;
														~Object() = default;

				Object&									operator = (const Object&) = default;
				Object&									operator = (Object&&) = default;

			public:

				const char*								GetName() const;
				void*									GetInstance(void* instance) const;
				ReflectionDescriptor*					GetReflectionDescriptor() const;

				void									Serialize(const void* instance, Document::Node& node) override;
				void									Deserialize(void* instance, const Document::Node& node) override;

			private:

				uint32_t								_offset;
				const char*								_name;
				ReflectionDescriptor*					_reflectionDesceriptor;
			};
		}
	}
}
