#include "Editor.h"

#include "HodEngine/Core/Src/Job/Job.h"
#include <HodEngine/ImGui/src/imgui.h>
#include <HodEngine/ImGui/src/imgui_impl_win32.h>
//#include <HodEngine/ImGui/src/imgui_impl_vulkan.h>

#if defined(PLATFORM_WINDOWS)
	#include <HodEngine/Window/src/DesktopWindow/Win32/Win32Window.h>
#endif

#include <HodEngine/Renderer/src/Renderer.h>
#include <HodEngine/Renderer/src/RenderCommand/RenderCommandImGui.h>

#include <HodEngine/Core/Src/Frame/FrameSequencer.h>

namespace HOD
{
	/// @brief 
	Editor::Editor()
		: _updateJob(this, &Editor::Update, JobQueue::Queue::FramedNormalPriority)
	{
	}

	/// @brief 
	Editor::~Editor()
	{
#if defined(PLATFORM_WINDOWS)
		ImGui_ImplWin32_Shutdown();
#endif
		//ImGui_ImplVulkan_Shutdown();

		ImGui::DestroyContext();
	}

	/// @brief 
	/// @param window 
	/// @return 
	bool Editor::Init(Window* window)
	{
		ImGui::CreateContext();
		ImGui::GetIO().Fonts->AddFontDefault();

		unsigned char* out_pixels;
		int out_width;
		int out_height;
		int out_bytes_per_pixel;
		ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&out_pixels, &out_width, &out_height, &out_bytes_per_pixel);

		RENDERER::Texture* fontTexture = RENDERER::Renderer::GetInstance()->CreateTexture("");

		ImGui::GetIO().Fonts->SetTexID((ImTextureID)fontTexture);

#if defined(PLATFORM_WINDOWS)
		ImGui_ImplWin32_Init(static_cast<Win32Window*>(window)->GetWindowHandle());
#endif

		FrameSequencer::GetInstance()->InsertJob(&_updateJob, FrameSequencer::Step::PreRender);

		return true;
	}

	/// @brief 
	void Editor::Update()
	{
		ImGui_ImplWin32_NewFrame();
		//ImGui_ImplVulkan_NewFrame();
		ImGui::NewFrame();

		ImGui::BeginMainMenuBar();

		ImGui::EndMainMenuBar();

		ImGui::EndFrame();
		ImGui::Render();

		ImDrawData* drawData = ImGui::GetDrawData();

		std::vector<RENDERER::RenderCommandImGui::DrawList*> drawLists;
		drawLists.resize(drawData->CmdListsCount);

		for (int drawListIndex = 0; drawListIndex < drawData->CmdListsCount; ++drawListIndex)
		{
			ImDrawList* imDrawList = drawData->CmdLists[drawListIndex];

			RENDERER::RenderCommandImGui::DrawList* drawList = new RENDERER::RenderCommandImGui::DrawList();

			drawList->_vertices.resize(imDrawList->VtxBuffer.Size);
			memcpy(drawList->_vertices.data(), imDrawList->VtxBuffer.Data, imDrawList->VtxBuffer.Size * sizeof(RENDERER::RenderCommandImGui::Vertex));

			drawList->_indices.resize(imDrawList->IdxBuffer.Size);
			memcpy(drawList->_indices.data(), imDrawList->IdxBuffer.Data, imDrawList->IdxBuffer.Size * sizeof(uint16_t));

			drawList->_commands.resize(imDrawList->CmdBuffer.Size);
			for (int cmdIndex = 0; cmdIndex < imDrawList->CmdBuffer.Size; ++cmdIndex)
			{
				ImDrawCmd& imCommand = imDrawList->CmdBuffer[cmdIndex];
				RENDERER::RenderCommandImGui::Command& command = drawList->_commands[cmdIndex];

				memcpy(&command._clipRect, &imCommand.ClipRect, sizeof(CORE::Rect));
				command._texture = reinterpret_cast<RENDERER::Texture*>(imCommand.TextureId);
				command._vertexOffset = imCommand.VtxOffset;
				command._indexOffset = imCommand.IdxOffset;
				command._elementCount = imCommand.ElemCount;
			}

			drawLists[drawListIndex] = drawList;
		}

		RENDERER::RenderCommandImGui* renderCommand = new RENDERER::RenderCommandImGui(drawLists);

		RENDERER::Renderer* renderer = RENDERER::Renderer::GetInstance();
		renderer->GetRenderQueue()->PushRenderCommand(renderCommand);
	}
}
