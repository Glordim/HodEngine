#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp"

namespace hod
{
	///@brief
	ReflectionTraitCustomSerialization::ReflectionTraitCustomSerialization(const std::function<bool(const void*, Document::Node&)> serialization,
	                                                                       const std::function<bool(void*, const Document::Node&)> deserialization)
	: ReflectionTrait()
	, _serialization(serialization)
	, _deserialization(deserialization)
	{
	}

	/// @brief
	/// @param instance
	/// @param documentNode
	bool ReflectionTraitCustomSerialization::Serialize(const void* instance, Document::Node& documentNode) const
	{
		return _serialization(instance, documentNode);
	}

	/// @brief
	/// @param instance
	/// @param documentNode
	bool ReflectionTraitCustomSerialization::Deserialize(void* instance, const Document::Node& documentNode) const
	{
		return _deserialization(instance, documentNode);
	}
}
