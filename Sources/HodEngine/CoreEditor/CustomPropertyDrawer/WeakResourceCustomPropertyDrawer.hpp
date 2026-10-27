#pragma once
#include "HodEngine/CoreEditor/Export.hpp"

#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawer.hpp"

#include <HodEngine/Core/UID.hpp>
#include <HodEngine/Core/Vector.hpp>

namespace hod::inline core
{
	class ReflectionPropertyObject;
}
namespace hod::inline gamesystems
{
	class WeakResourceBase;
}

namespace hod::inline editor
{
	/// @brief
	class HOD_CORE_EDITOR_API WeakResourceCustomPropertyDrawer : public CustomPropertyDrawer
	{
	public:
		WeakResourceCustomPropertyDrawer() = default;
		WeakResourceCustomPropertyDrawer(const WeakResourceCustomPropertyDrawer&) = delete;
		WeakResourceCustomPropertyDrawer(WeakResourceCustomPropertyDrawer&&) = delete;
		~WeakResourceCustomPropertyDrawer() override = default;

		WeakResourceCustomPropertyDrawer& operator=(const WeakResourceCustomPropertyDrawer&) = delete;
		WeakResourceCustomPropertyDrawer& operator=(WeakResourceCustomPropertyDrawer&&) = delete;

	public:
		bool Draw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue = false) override;

		/// @brief expectedSubType, when non-zero, restricts the asset picker to resources whose subType
		/// (see ReflectionTraitAssetSubType) matches.
		static bool Draw(WeakResourceBase& weakResource, uint64_t expectedSubType = 0);

		/// @brief Left out of the pickers drawn until the pop (the font itself, the ones already in a list...). Not copied: alive until then
		static void PushExcludedResources(const Vector<UID>& excludedResources);
		static void PopExcludedResources();
		static bool IsResourceExcluded(const UID& uid);

	private:
		static Vector<const Vector<UID>*> _excludedResources;
	};
}
