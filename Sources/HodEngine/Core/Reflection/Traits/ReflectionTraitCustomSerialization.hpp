#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

#include <functional>

namespace hod::inline core
{
	///@brief
	class HOD_CORE_API ReflectionTraitCustomSerialization : public ReflectionTrait
	{
		RTTI(ReflectionTraitCustomSerialization, ReflectionTrait)

	public:
		ReflectionTraitCustomSerialization(const std::function<bool(const void*, DocumentNode&)> serialization,
		                                   const std::function<bool(void*, const DocumentNode&)> deserialization);
		ReflectionTraitCustomSerialization(const ReflectionTraitCustomSerialization& copy) = default;
		ReflectionTraitCustomSerialization(ReflectionTraitCustomSerialization&& move) = default;
		~ReflectionTraitCustomSerialization() = default;

		ReflectionTraitCustomSerialization& operator=(const ReflectionTraitCustomSerialization& copy) = default;
		ReflectionTraitCustomSerialization& operator=(ReflectionTraitCustomSerialization&& move) = default;

	public:
		bool Serialize(const void* instance, DocumentNode& documentNode) const;
		bool Deserialize(void* instance, const DocumentNode& documentNode) const;

	private:
		std::function<bool(const void*, DocumentNode&)> _serialization;
		std::function<bool(void*, const DocumentNode&)> _deserialization;
	};
}
