#include "HodEngine/ImGui/ImGuiManager.hpp"

#include <HodEngine/Core/Job/Job.hpp>

#if defined(PLATFORM_WINDOWS)
	#include <HodEngine/Window/Desktop/Windows/Win32/Win32Window.hpp>
#endif

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/DearImGui/imgui_internal.h"
#include "HodEngine/ImGui/DearImGui/imgui_impl_win32.h"
#include "HodEngine/ImGui/RenderCommandImGui.hpp"

#include "HodEngine/ImGui/Window.hpp"
#include "HodEngine/ImGui/MainBar.hpp"

#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RHI/Texture.hpp>

#include <HodEngine/Core/Frame/FrameSequencer.hpp>
#include <HodEngine/Core/FileSystem.hpp>

#include <HodEngine/Application/Application.hpp>
#include <HodEngine/Application/GraphicApplications/GraphicApplication.hpp>

#include <HodEngine/Window/Desktop/DesktopWindow.hpp>
#include <HodEngine/Window/Desktop/DesktopDisplayManager.hpp>

#include "HodEngine/ImGui/Shader/Generated/imgui.vert.hpp"
#include "HodEngine/ImGui/Shader/Generated/imgui.frag.hpp"
#include "HodEngine/ImGui/Font/MaterialIcons-Regular.ttf.h"
#include "HodEngine/ImGui/Font/IconsMaterialDesign.h"

#include "HodEngine/Renderer/RHI/VertexInput.hpp"

#include <filesystem>
#include <cstring>

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

		delete _material;
		delete _vertexShader;
		delete _fragmentShader;
	}

	/// @brief 
	/// @param window 
	/// @return 
	bool ImGuiManager::Init(window::Window* window)
	{
		if (CreateMaterial() == false)
		{
			return false;
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		std::filesystem::path projectsPath = FileSystem::GetUserSettingsPath();
		projectsPath /= ("HodEngine");
		projectsPath /= ("ImGui.ini");

		static char iniFileName[4096] = {'\0'};
		std::strcpy(iniFileName, projectsPath.string().c_str());

		ImGui::GetIO().IniFilename = iniFileName;

		//ImGui::GetIO().Fonts->AddFontDefault();
		ImFontConfig icons_configProggyClean;
		icons_configProggyClean.SizePixels = 13.0f;
		icons_configProggyClean.OversampleH = 1;
		icons_configProggyClean.OversampleV = 1;
		icons_configProggyClean.PixelSnapH = true;
		ImGui::GetIO().Fonts->AddFontDefault(&icons_configProggyClean);

		const ImWchar iconsRangesMDI[] = { ICON_MIN_MD, ICON_MAX_MD, 0 };
		ImFontConfig icons_configMDI;
		icons_configMDI.MergeMode = true;
		icons_configMDI.PixelSnapH = true;
		icons_configMDI.GlyphOffset.y = 2;
		icons_configMDI.GlyphOffset.x = -0.5;
		icons_configMDI.GlyphMaxAdvanceX = 13;

		ImGui::GetIO().Fonts->AddFontFromMemoryTTF(MaterialIcons_Regular_ttf, MaterialIcons_Regular_ttf_size, 16.0f, &icons_configMDI, iconsRangesMDI);
		ImGui::GetIO().Fonts->Build();

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
#if defined(PLATFORM_WINDOWS)
		ImGui_ImplWin32_NewFrame();
#elif defined(PLATFORM_LINUX)
		window::DesktopWindow* window = (window::DesktopWindow*)application::GraphicApplication::GetInstance()->GetWindow();

		ImGui::GetIO().DisplaySize.x = window->GetWidth();
		ImGui::GetIO().DisplaySize.y = window->GetHeight();
		ImGui::GetIO().AddMousePosEvent(window->GetMousePosition().GetX(), window->GetMousePosition().GetY());
		ImGui::GetIO().AddMouseButtonEvent(ImGuiMouseButton_Left, window->GetMouseButton(window::DesktopWindow::MouseButton::Left));
		ImGui::GetIO().AddMouseButtonEvent(ImGuiMouseButton_Middle, window->GetMouseButton(window::DesktopWindow::MouseButton::Middle));
		ImGui::GetIO().AddMouseButtonEvent(ImGuiMouseButton_Right, window->GetMouseButton(window::DesktopWindow::MouseButton::Right));
#else
		return;
#endif
		ImGui::NewFrame();

		if (_mainBar != nullptr)
		{
			float mainbarhieght = ImGui::GetStyle().FramePadding.y;
			ImGui::GetStyle().FramePadding.y = 16;
			if (ImGui::BeginMainMenuBar() == true)
			{
				ImGui::GetStyle().FramePadding.y = mainbarhieght;
				_mainBar->Draw();
				ImGui::EndMainMenuBar();
			}
		}
/*
		{
			ImGuiViewport* viewport = (ImGuiViewport*)(void*)ImGui::GetMainViewport();

			// For the main menu bar, which cannot be moved, we honor g.Style.DisplaySafeAreaPadding to ensure text can be visible on a TV set.
			// FIXME: This could be generalized as an opt-in way to clamp window->DC.CursorStartPos to avoid SafeArea?
			// FIXME: Consider removing support for safe area down the line... it's messy. Nowadays consoles have support for TV calibration in OS settings.
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
			float height = ImGui::GetFrameHeight();
			bool is_open = ImGui::BeginViewportSideBar("##PlayMenuBar", viewport, ImGuiDir_Up, height, window_flags);

			if (is_open)
			{
        		ImGui::BeginMenuBar();
				ImGui::EndMenuBar();
				ImGui::End();
			}
    		else
			{
        		ImGui::End();
			}
		}
*/
		ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

		static bool showDemo = false;
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
			bool open = true;
			if (ImGui::Begin(window->GetIdentifier(), &open) == true)
			{
				window->Draw();
			}
			if (open == false)
			{
				window->Close();
			}
			else if (ImGui::IsWindowFocused() == true)
			{
				_activeWindow = window;
			}
			ImGui::End();
		}

		ImGui::Render();

		ImDrawData* drawData = ImGui::GetDrawData();

		std::vector<RenderCommandImGui::DrawList*> drawLists;
		drawLists.resize(drawData->CmdListsCount);

		for (int drawListIndex = 0; drawListIndex < drawData->CmdListsCount; ++drawListIndex)
		{
			ImDrawList* imDrawList = drawData->CmdLists[drawListIndex];

			RenderCommandImGui::DrawList* drawList = new RenderCommandImGui::DrawList();

			memcpy(&drawList->_displayPosition, &drawData->DisplayPos, sizeof(Vector2));
			memcpy(&drawList->_displaySize, &drawData->DisplaySize, sizeof(Vector2));

			drawList->_vertices.resize(imDrawList->VtxBuffer.Size);
			memcpy(drawList->_vertices.data(), imDrawList->VtxBuffer.Data, imDrawList->VtxBuffer.Size * sizeof(RenderCommandImGui::Vertex));

			drawList->_indices.resize(imDrawList->IdxBuffer.Size);
			memcpy(drawList->_indices.data(), imDrawList->IdxBuffer.Data, imDrawList->IdxBuffer.Size * sizeof(uint16_t));

			drawList->_commands.resize(imDrawList->CmdBuffer.Size);
			for (int cmdIndex = 0; cmdIndex < imDrawList->CmdBuffer.Size; ++cmdIndex)
			{
				ImDrawCmd& imCommand = imDrawList->CmdBuffer[cmdIndex];
				RenderCommandImGui::Command& command = drawList->_commands[cmdIndex];

				command._clipRect._position.SetX(imCommand.ClipRect.x);
				command._clipRect._position.SetY(imCommand.ClipRect.y);
				command._clipRect._size.SetX(imCommand.ClipRect.z - imCommand.ClipRect.x);
				command._clipRect._size.SetY(imCommand.ClipRect.w - imCommand.ClipRect.y);
				command._texture = reinterpret_cast<renderer::Texture*>(imCommand.TextureId);
				command._vertexOffset = imCommand.VtxOffset;
				command._indexOffset = imCommand.IdxOffset;
				command._elementCount = imCommand.ElemCount;
			}

			drawLists[drawListIndex] = drawList;
		}

		Rect viewport;
		viewport._position.SetX(0.0f);
		viewport._position.SetY(0.0f);
		viewport._size.SetX(ImGui::GetIO().DisplaySize.x);
		viewport._size.SetY(ImGui::GetIO().DisplaySize.y);
		
		RenderCommandImGui* renderCommand = new RenderCommandImGui(drawLists, viewport);

		renderer::Renderer* renderer = renderer::Renderer::GetInstance();
		renderer->GetRenderQueue()->PushRenderCommand(renderCommand);
	}

	/// @brief 
	/// @return 
	Window* ImGuiManager::GetActiveWindow() const
	{
		return _activeWindow;
	}

	/// @brief 
	/// @return 
	renderer::Material* ImGuiManager::GetMaterial() const
	{
		return _material;
	}

	/// @brief 
	/// @return 
	bool ImGuiManager::CreateMaterial()
	{
		if (_material == nullptr)
		{
			renderer::Renderer* renderer = renderer::Renderer::GetInstance();

			renderer::VertexInput vertexInput[3] = {
				{ 0, renderer::VertexInput::Format::R32G32_SFloat },
				{ 8, renderer::VertexInput::Format::R32G32_SFloat },
				{ 16, renderer::VertexInput::Format::A8B8G8R8_UNorm_Pack32 },
			};

			_vertexShader = renderer->CreateShader(renderer::Shader::ShaderType::Vertex);
			//if (_vertexShader->LoadFromMemory((void*)__glsl_shader_vert_spv, sizeof(__glsl_shader_vert_spv)) == false)
			if (_vertexShader->LoadFromMemory(imgui_vert, imgui_vert_size) == false)
			{
				delete _vertexShader;
				_vertexShader = nullptr;
				return false;
			}

			_fragmentShader = renderer->CreateShader(renderer::Shader::ShaderType::Fragment);
			if (_fragmentShader->LoadFromMemory(imgui_frag, imgui_frag_size) == false)
			{
				delete _vertexShader;
				_vertexShader = nullptr;

				delete _fragmentShader;
				_fragmentShader = nullptr;
				return false;
			}

			_material = renderer->CreateMaterial(vertexInput, 3, _vertexShader, _fragmentShader);
			// todo error handling ?
		}
		return true;
	}
}
