#pragma once
#include "HodEngine/CoreEditor/Export.hpp"
#include <HodEngine/Editor/PropertyDrawer/PropertyDrawer.hpp>
#include <HodEngine/Core/UID.hpp>
#include <HodEngine/Core/Vector.hpp>

namespace hod::inline gamesystems
{
	class ResourceRefBase;
}

namespace hod::inline editor
{
	/// @brief A reference to a resource (ResourceRef<T>): the asset it references, picked among the ones of its type
	class HOD_CORE_EDITOR_API ResourceRefDrawer : public PropertyDrawer<ResourceRefBase>
	{
	public:
		/// @brief Left out of the pickers drawn until the pop (the font itself, the ones already in a list...). Not copied: alive until then
		static void PushExcludedResources(const Vector<UID>& excludedResources);
		static void PopExcludedResources();
		static bool IsResourceExcluded(const UID& uid);

	protected:
		bool OnDraw(const PropertyValues<ResourceRefBase>& values) override;

	private:
		/// @brief The asset picker of resourceRef: only the assets holding the data it expects (ResourceRefBase::GetExpectedSubType)
		/// @param mixed the values edited together reference different resources: shows the mixed value label in place of resourceRef's
		/// @param picked The resource picked (invalid: None)
		/// @return true if one was picked
		static bool DrawPicker(const ResourceRefBase& resourceRef, bool mixed, UID& picked);

		static Vector<const Vector<UID>*> _excludedResources;
	};
}
