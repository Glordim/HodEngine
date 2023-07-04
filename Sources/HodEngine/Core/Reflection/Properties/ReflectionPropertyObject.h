#pragma once

#include <vector>

#include "HodEngine/Core/Reflection/ReflectionProperty.h"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.h"
#include "HodEngine/Core/Document/Document.h"

namespace hod
{
	namespace core
	{
		namespace Reflection
		{
			namespace Property
			{
				///@brief 
				class Object : public ReflectionProperty
				{
					META_TYPE(Object)

				public:

															Object(uint32_t offset, const char* name);
															Object(const Object&) = default;
															Object(Object&&) = default;
															~Object() = default;

					Object&									operator = (const Object&) = default;
					Object&									operator = (Object&&) = default;

				public:

					const char*								GetName() const;

					void									Serialize(const void* instance, Document::Node& node) override;
					void									Deserialize(void* instance, const Document::Node& node) override;

				private:

					uint32_t								_offset;
					const char*								_name;
				};
			}
		}
	}
}

#include "ReflectionPropertyObject.inl"
