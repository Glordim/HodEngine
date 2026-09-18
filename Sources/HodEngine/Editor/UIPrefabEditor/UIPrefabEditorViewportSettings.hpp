#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"

#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Vector2.hpp>

namespace hod::inline editor
{
	/// @brief Per-user settings of the UIPrefabEditorViewportWindow (guides and grid), persisted in
	/// the user settings directory (see FileSystem::GetUserSettingsPath), shared by every UI prefab
	/// tab and independent of the project being edited.
	struct HOD_EDITOR_API UIPrefabEditorViewportSettings
	{
		REFLECTED_CLASS_NO_VIRTUAL(UIPrefabEditorViewportSettings)

	public:

		/// @brief A named reference rect (target resolution, safe area...) drawn over the canvas.
		struct Guide
		{
			REFLECTED_CLASS_NO_VIRTUAL(Guide)

			String  _name;
			Vector2 _resolution = Vector2(1920.0f, 1080.0f);
			Color   _color = Color(1.0f, 1.0f, 1.0f, 1.0f);
			bool    _enabled = true;
		};

	public:

		/// @brief Loads the settings from the user settings directory. Leaves the defaults untouched
		/// when there is no (readable) settings file yet.
		void Load();

		/// @brief Writes the settings to the user settings directory.
		bool Save();

	public:

		Vector<Guide> _guides;
		bool          _guidesVisible = true;

		bool  _gridVisible = true;
		float _gridCellSize = 100.0f; // canvas units, at zoom 1.0
	};
}
