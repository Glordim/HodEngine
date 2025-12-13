#include "HodEngine/ImGui/Pch.hpp"
#include "HodEngine/ImGui/ImGuiManager.hpp"

#include <HodEngine/Core/Job/Job.hpp>
#include <HodEngine/Core/Time/SystemTime.hpp>

#include "HodEngine/Window/PlatformWindow.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/DearImGui/imgui_internal.h"
#if defined(PLATFORM_MACOS)
	#include "HodEngine/ImGui/DearImGui/imgui_impl_osx.h"
#endif
#include "HodEngine/ImGui/RenderCommandImGui.hpp"

#include "HodEngine/ImGui/MainBar.hpp"
#include "HodEngine/ImGui/Window/Window.hpp"

#include <HodEngine/Renderer/FrameResources.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/RHI/PresentationSurface.hpp>
#include <HodEngine/Renderer/RHI/Texture.hpp>

#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/Core/Frame/FrameSequencer.hpp>

#include <HodEngine/Window/Desktop/DesktopDisplayManager.hpp>
#include <HodEngine/Window/Desktop/DesktopWindow.hpp>

#include "HodEngine/ImGui/Font/IconsMaterialDesignIcons.h"
#include "HodEngine/ImGui/Font/MaterialDesignIcons.ttf.hpp"
#include "HodEngine/ImGui/Font/Roboto-Regular.ttf.hpp"
#include "HodEngine/ImGui/Shader/ImGui_Fragment.hpp"
#include "HodEngine/ImGui/Shader/ImGui_Vertex.hpp"

#include <HodEngine/Renderer/RHI/VertexInput.hpp>

#include <cstring>
#include <HodEngine/Core/FileSystem/Path.hpp>

#if defined(PLATFORM_WINDOWS)
	#undef FindWindow
	#undef min
#endif

namespace hod::imgui
{
	/// @brief
	_SingletonConstructor(ImGuiManager)
	: _updateJob(this, &ImGuiManager::Update, JobQueue::Queue::Framed)
	{
	}

	/// @brief
	ImGuiManager::~ImGuiManager()
	{
		DestroyAllWindow();

		// ImGui::DestroyContext(); // todo
		for (renderer::Texture* texure : _textures)
		{
			DefaultAllocator::GetInstance().Delete(texure);
		}

		DefaultAllocator::GetInstance().Delete(_material);
		DefaultAllocator::GetInstance().Delete(_vertexShader);
		DefaultAllocator::GetInstance().Delete(_fragmentShader);
	}

	void embraceTheDarkness()
	{
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
		colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
		colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
		colors[ImGuiCol_Separator] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.08f, 1.0f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TabSelected] = ImVec4(0.141f, 0.141f, 0.141f, 1.0f);
		colors[ImGuiCol_TabDimmed] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.05f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
		colors[ImGuiCol_NavCursor] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(8.00f, 8.00f);
		style.FramePadding = ImVec2(5.00f, 2.00f);
		style.CellPadding = ImVec2(6.00f, 6.00f);
		style.ItemSpacing = ImVec2(6.00f, 6.00f);
		style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
		style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
		style.IndentSpacing = 25;
		style.ScrollbarSize = 15;
		style.GrabMinSize = 10;
		style.WindowBorderSize = 1;
		style.ChildBorderSize = 1;
		style.PopupBorderSize = 1;
		style.FrameBorderSize = 1;
		style.TabBorderSize = 1;
		style.WindowRounding = 7;
		style.ChildRounding = 4;
		style.FrameRounding = 3;
		style.PopupRounding = 4;
		style.ScrollbarRounding = 9;
		style.GrabRounding = 3;
		style.LogSliderDeadzone = 4;
		style.TabRounding = 4;
		style.DisabledAlpha = 0.5f;
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

		// ImGui::StyleColorsDark();
		embraceTheDarkness();

		Path projectsPath = FileSystem::GetUserSettingsPath();
		projectsPath /= ("HodEngine");
		projectsPath /= ("ImGui.ini");

		static char iniFileName[4096] = {'\0'};
		std::strcpy(iniFileName, projectsPath.GetString().CStr());

		ImGui::GetIO().IniFilename = iniFileName;
		ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_RendererHasTextures;
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// ImGui::GetIO().Fonts->AddFontDefault();
		/*
		ImFontConfig icons_configProggyClean;
		icons_configProggyClean.SizePixels = 13.0f;
		icons_configProggyClean.OversampleH = 1;
		icons_configProggyClean.OversampleV = 1;
		icons_configProggyClean.PixelSnapH = true;
		*/
		// ImGui::GetIO().Fonts->AddFontDefault(&icons_configProggyClean);

		ImFontConfig icons_configProggyClean;
		icons_configProggyClean.SizePixels = 13.0f;
		icons_configProggyClean.OversampleH = 1;
		icons_configProggyClean.OversampleV = 1;
		icons_configProggyClean.PixelSnapH = true;
#if defined(PLATFORM_MACOS)
		window::MacOsWindow* macOsWindow = static_cast<window::MacOsWindow*>(window);
		icons_configProggyClean.RasterizerDensity = macOsWindow->GetScaleFactor();
#endif
		ImGui::GetIO().Fonts->AddFontFromMemoryTTF(Roboto_Regular_ttf, Roboto_Regular_ttf_size, 16.0f, &icons_configProggyClean);

		const ImWchar iconsRangesMDI[] = {ICON_MIN_MDI, ICON_MAX_MDI, 0};
		ImFontConfig  icons_configMDI;
		icons_configMDI.MergeMode = true;
		icons_configMDI.PixelSnapH = true;
		icons_configMDI.GlyphOffset.y = 2;
		icons_configMDI.GlyphOffset.x = -0.5;
		icons_configMDI.GlyphMaxAdvanceX = 13;
#if defined(PLATFORM_MACOS)
		icons_configProggyClean.RasterizerDensity = macOsWindow->GetScaleFactor();
#endif

		ImGui::GetIO().Fonts->AddFontFromMemoryTTF(MaterialDesignIcons_ttf, MaterialDesignIcons_ttf_size, 16.0f, &icons_configMDI, iconsRangesMDI);

		_mainWindow = window;
#if defined(PLATFORM_MACOS)
		ImGui_ImplOSX_Init(static_cast<window::MacOsWindow*>(_mainWindow)->GetNsView());
#endif

		FrameSequencer::GetInstance()->InsertJob(&_updateJob, FrameSequencer::Step::PreRender);

		return true;
	}

	/// @brief
	/// @return
	window::Window* ImGuiManager::GetMainWindow() const
	{
		return _mainWindow;
	}

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
	void ImGuiManager::DestroyAllWindow()
	{
		for (Window* window : _windows)
		{
			DefaultAllocator::GetInstance().Delete(window);
		}
		_windows.Clear();
	}

	/// @brief
	void ImGuiManager::Update()
	{
		ImGuiIO& io = ImGui::GetIO();

#if defined(PLATFORM_MACOS)
		window::DesktopWindow* window = static_cast<window::DesktopWindow*>(_mainWindow);
		ImGui_ImplOSX_NewFrame(static_cast<window::MacOsWindow*>(window)->GetNsView());
		// To avoid having animations (ping, translation, etc.) that lag too much (and to keep them at a reasonable duration) in the editor, even when there is lag.
		io.DeltaTime = std::min(io.DeltaTime, 1.0f / 30.0f);
#else
		static SystemTime::TimeStamp last = SystemTime::INVALID_TIMESTAMP;
		SystemTime::TimeStamp        now = SystemTime::Now();
		io.DeltaTime = SystemTime::ElapsedTimeInSeconds(last, now);
		last = now;

		// To avoid having animations (ping, translation, etc.) that lag too much (and to keep them at a reasonable duration) in the editor, even when there is lag.
		io.DeltaTime = std::min(io.DeltaTime, 1.0f / 30.0f);

		window::Event event;
		uint32_t      eventIndex = 0;
		while (_mainWindow->PollEvent(eventIndex, event))
		{
			if (event.type == window::EventType::MouseMoved)
			{
				io.AddMousePosEvent(event.data.mouseMove.x, event.data.mouseMove.y);
			}
			else if (event.type == window::EventType::MouseButtonDown)
			{
				io.AddMouseButtonEvent(static_cast<ImGuiMouseButton>(event.data.mouseButton.button), true);
			}
			else if (event.type == window::EventType::MouseButtonUp)
			{
				io.AddMouseButtonEvent(static_cast<ImGuiMouseButton>(event.data.mouseButton.button), false);
			}
			else if (event.type == window::EventType::MouseScroll)
			{
				io.AddMouseWheelEvent(0.0f, event.data.mouseScroll.value);
			}
			else if (event.type == window::EventType::MouseHorizontalScroll)
			{
				io.AddMouseWheelEvent(event.data.mouseScroll.value, 0.0f);
			}
			else if (event.type == window::EventType::FocusGained)
			{
				io.AddFocusEvent(true);
			}
			else if (event.type == window::EventType::FocusLost)
			{
				io.AddFocusEvent(false);
			}
		}

		Vector2 resolution;

		renderer::PresentationSurface* presentationSurface = renderer::Renderer::GetInstance()->FindPresentationSurface(_mainWindow);
		if (presentationSurface)
		{
			resolution = presentationSurface->GetResolution();
		}

		if (resolution == Vector2::Zero)
		{
			return;
		}
		io.DisplaySize.x = resolution.GetX();
		io.DisplaySize.y = resolution.GetY();

	#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_MACOS) || defined(PLATFORM_LINUX)
		ImGuiMouseCursor mouseCursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();

		static window::DesktopDisplayManager::BuiltinCursor builtinCursorMapping[ImGuiMouseCursor_COUNT] = {
			window::DesktopDisplayManager::BuiltinCursor::Arrow,    // ImGuiMouseCursor_Arrow
			window::DesktopDisplayManager::BuiltinCursor::Ibeam,    // ImGuiMouseCursor_TextInput
			window::DesktopDisplayManager::BuiltinCursor::SizeNS,   // ImGuiMouseCursor_ResizeAll
			window::DesktopDisplayManager::BuiltinCursor::SizeNS,   // ImGuiMouseCursor_ResizeNS
			window::DesktopDisplayManager::BuiltinCursor::SizeWE,   // ImGuiMouseCursor_ResizeEW
			window::DesktopDisplayManager::BuiltinCursor::SizeNESW, // ImGuiMouseCursor_ResizeNESW
			window::DesktopDisplayManager::BuiltinCursor::SizeNWSE, // ImGuiMouseCursor_ResizeNWSE
			window::DesktopDisplayManager::BuiltinCursor::Hand,     // ImGuiMouseCursor_Hand
			window::DesktopDisplayManager::BuiltinCursor::Wait,     // ImGuiMouseCursor_Wait
			window::DesktopDisplayManager::BuiltinCursor::Wait,     // ImGuiMouseCursor_Progress
			window::DesktopDisplayManager::BuiltinCursor::No,       // ImGuiMouseCursor_NotAllowed
		};
		static_cast<window::DesktopWindow*>(_mainWindow)->SetCursor(window::DesktopDisplayManager::GetInstance()->GetBultinCursor(builtinCursorMapping[mouseCursor]));
	#endif
#endif

		ImGui::NewFrame();

		if (_mainBar != nullptr)
		{
			// float mainbarhieght = ImGui::GetStyle().FramePadding.y;
			// ImGui::GetStyle().FramePadding.y = 16;
			if (ImGui::BeginMainMenuBar() == true)
			{
				// ImGui::GetStyle().FramePadding.y = mainbarhieght;
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
		//_centralDockSpace = ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

		static bool showDemo = false;
		if (showDemo == true)
		{
			ImGui::ShowDemoWindow(&showDemo);
		}

		if (_callback != nullptr)
		{
			_callback();
		}

		Vector<Window*>::Iterator windowIt = _windows.Begin();
		Vector<Window*>::Iterator windowItEnd = _windows.End();
		while (windowIt != windowItEnd)
		{
			if ((*windowIt)->IsClosed() == true)
			{
				DefaultAllocator::GetInstance().Delete(*windowIt);
				windowIt = _windows.Erase(windowIt);
				windowItEnd = _windows.End();
			}
			else
			{
				++windowIt;
			}
		}

		for (uint32_t index = 0; index < _windows.Size(); ++index)
		{
			Window* window = _windows[index];
			/*
			ImGuiWindowClass window_class;
			window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
			ImGui::SetNextWindowClass(&window_class);
			*/
			/*
			bool open = true;
			if (ImGui::Begin(window->GetIdentifier(), &open) == true)
			{
			    ImRect cliprect = ImGui::GetCurrentWindow()->ClipRect;
			    cliprect.Min.x -= ImGui::GetStyle().WindowPadding.x * 0.5f;
			    cliprect.Min.y -= ImGui::GetStyle().WindowPadding.y * 0.5f;
			    cliprect.Max.x += ImGui::GetStyle().WindowPadding.x * 0.5f;
			    cliprect.Max.y += ImGui::GetStyle().WindowPadding.y * 0.5f;
			    ImGui::PopClipRect();
			    ImGui::PushClipRect(cliprect.Min, cliprect.Max, false);
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
			*/
			window->Draw();
		}

		ImGui::Render();

		ImDrawData* drawData = ImGui::GetDrawData();

		if (drawData->Textures != nullptr)
		{
			for (ImTextureData* textureData : *drawData->Textures)
			{
				switch (textureData->Status)
				{
					case ImTextureStatus_OK:
					{
						// Nothing to do
					}
					break;

					case ImTextureStatus_WantCreate:
					{
						renderer::Texture*                 texture = renderer::Renderer::GetInstance()->CreateTexture();
						hod::renderer::Texture::CreateInfo createInfo;
						createInfo._allowReadWrite = false;
						createInfo._filterMode = renderer::FilterMode::Linear;
						createInfo._wrapMode = renderer::WrapMode::Clamp;
						assert(textureData->Format == ImTextureFormat_RGBA32 && textureData->BytesPerPixel == 4);
						if (texture->BuildBuffer(textureData->Width, textureData->Height, textureData->Pixels, createInfo) == false)
						{
							assert(false);
						}
						textureData->SetTexID(texture);
						textureData->SetStatus(ImTextureStatus_OK);

						_textures.push_back(texture);
					}
					break;

					case ImTextureStatus_WantUpdates:
					{
						for (uint32_t i = 0; i < _textures.Size(); ++i)
						{
							if (_textures[i] == textureData->TexID)
							{
								_textures.Erase(_textures.Begin() + i);
								break;
							}
						}
						renderer::Renderer::GetInstance()->DestroyTexture(textureData->TexID);
						textureData->SetTexID(nullptr);
						textureData->SetStatus(ImTextureStatus_Destroyed);

						renderer::Texture*                 texture = renderer::Renderer::GetInstance()->CreateTexture();
						hod::renderer::Texture::CreateInfo createInfo;
						createInfo._allowReadWrite = false;
						createInfo._filterMode = renderer::FilterMode::Linear;
						createInfo._wrapMode = renderer::WrapMode::Clamp;
						assert(textureData->Format == ImTextureFormat_RGBA32 && textureData->BytesPerPixel == 4);
						if (texture->BuildBuffer(textureData->Width, textureData->Height, textureData->Pixels, createInfo) == false)
						{
							assert(false);
						}
						textureData->SetTexID(texture);
						textureData->SetStatus(ImTextureStatus_OK);

						_textures.push_back(texture);
					}
					break;

					case ImTextureStatus_WantDestroy:
					{
						for (uint32_t i = 0; i < _textures.Size(); ++i)
						{
							if (_textures[i] == textureData->TexID)
							{
								_textures.Erase(_textures.Begin() + i);
								break;
							}
						}
						renderer::Renderer::GetInstance()->DestroyTexture(textureData->TexID);
						textureData->SetTexID(nullptr);
						textureData->SetStatus(ImTextureStatus_Destroyed);
					}
					break;

					case ImTextureStatus_Destroyed:
					{
						// Nothing to do
					}
					break;
				}
			}
		}

		ImVec2 clipOffset = drawData->DisplayPos;
		ImVec2 clipScale = drawData->FramebufferScale;

		Vector<RenderCommandImGui::DrawList*> drawLists;
		drawLists.Resize(drawData->CmdListsCount);

		for (int drawListIndex = 0; drawListIndex < drawData->CmdListsCount; ++drawListIndex)
		{
			ImDrawList* imDrawList = drawData->CmdLists[drawListIndex];

			RenderCommandImGui::DrawList* drawList = DefaultAllocator::GetInstance().New<RenderCommandImGui::DrawList>();

			memcpy((void*)&drawList->_displayPosition, &drawData->DisplayPos, sizeof(Vector2));
			memcpy((void*)&drawList->_displaySize, &drawData->DisplaySize, sizeof(Vector2));

			drawList->_vertices.Resize(imDrawList->VtxBuffer.Size);
			memcpy((void*)drawList->_vertices.Data(), imDrawList->VtxBuffer.Data, imDrawList->VtxBuffer.Size * sizeof(RenderCommandImGui::Vertex));

			drawList->_indices.Resize(imDrawList->IdxBuffer.Size);
			memcpy((void*)drawList->_indices.Data(), imDrawList->IdxBuffer.Data, imDrawList->IdxBuffer.Size * sizeof(uint16_t));

			drawList->_commands.Resize(imDrawList->CmdBuffer.Size);
			for (int cmdIndex = 0; cmdIndex < imDrawList->CmdBuffer.Size; ++cmdIndex)
			{
				ImDrawCmd&                   imCommand = imDrawList->CmdBuffer[cmdIndex];
				RenderCommandImGui::Command& command = drawList->_commands[cmdIndex];

				// Project scissor/clipping rectangles into framebuffer space
				ImVec2 clipMin((imCommand.ClipRect.x - clipOffset.x) * clipScale.x, (imCommand.ClipRect.y - clipOffset.y) * clipScale.y);
				ImVec2 clipMax((imCommand.ClipRect.z - clipOffset.x) * clipScale.x, (imCommand.ClipRect.w - clipOffset.y) * clipScale.y);

				command._clipRect._position.SetX(clipMin.x);
				command._clipRect._position.SetY(clipMin.y);
				command._clipRect._size.SetX(clipMax.x - clipMin.x);
				command._clipRect._size.SetY(clipMax.y - clipMin.y);
				command._texture = reinterpret_cast<renderer::Texture*>(imCommand.GetTexID());
				command._vertexOffset = imCommand.VtxOffset;
				command._indexOffset = imCommand.IdxOffset;
				command._elementCount = imCommand.ElemCount;
			}

			drawLists[drawListIndex] = drawList;
		}

		Rect viewport;
		viewport._position.SetX(0.0f);
		viewport._position.SetY(ImGui::GetIO().DisplaySize.y * ImGui::GetIO().DisplayFramebufferScale.y);
		viewport._size.SetX(ImGui::GetIO().DisplaySize.x * ImGui::GetIO().DisplayFramebufferScale.x);
		viewport._size.SetY(-ImGui::GetIO().DisplaySize.y * ImGui::GetIO().DisplayFramebufferScale.y);

		RenderCommandImGui* renderCommand = DefaultAllocator::GetInstance().New<RenderCommandImGui>(drawLists, viewport);

		renderer::RenderView* renderView = renderer::Renderer::GetInstance()->GetCurrentFrameResources().CreateRenderView();
		renderView->Init();
		renderView->Prepare(_mainWindow);
		renderView->SetupCamera(Matrix4::Identity, Matrix4::Identity, viewport);
		renderView->PushRenderCommand(renderCommand);
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
				{0, 0, renderer::VertexInput::Format::R32G32_SFloat},
				{0, 8, renderer::VertexInput::Format::R32G32_SFloat},
				{0, 16, renderer::VertexInput::Format::R8G8B8A8_UNorm},
			};

			_vertexShader = renderer->CreateShader(renderer::Shader::ShaderType::Vertex);
			if (_vertexShader->LoadFromIR(ImGui_Vertex, ImGui_Vertex_size) == false)
			{
				DefaultAllocator::GetInstance().Delete(_vertexShader);
				return false;
			}

			_fragmentShader = renderer->CreateShader(renderer::Shader::ShaderType::Fragment);
			if (_fragmentShader->LoadFromIR(ImGui_Fragment, ImGui_Fragment_size) == false)
			{
				DefaultAllocator::GetInstance().Delete(_vertexShader);
				DefaultAllocator::GetInstance().Delete(_fragmentShader);
				return false;
			}

			_material = renderer->CreateMaterial(vertexInput, 3, _vertexShader, _fragmentShader);
			if (_material == nullptr)
			{
				DefaultAllocator::GetInstance().Delete(_material);
				DefaultAllocator::GetInstance().Delete(_vertexShader);
				DefaultAllocator::GetInstance().Delete(_fragmentShader);
				return false;
			}

			// todo error handling ?
		}
		return true;
	}

	/// @brief
	/// @param window
	void ImGuiManager::OpenWindow(Window* window)
	{
		_windows.push_back(window); // todo check uniq
	}

	/// @brief
	/// @param windowDescription
	/// @return
	Window* ImGuiManager::FindWindow(ReflectionDescriptor& windowDescription) const
	{
		for (Window* window : _windows)
		{
			if (&window->GetReflectionDescriptorV() == &windowDescription)
			{
				return window;
			}
		}
		return nullptr;
	}

	/// @brief
	/// @param windowDescription
	/// @return
	Vector<Window*> ImGuiManager::FindWindows(ReflectionDescriptor& windowDescription) const
	{
		Vector<Window*> windows;

		for (Window* window : _windows)
		{
			if (&window->GetReflectionDescriptorV() == &windowDescription)
			{
				windows.push_back(window);
			}
		}

		return windows;
	}

	/// @brief
	/// @return
	ImGuiID ImGuiManager::GetCentralDockSpace() const
	{
		return _centralDockSpace;
	}
}
