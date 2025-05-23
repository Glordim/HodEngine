#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/ComponentCustomEditor/ComponentCustomEditor.hpp"

namespace hod::renderer
{
	class MaterialInstance;
}

namespace hod::editor
{
	/// @brief 
	class HOD_EDITOR_API CameraComponentCustomEditor : public ComponentCustomEditor
	{
	public:

											CameraComponentCustomEditor();
											CameraComponentCustomEditor(const CameraComponentCustomEditor&) = delete;
											CameraComponentCustomEditor(CameraComponentCustomEditor&&) = delete;
											~CameraComponentCustomEditor() override;

		CameraComponentCustomEditor			operator = (const CameraComponentCustomEditor&) = delete;
		CameraComponentCustomEditor			operator = (CameraComponentCustomEditor&&) = delete;

	public:

		bool								OnDrawGizmo(game::Component* component, ViewportWindow& viewport) override;

	private:

		renderer::MaterialInstance*			_materialInstance = nullptr;
	};
}
