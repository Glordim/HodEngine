#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/CustomComponentDrawer/CustomComponentDrawer.hpp"

namespace hod::renderer
{
	class MaterialInstance;
}

namespace hod::editor
{
	/// @brief 
	class HOD_GAME_EDITOR_API CameraComponentCustomEditor : public CustomComponentDrawer
	{
	public:

											CameraComponentCustomEditor();
											CameraComponentCustomEditor(const CameraComponentCustomEditor&) = delete;
											CameraComponentCustomEditor(CameraComponentCustomEditor&&) = delete;
											~CameraComponentCustomEditor() override;

		CameraComponentCustomEditor			operator = (const CameraComponentCustomEditor&) = delete;
		CameraComponentCustomEditor			operator = (CameraComponentCustomEditor&&) = delete;

	public:

		bool								OnDrawGizmo(game::Component* component, ViewportWindow& viewport, bool selected) override;

	private:

		renderer::MaterialInstance*			_materialInstance = nullptr;
	};
}
