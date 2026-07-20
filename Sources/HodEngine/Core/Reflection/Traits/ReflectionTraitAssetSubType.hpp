#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"
#include "HodEngine/Core/Reflection/Rtti.hpp"

namespace hod::inline core
{
	/// @brief Restricts a WeakResource<T> property (typically DataTableResource/DataItemResource) to
	/// assets whose row/item RttiType (Asset::GetSubType(), set by the editor tab on save) matches, since
	/// a single concrete resource class serves every row/item struct and the constraint only makes sense
	/// per usage.
	class HOD_CORE_API ReflectionTraitAssetSubType : public ReflectionTrait
	{
		RTTI(ReflectionTraitAssetSubType, ReflectionTrait)

	public:
		ReflectionTraitAssetSubType() = default;
		ReflectionTraitAssetSubType(RttiType expectedSubType);
		ReflectionTraitAssetSubType(const ReflectionTraitAssetSubType& copy) = default;
		ReflectionTraitAssetSubType(ReflectionTraitAssetSubType&& move) = default;
		~ReflectionTraitAssetSubType() = default;

		ReflectionTraitAssetSubType& operator=(const ReflectionTraitAssetSubType& copy) = default;
		ReflectionTraitAssetSubType& operator=(ReflectionTraitAssetSubType&& move) = default;

	public:
		RttiType GetExpectedSubType() const;

	private:
		RttiType _expectedSubType = 0;
	};
}
