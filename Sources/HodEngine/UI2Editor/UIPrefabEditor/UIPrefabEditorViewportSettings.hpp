#pragma once
#include "HodEngine/UI2Editor/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"

#include <HodEngine/Math/Vector2.hpp>

namespace hod::inline editor
{
	/// @brief Per-user settings of the UIPrefabEditorViewportWindow (preview devices and grid), persisted in
	/// the user settings directory (see FileSystem::GetUserSettingsPath), shared by every UI prefab
	/// tab and independent of the project being edited.
	struct HOD_UI2_EDITOR_API UIPrefabEditorViewportSettings
	{
		REFLECTED_CLASS_NO_VIRTUAL(UIPrefabEditorViewportSettings)

	public:

		/// @brief A screen the prefab can be previewed on: the canvas is laid out for its resolution (see
		/// ui2::Canvas::SetTargetResolution), as it would be in game.
		struct Device
		{
			REFLECTED_CLASS_NO_VIRTUAL(Device)

			String  _name;
			Vector2 _resolution = Vector2(1920.0f, 1080.0f);
		};

	public:

		/// @brief Loads the settings from the user settings directory. Leaves the defaults untouched
		/// when there is no (readable) settings file yet.
		void Load();

		/// @brief Writes the settings to the user settings directory.
		bool Save();

		/// @brief Replaces the device list by the built-in one (common desktop, tablet and phone screens) and
		/// goes back to previewing the design resolution.
		void ResetDevices();

		/// @brief Resolution the canvas is previewed at: the active device's (swapped when rotated), or zero
		/// (the canvas' design resolution) when there is no active device.
		Vector2 GetPreviewResolution() const;

	public:

		Vector<Device> _devices;
		int32_t        _activeDevice = -1; // Index in _devices, -1 for the canvas' design resolution
		bool           _deviceRotated = false; // Active device previewed with its width and height swapped (portrait/landscape)

		bool  _gridVisible = true;
		float _gridCellSize = 100.0f; // canvas units, at zoom 1.0
	};
}
