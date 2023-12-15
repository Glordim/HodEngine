#pragma once

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"
#include "HodEngine/Core/Document/Document.hpp"

#include <functional>

namespace hod
{
	///@brief 
	class ReflectionTraitCustomSerialization : public ReflectionTrait
	{
		META_TYPE(ReflectionTraitCustomSerialization, ReflectionTrait)

	public:

												ReflectionTraitCustomSerialization(const std::function<void(const void*, Document::Node&)> serialization, const std::function<void(void*, const Document::Node&)> deserialization);
												ReflectionTraitCustomSerialization(const ReflectionTraitCustomSerialization& copy) = default;
												ReflectionTraitCustomSerialization(ReflectionTraitCustomSerialization&& move) = default;
												~ReflectionTraitCustomSerialization() = default;

		ReflectionTraitCustomSerialization&		operator = (const ReflectionTraitCustomSerialization& copy) = default;
		ReflectionTraitCustomSerialization&		operator = (ReflectionTraitCustomSerialization&& move) = default;

	public:

		void									Serialize(const void* instance, Document::Node& documentNode) const;
		void									Deserialize(void* instance, const Document::Node& documentNode) const;

	private:

		std::function<void(const void*, Document::Node&)>	_serialization;
		std::function<void(void*, const Document::Node&)>	_deserialization;
	};
}
