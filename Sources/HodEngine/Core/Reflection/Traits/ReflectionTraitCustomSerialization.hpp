#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

#include <functional>

namespace hod
{
	///@brief
	class HOD_CORE_API ReflectionTraitCustomSerialization : public ReflectionTrait
	{
		META_TYPE(ReflectionTraitCustomSerialization, ReflectionTrait)

	public:
		ReflectionTraitCustomSerialization(const std::function<bool(const void*, Document::Node&)> serialization,
		                                   const std::function<bool(void*, const Document::Node&)> deserialization);
		ReflectionTraitCustomSerialization(const ReflectionTraitCustomSerialization& copy) = default;
		ReflectionTraitCustomSerialization(ReflectionTraitCustomSerialization&& move) = default;
		~ReflectionTraitCustomSerialization() = default;

		ReflectionTraitCustomSerialization& operator=(const ReflectionTraitCustomSerialization& copy) = default;
		ReflectionTraitCustomSerialization& operator=(ReflectionTraitCustomSerialization&& move) = default;

	public:
		bool Serialize(const void* instance, Document::Node& documentNode) const;
		bool Deserialize(void* instance, const Document::Node& documentNode) const;

	private:
		std::function<bool(const void*, Document::Node&)> _serialization;
		std::function<bool(void*, const Document::Node&)> _deserialization;
	};
}
