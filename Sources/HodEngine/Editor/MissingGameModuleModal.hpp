#pragma once
#include <HodEngine/HodEngine.hpp>

#include <HodEngine/ImGui/Window/Modal.hpp>
#include <HodEngine/Editor/Project.hpp>

namespace hod::editor
{
	class EditorReflectedObject;

	/// @brief 
	class HOD_API MissingGameModuleModal : public imgui::Modal
	{
		META_TYPE(MissingGameModuleModal, imgui::Window);
		WINDOW_DESCRIPTION()

	public:

					MissingGameModuleModal();
					~MissingGameModuleModal() override = default;

	public:

		bool		Draw() override;
		void		DrawContent() override;

	private:

		
	};
}
