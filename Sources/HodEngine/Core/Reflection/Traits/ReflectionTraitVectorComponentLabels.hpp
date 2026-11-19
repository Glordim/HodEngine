#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"
#include "HodEngine/Core/String.hpp"

#include <cstdint>
#include <string_view>

namespace hod::inline core
{
	/// @brief The labels the editor shows on the components of a vector property (Vector2, Vector4), X Y Z W otherwise:
	/// HOD_PROPERTY(_minSize)->AddTrait<ReflectionTraitVectorComponentLabels>("Width", "Height");
	class HOD_CORE_API ReflectionTraitVectorComponentLabels : public ReflectionTrait
	{
		RTTI(ReflectionTraitVectorComponentLabels, ReflectionTrait)

	public:
		static constexpr uint32_t MaxComponentCount = 4;

	public:
		/// @param x, y, z, w The labels of the components, the default one (X Y Z W) for an empty one
		ReflectionTraitVectorComponentLabels(std::string_view x, std::string_view y, std::string_view z = {}, std::string_view w = {});

	public:
		/// @return The label of the component (0: X), nullptr for the default one
		const char* GetLabel(uint32_t component) const;

	private:
		String _labels[MaxComponentCount];
	};
}
