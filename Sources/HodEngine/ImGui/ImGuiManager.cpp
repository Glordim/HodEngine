#include "HodEngine/ImGui/ImGuiManager.h"

#include <HodEngine/Core/Job/Job.h>

#if defined(PLATFORM_WINDOWS)
	#include <HodEngine/Window/DesktopWindow/Win32/Win32Window.h>
#endif

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/DearImGui/imgui_impl_win32.h"
//#include "HodEngine/ImGui/imgui_impl_vulkan.h"

#include "HodEngine/ImGui/Window.h"
#include "HodEngine/ImGui/MainBar.h"

#include <glm/vec2.hpp>

#include <HodEngine/Renderer/Renderer.h>
#include <HodEngine/Renderer/RenderCommand/RenderCommandImGui.h>
#include <HodEngine/Renderer/RHI/Texture.h>

#include <HodEngine/Core/Frame/FrameSequencer.h>

#include <HodEngine/Application/Application.h>

#if defined(PLATFORM_WINDOWS)
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

namespace hod::imgui
{
	/// @brief 
	ImGuiManager::ImGuiManager()
		: _updateJob(this, &ImGuiManager::Update, JobQueue::Queue::FramedNormalPriority)
#if defined (PLATFORM_WINDOWS)
		, _winProcSlot(&ImGui_ImplWin32_WndProcHandler)
#endif
	{
	}

	/// @brief 
	ImGuiManager::~ImGuiManager()
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
	bool ImGuiManager::Init(window::Window* window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		//ImGui::GetIO().Fonts->AddFontDefault();

		unsigned char* out_pixels;
		int out_width;
		int out_height;
		int out_bytes_per_pixel;
		ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&out_pixels, &out_width, &out_height, &out_bytes_per_pixel);

		renderer::Texture* fontTexture = renderer::Renderer::GetInstance()->CreateTexture();
		fontTexture->BuildBuffer(out_width, out_height, out_pixels);

		ImGui::GetIO().Fonts->SetTexID((ImTextureID)fontTexture);
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

#if defined(PLATFORM_WINDOWS)
		ImGui_ImplWin32_Init(static_cast<window::Win32Window*>(window)->GetWindowHandle());
		static_cast<window::Win32Window*>(window)->OnWinProc.Connect(_winProcSlot);
#endif

		FrameSequencer::GetInstance()->InsertJob(&_updateJob, FrameSequencer::Step::PreRender);

		return true;
	}

#if defined(PLATFORM_WINDOWS)
	void ImGuiManager::OnWinProcEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
	}
#endif

	/// @brief 
	/// @param mainBar 
	void ImGuiManager::SetMainBar(MainBar* mainBar)
	{
		_mainBar = mainBar;
	}

	/// @brief 
	void ImGuiManager::CloseAllWindow()
	{
		for (Window* window : _windows)
		{
			window->Close();
		}
	}

	/// @brief 
	void ImGuiManager::Update()
	{
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (_mainBar != nullptr)
		{
			if (ImGui::BeginMainMenuBar() == true)
			{
				_mainBar->Draw();
				ImGui::EndMainMenuBar();
			}
		}

		ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

		static bool showDemo = true;
		if (showDemo == true)
		{
			ImGui::ShowDemoWindow(&showDemo);
		}

		std::vector<Window*>::iterator windowIt = _windows.begin();
		std::vector<Window*>::iterator windowItEnd = _windows.end();
		while (windowIt != windowItEnd)
		{
			if ((*windowIt)->IsClosed() == true)
			{
				delete *windowIt;
				windowIt = _windows.erase(windowIt);
				windowItEnd = _windows.end();
			}
			else
			{
				++windowIt;
			}
		}

		for (Window* window : _windows)
		{
			/*
			ImGuiWindowClass window_class;
			window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
			ImGui::SetNextWindowClass(&window_class);
			*/
			window->Draw();
		}

		ImGui::Render();

		ImDrawData* drawData = ImGui::GetDrawData();

		std::vector<renderer::RenderCommandImGui::DrawList*> drawLists;
		drawLists.resize(drawData->CmdListsCount);

		for (int drawListIndex = 0; drawListIndex < drawData->CmdListsCount; ++drawListIndex)
		{
			ImDrawList* imDrawList = drawData->CmdLists[drawListIndex];

			renderer::RenderCommandImGui::DrawList* drawList = new renderer::RenderCommandImGui::DrawList();

			memcpy(&drawList->_displayPosition, &drawData->DisplayPos, sizeof(glm::vec2));
			memcpy(&drawList->_displaySize, &drawData->DisplaySize, sizeof(glm::vec2));

			drawList->_vertices.resize(imDrawList->VtxBuffer.Size);
			memcpy(drawList->_vertices.data(), imDrawList->VtxBuffer.Data, imDrawList->VtxBuffer.Size * sizeof(renderer::RenderCommandImGui::Vertex));

			drawList->_indices.resize(imDrawList->IdxBuffer.Size);
			memcpy(drawList->_indices.data(), imDrawList->IdxBuffer.Data, imDrawList->IdxBuffer.Size * sizeof(uint16_t));

			drawList->_commands.resize(imDrawList->CmdBuffer.Size);
			for (int cmdIndex = 0; cmdIndex < imDrawList->CmdBuffer.Size; ++cmdIndex)
			{
				ImDrawCmd& imCommand = imDrawList->CmdBuffer[cmdIndex];
				renderer::RenderCommandImGui::Command& command = drawList->_commands[cmdIndex];

				command._clipRect._position.x = imCommand.ClipRect.x;
				command._clipRect._position.y = imCommand.ClipRect.y;
				command._clipRect._size.x = imCommand.ClipRect.z - imCommand.ClipRect.x;
				command._clipRect._size.y = imCommand.ClipRect.w - imCommand.ClipRect.y;
				command._texture = reinterpret_cast<renderer::Texture*>(imCommand.TextureId);
				command._vertexOffset = imCommand.VtxOffset;
				command._indexOffset = imCommand.IdxOffset;
				command._elementCount = imCommand.ElemCount;
			}

			drawLists[drawListIndex] = drawList;
		}

		renderer::RenderCommandImGui* renderCommand = new renderer::RenderCommandImGui(drawLists);

		renderer::Renderer* renderer = renderer::Renderer::GetInstance();
		renderer->GetRenderQueue()->PushRenderCommand(renderCommand);
	}
}