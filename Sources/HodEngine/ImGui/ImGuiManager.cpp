#include "HodEngine/ImGui/ImGuiManager.h"

#include <HodEngine/Core/Job/Job.h>

#if defined(PLATFORM_WINDOWS)
	#include <HodEngine/Window/Desktop/Windows/Win32/Win32Window.h>
#endif

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/DearImGui/imgui_impl_win32.h"
//#include "HodEngine/ImGui/imgui_impl_vulkan.h"
#include "HodEngine/ImGui/DearImGui/ImGuizmo.h"
#include "HodEngine/ImGui/RenderCommandImGui.h"

#include "HodEngine/ImGui/Window.h"
#include "HodEngine/ImGui/MainBar.h"

#include <HodEngine/Renderer/Renderer.h>
#include <HodEngine/Renderer/RHI/Texture.h>

#include <HodEngine/Core/Frame/FrameSequencer.h>
#include <HodEngine/Core/FileSystem.h>

#include <HodEngine/Application/Application.h>
#include <HodEngine/Application/GraphicApplications/GraphicApplication.h>

#include <HodEngine/Window/Desktop/DesktopWindow.h>
#include <HodEngine/Window/Desktop/DesktopDisplayManager.h>

#include "HodEngine/ImGui/Shader/Generated/imgui.vert.h"
#include "HodEngine/ImGui/Shader/Generated/imgui.frag.h"

#include "HodEngine/Renderer/RHI/VertexInput.h"

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
		ImGuizmo::BeginFrame();

		if (_mainBar != nullptr)
		{
			if (ImGui::BeginMainMenuBar() == true)
			{
				_mainBar->Draw();
				ImGui::EndMainMenuBar();
			}
		}

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

	static uint32_t __glsl_shader_vert_spv[] =
	{
		0x07230203,0x00010300,0x0008000a,0x0000002e,0x00000000,0x00020011,0x00000001,0x0006000b,
		0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
		0x000a000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000b,0x0000000f,0x00000015,
		0x0000001b,0x0000001c,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
		0x00000000,0x00030005,0x00000009,0x00000000,0x00050006,0x00000009,0x00000000,0x6f6c6f43,
		0x00000072,0x00040006,0x00000009,0x00000001,0x00005655,0x00030005,0x0000000b,0x0074754f,
		0x00040005,0x0000000f,0x6c6f4361,0x0000726f,0x00030005,0x00000015,0x00565561,0x00060005,
		0x00000019,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x00000019,0x00000000,
		0x505f6c67,0x7469736f,0x006e6f69,0x00030005,0x0000001b,0x00000000,0x00040005,0x0000001c,
		0x736f5061,0x00000000,0x00060005,0x0000001e,0x73755075,0x6e6f4368,0x6e617473,0x00000074,
		0x00050006,0x0000001e,0x00000000,0x61635375,0x0000656c,0x00060006,0x0000001e,0x00000001,
		0x61725475,0x616c736e,0x00006574,0x00030005,0x00000020,0x00006370,0x00040047,0x0000000b,
		0x0000001e,0x00000000,0x00040047,0x0000000f,0x0000001e,0x00000002,0x00040047,0x00000015,
		0x0000001e,0x00000001,0x00050048,0x00000019,0x00000000,0x0000000b,0x00000000,0x00030047,
		0x00000019,0x00000002,0x00040047,0x0000001c,0x0000001e,0x00000000,0x00050048,0x0000001e,
		0x00000000,0x00000023,0x00000000,0x00050048,0x0000001e,0x00000001,0x00000023,0x00000008,
		0x00030047,0x0000001e,0x00000002,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
		0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040017,
		0x00000008,0x00000006,0x00000002,0x0004001e,0x00000009,0x00000007,0x00000008,0x00040020,
		0x0000000a,0x00000003,0x00000009,0x0004003b,0x0000000a,0x0000000b,0x00000003,0x00040015,
		0x0000000c,0x00000020,0x00000001,0x0004002b,0x0000000c,0x0000000d,0x00000000,0x00040020,
		0x0000000e,0x00000001,0x00000007,0x0004003b,0x0000000e,0x0000000f,0x00000001,0x00040020,
		0x00000011,0x00000003,0x00000007,0x0004002b,0x0000000c,0x00000013,0x00000001,0x00040020,
		0x00000014,0x00000001,0x00000008,0x0004003b,0x00000014,0x00000015,0x00000001,0x00040020,
		0x00000017,0x00000003,0x00000008,0x0003001e,0x00000019,0x00000007,0x00040020,0x0000001a,
		0x00000003,0x00000019,0x0004003b,0x0000001a,0x0000001b,0x00000003,0x0004003b,0x00000014,
		0x0000001c,0x00000001,0x0004001e,0x0000001e,0x00000008,0x00000008,0x00040020,0x0000001f,
		0x00000009,0x0000001e,0x0004003b,0x0000001f,0x00000020,0x00000009,0x00040020,0x00000021,
		0x00000009,0x00000008,0x0004002b,0x00000006,0x00000028,0x00000000,0x0004002b,0x00000006,
		0x00000029,0x3f800000,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
		0x00000005,0x0004003d,0x00000007,0x00000010,0x0000000f,0x00050041,0x00000011,0x00000012,
		0x0000000b,0x0000000d,0x0003003e,0x00000012,0x00000010,0x0004003d,0x00000008,0x00000016,
		0x00000015,0x00050041,0x00000017,0x00000018,0x0000000b,0x00000013,0x0003003e,0x00000018,
		0x00000016,0x0004003d,0x00000008,0x0000001d,0x0000001c,0x00050041,0x00000021,0x00000022,
		0x00000020,0x0000000d,0x0004003d,0x00000008,0x00000023,0x00000022,0x00050085,0x00000008,
		0x00000024,0x0000001d,0x00000023,0x00050041,0x00000021,0x00000025,0x00000020,0x00000013,
		0x0004003d,0x00000008,0x00000026,0x00000025,0x00050081,0x00000008,0x00000027,0x00000024,
		0x00000026,0x00050051,0x00000006,0x0000002a,0x00000027,0x00000000,0x00050051,0x00000006,
		0x0000002b,0x00000027,0x00000001,0x00070050,0x00000007,0x0000002c,0x0000002a,0x0000002b,
		0x00000028,0x00000029,0x00050041,0x00000011,0x0000002d,0x0000001b,0x0000000d,0x0003003e,
		0x0000002d,0x0000002c,0x000100fd,0x00010038
	};

	static uint32_t __glsl_shader_frag_spv[] =
	{
		0x07230203,0x00010300,0x0008000a,0x0000001e,0x00000000,0x00020011,0x00000001,0x0006000b,
		0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
		0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000d,0x00030010,
		0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
		0x00000000,0x00040005,0x00000009,0x6c6f4366,0x0000726f,0x00030005,0x0000000b,0x00000000,
		0x00050006,0x0000000b,0x00000000,0x6f6c6f43,0x00000072,0x00040006,0x0000000b,0x00000001,
		0x00005655,0x00030005,0x0000000d,0x00006e49,0x00050005,0x00000016,0x78655473,0x65727574,
		0x00000000,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000d,0x0000001e,
		0x00000000,0x00040047,0x00000016,0x00000022,0x00000000,0x00040047,0x00000016,0x00000021,
		0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
		0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,
		0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x00040017,0x0000000a,0x00000006,
		0x00000002,0x0004001e,0x0000000b,0x00000007,0x0000000a,0x00040020,0x0000000c,0x00000001,
		0x0000000b,0x0004003b,0x0000000c,0x0000000d,0x00000001,0x00040015,0x0000000e,0x00000020,
		0x00000001,0x0004002b,0x0000000e,0x0000000f,0x00000000,0x00040020,0x00000010,0x00000001,
		0x00000007,0x00090019,0x00000013,0x00000006,0x00000001,0x00000000,0x00000000,0x00000000,
		0x00000001,0x00000000,0x0003001b,0x00000014,0x00000013,0x00040020,0x00000015,0x00000000,
		0x00000014,0x0004003b,0x00000015,0x00000016,0x00000000,0x0004002b,0x0000000e,0x00000018,
		0x00000001,0x00040020,0x00000019,0x00000001,0x0000000a,0x00050036,0x00000002,0x00000004,
		0x00000000,0x00000003,0x000200f8,0x00000005,0x00050041,0x00000010,0x00000011,0x0000000d,
		0x0000000f,0x0004003d,0x00000007,0x00000012,0x00000011,0x0004003d,0x00000014,0x00000017,
		0x00000016,0x00050041,0x00000019,0x0000001a,0x0000000d,0x00000018,0x0004003d,0x0000000a,
		0x0000001b,0x0000001a,0x00050057,0x00000007,0x0000001c,0x00000017,0x0000001b,0x00050085,
		0x00000007,0x0000001d,0x00000012,0x0000001c,0x0003003e,0x00000009,0x0000001d,0x000100fd,
		0x00010038
	};

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
