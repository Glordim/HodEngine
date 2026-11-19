#pragma once
#include "HodEngine/CoreEditor/Export.hpp"
#include <HodEngine/Editor/PropertyDrawer/PropertyDrawer.hpp>
#include <HodEngine/Core/UID.hpp>
#include <HodEngine/Core/Vector.hpp>

namespace hod::inline gamesystems
{
	class WeakResourceBase;
}

namespace hod::inline editor
{
	/// @brief
	class HOD_CORE_EDITOR_API WeakResourceDrawer : public PropertyDrawer<WeakResourceBase>
	{
	public:
		/// @brief The asset picker of weakResource: only the assets holding the data it expects (WeakResourceBase::GetExpectedSubType)
		/// @param mixed the objects edited together reference different resources: shows the mixed value label in place of weakResource's
		static bool Draw(WeakResourceBase& weakResource, bool mixed = false);

		/// @brief Left out of the pickers drawn until the pop (the font itself, the ones already in a list...). Not copied: alive until then
		static void PushExcludedResources(const Vector<UID>& excludedResources);
		static void PopExcludedResources();
		static bool IsResourceExcluded(const UID& uid);

	protected:
		bool OnDraw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue) override;

	private:
		static Vector<const Vector<UID>*> _excludedResources;
	};
}
