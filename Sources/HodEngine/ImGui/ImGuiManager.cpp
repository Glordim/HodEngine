#include "HodEngine/ImGui/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Core/Vector.hpp"
#include "HodEngine/ImGui/ImGuiManager.hpp"

#include "HodEngine/Window/Desktop/Windows/Win32/Win32Window.hpp"
#include "HodEngine/Window/Window.hpp"
#include <HodEngine/Core/Time/SystemTime.hpp>
#include <HodEngine/GameSystems/Job/Job.hpp>

#include "HodEngine/Window/PlatformWindow.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"
#include "HodEngine/ImGui/DearImGui/imgui_internal.h"
#include <cstdint>
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
#include <HodEngine/GameSystems/Frame/FrameSequencer.hpp>

#include <HodEngine/Window/Desktop/DesktopDisplayManager.hpp>
#include <HodEngine/Window/Desktop/DesktopWindow.hpp>

#include "Font/MaterialDesignIcons.ttf.hpp"
#include "Font/Roboto-Regular.ttf.hpp"
#include "HodEngine/ImGui/Font/IconsMaterialDesignIcons.h"
#include "Shader/ImGui_Fragment.hpp"
#include "Shader/ImGui_Vertex.hpp"

#include <HodEngine/Renderer/RHI/VertexInput.hpp>

#include <cstring>
#include <HodEngine/Core/FileSystem/Path.hpp>

#if defined(PLATFORM_WINDOWS)
	#undef FindWindow
	#undef min
#endif

namespace hod::inline imgui
{
	void ImGuiManager::PlatformCreateWindow(ImGuiViewport* vp)
	{
		DesktopWindow* desktopWindow = (DesktopWindow*)DesktopDisplayManager::GetInstance()->CreateWindow();
		#if PLATFORM_WINDOWS
			Win32Window* win32Window = static_cast<Win32Window*>(desktopWindow);
			while (win32Window->GetWindowHandle() == NULL) {}
		#endif
		desktopWindow->SetDecoration((vp->Flags & ImGuiViewportFlags_NoDecoration) == 0);
		desktopWindow->SetSize(vp->Size.x, vp->Size.y);
		vp->PlatformHandle = desktopWindow;
	}

	void ImGuiManager::PlatformDestroyWindow(ImGuiViewport* vp)
	{
		DesktopWindow* desktopWindow = static_cast<DesktopWindow*>(vp->PlatformHandle);
		DesktopDisplayManager::GetInstance()->DestroyWindow(desktopWindow);
		vp->PlatformHandle = nullptr;
	}

	void ImGuiManager::PlatformShowWindow(ImGuiViewport* vp)
	{
		DesktopWindow* desktopWindow = static_cast<DesktopWindow*>(vp->PlatformHandle);
		desktopWindow->SetVisible(true);
	}

	void ImGuiManager::PlatformSetWindowPos(ImGuiViewport* vp, ImVec2 pos)
	{
		DesktopWindow* desktopWindow = static_cast<DesktopWindow*>(vp->PlatformHandle);
		desktopWindow->SetPosition(Vector2(pos.x, pos.y));
	}

	ImVec2 ImGuiManager::PlatformGetWindowPos(ImGuiViewport* vp)
	{
		DesktopWindow* desktopWindow = static_cast<DesktopWindow*>(vp->PlatformHandle);

		ImVec2 pos;
		pos.x = desktopWindow->GetPosition().GetX();
		pos.y = desktopWindow->GetPosition().GetY();
		return pos;
	}

	void ImGuiManager::PlatformSetWindowSize(ImGuiViewport* vp, ImVec2 size)
	{
		DesktopWindow* desktopWindow = static_cast<DesktopWindow*>(vp->PlatformHandle);
		desktopWindow->SetSize(size.x, size.y);
	}

	ImVec2 ImGuiManager::PlatformGetWindowSize(ImGuiViewport* vp)
	{
		DesktopWindow* desktopWindow = static_cast<DesktopWindow*>(vp->PlatformHandle);

		ImVec2 size;
		size.x = desktopWindow->GetWidth();
		size.y = desktopWindow->GetHeight();
		return size;
	}

	ImVec2 ImGuiManager::PlatformGetWindowFramebufferScale(ImGuiViewport* vp)
	{
		(void)vp;
		return ImVec2(1, 1);
	}

	void ImGuiManager::PlatformSetWindowFocus(ImGuiViewport* vp)
	{
		(void)vp;
		/*
		DesktopWindow* desktopWindow = static_cast<DesktopWindow*>(vp->PlatformHandle);
		desktopWindow->Set(size.x, size.y);
		*/
	}

	bool ImGuiManager::PlatformGetWindowFocus(ImGuiViewport* vp)
	{
		DesktopWindow* desktopWindow = static_cast<DesktopWindow*>(vp->PlatformHandle);
		return desktopWindow->IsFocused();
	}

	bool ImGuiManager::PlatformGetWindowMinimized(ImGuiViewport* vp)
	{
		(void)vp;
		return false;
	}

	void ImGuiManager::PlatformSetWindowTitle(ImGuiViewport* vp, const char* str)
	{
		DesktopWindow* desktopWindow = static_cast<DesktopWindow*>(vp->PlatformHandle);
		desktopWindow->SetTitle(str);
	}

	void ImGuiManager::RendererCreateWindow(ImGuiViewport* vp)
	{
		DesktopWindow* desktopWindow = static_cast<DesktopWindow*>(vp->PlatformHandle);
		Renderer::GetInstance()->CreatePresentationSurface(desktopWindow);
	}

	void ImGuiManager::RendererDestroyWindow(ImGuiViewport* vp)
	{
		DesktopWindow* desktopWindow = static_cast<DesktopWindow*>(vp->PlatformHandle);
		(void)desktopWindow;
		//Renderer::GetInstance()->DestroyPresentationSurface(desktopWindow);
	}

	void ImGuiManager::RendererSetWindowSize(ImGuiViewport* vp, ImVec2 size)
	{
		(void)vp;
		(void)size;
		/*
		DesktopWindow* desktopWindow = static_cast<DesktopWindow*>(vp->PlatformHandle);
		Renderer::PresentationSurface* presentationSurface = Renderer::GetInstance()->FindPresentationSurface(desktopWindow);
		presentationSurface->
		*/
	}

	void ImGuiManager::RendererRenderWindow(ImGuiViewport* vp, void* render_arg)
	{
		(void)render_arg;

		DesktopWindow* desktopWindow = static_cast<DesktopWindow*>(vp->PlatformHandle);
		ImGuiManager::GetInstance()->ProcessDrawData(vp->DrawData, desktopWindow);
	}

	void ImGuiManager::RendererSwapBuffers(ImGuiViewport* vp, void* render_arg)
	{
		(void)vp;
		(void)render_arg;
	}

	ImGuiKey KeyToImGuiKeyTable[] = {
		/* Key::None */ ImGuiKey_None,

		// Letters
		/* Key::A */ ImGuiKey_A,
		/* Key::B */ ImGuiKey_B,
		/* Key::C */ ImGuiKey_C,
		/* Key::D */ ImGuiKey_D,
		/* Key::E */ ImGuiKey_E,
		/* Key::F */ ImGuiKey_F,
		/* Key::G */ ImGuiKey_G,
		/* Key::H */ ImGuiKey_H,
		/* Key::I */ ImGuiKey_I,
		/* Key::J */ ImGuiKey_J,
		/* Key::K */ ImGuiKey_K,
		/* Key::L */ ImGuiKey_L,
		/* Key::M */ ImGuiKey_M,
		/* Key::N */ ImGuiKey_N,
		/* Key::O */ ImGuiKey_O,
		/* Key::P */ ImGuiKey_P,
		/* Key::Q */ ImGuiKey_Q,
		/* Key::R */ ImGuiKey_R,
		/* Key::S */ ImGuiKey_S,
		/* Key::T */ ImGuiKey_T,
		/* Key::U */ ImGuiKey_U,
		/* Key::V */ ImGuiKey_V,
		/* Key::W */ ImGuiKey_W,
		/* Key::X */ ImGuiKey_X,
		/* Key::Y */ ImGuiKey_Y,
		/* Key::Z */ ImGuiKey_Z,

		// Digits (top row)
		/* Key::Digit0 */ ImGuiKey_0,
		/* Key::Digit1 */ ImGuiKey_1,
		/* Key::Digit2 */ ImGuiKey_2,
		/* Key::Digit3 */ ImGuiKey_3,
		/* Key::Digit4 */ ImGuiKey_4,
		/* Key::Digit5 */ ImGuiKey_5,
		/* Key::Digit6 */ ImGuiKey_6,
		/* Key::Digit7 */ ImGuiKey_7,
		/* Key::Digit8 */ ImGuiKey_8,
		/* Key::Digit9 */ ImGuiKey_9,

		// Whitespace / editing
		/* Key::Enter */ ImGuiKey_Enter,
		/* Key::Escape */ ImGuiKey_Escape,
		/* Key::Backspace */ ImGuiKey_Backspace,
		/* Key::Tab */ ImGuiKey_Tab,
		/* Key::Space */ ImGuiKey_Space,

		// Punctuation / symbols
		/* Key::Minus */ ImGuiKey_Minus,
		/* Key::Equal */ ImGuiKey_Equal,
		/* Key::LeftBracket */ ImGuiKey_LeftBracket,
		/* Key::RightBracket */ ImGuiKey_RightBracket,
		/* Key::Backslash */ ImGuiKey_Backslash,
		/* Key::Semicolon */ ImGuiKey_Semicolon,
		/* Key::Apostrophe */ ImGuiKey_Apostrophe,
		/* Key::Grave */ ImGuiKey_GraveAccent,
		/* Key::Comma */ ImGuiKey_Comma,
		/* Key::Period */ ImGuiKey_Period,
		/* Key::Slash */ ImGuiKey_Slash,

		// Lock keys
		/* Key::CapsLock */ ImGuiKey_CapsLock,
		/* Key::NumLock */ ImGuiKey_NumLock,
		/* Key::ScrollLock */ ImGuiKey_ScrollLock,

		// Function keys
		/* Key::F1 */ ImGuiKey_F1,
		/* Key::F2 */ ImGuiKey_F2,
		/* Key::F3 */ ImGuiKey_F3,
		/* Key::F4 */ ImGuiKey_F4,
		/* Key::F5 */ ImGuiKey_F5,
		/* Key::F6 */ ImGuiKey_F6,
		/* Key::F7 */ ImGuiKey_F7,
		/* Key::F8 */ ImGuiKey_F8,
		/* Key::F9 */ ImGuiKey_F9,
		/* Key::F10 */ ImGuiKey_F10,
		/* Key::F11 */ ImGuiKey_F11,
		/* Key::F12 */ ImGuiKey_F12,
		/* Key::F13 */ ImGuiKey_None,
		/* Key::F14 */ ImGuiKey_None,
		/* Key::F15 */ ImGuiKey_None,
		/* Key::F16 */ ImGuiKey_None,
		/* Key::F17 */ ImGuiKey_None,
		/* Key::F18 */ ImGuiKey_None,
		/* Key::F19 */ ImGuiKey_None,
		/* Key::F20 */ ImGuiKey_None,

		/* Key::Shift */ ImGuiKey_LeftShift,
		/* Key::Ctrl */ ImGuiKey_LeftCtrl,
		/* Key::Alt */ ImGuiKey_LeftAlt,
		/* Key::Super */ ImGuiKey_LeftSuper,

		// Navigation
		/* Key::Insert */ ImGuiKey_Insert,
		/* Key::Delete */ ImGuiKey_Delete,
		/* Key::Home */ ImGuiKey_Home,
		/* Key::End */ ImGuiKey_End,
		/* Key::PageUp */ ImGuiKey_PageUp,
		/* Key::PageDown */ ImGuiKey_PageDown,

		// Arrows
		/* Key::ArrowLeft */ ImGuiKey_LeftArrow,
		/* Key::ArrowRight */ ImGuiKey_RightArrow,
		/* Key::ArrowUp */ ImGuiKey_UpArrow,
		/* Key::ArrowDown */ ImGuiKey_DownArrow,

		// Numpad
		/* Key::Numpad0 */ ImGuiKey_Keypad0,
		/* Key::Numpad1 */ ImGuiKey_Keypad1,
		/* Key::Numpad2 */ ImGuiKey_Keypad2,
		/* Key::Numpad3 */ ImGuiKey_Keypad3,
		/* Key::Numpad4 */ ImGuiKey_Keypad4,
		/* Key::Numpad5 */ ImGuiKey_Keypad5,
		/* Key::Numpad6 */ ImGuiKey_Keypad6,
		/* Key::Numpad7 */ ImGuiKey_Keypad7,
		/* Key::Numpad8 */ ImGuiKey_Keypad8,
		/* Key::Numpad9 */ ImGuiKey_Keypad9,
		/* Key::NumpadAdd */ ImGuiKey_KeypadAdd,
		/* Key::NumpadSubtract */ ImGuiKey_KeypadSubtract,
		/* Key::NumpadMultiply */ ImGuiKey_KeypadMultiply,
		/* Key::NumpadDivide */ ImGuiKey_KeypadDivide,
		/* Key::NumpadDecimal */ ImGuiKey_KeypadDecimal,
		/* Key::NumpadEnter */ ImGuiKey_KeypadEnter,

		// System / UI
		/* Key::PrintScreen */ ImGuiKey_PrintScreen,
		/* Key::Pause */ ImGuiKey_Pause,
		/* Key::Menu */ ImGuiKey_Menu,
	};

	ImGuiKey ImGuiManager::KeyToImGuiKey(Key key)
	{
		uint8_t index = std::to_underlying(key);
		return (index < std::size(KeyToImGuiKeyTable)) ? KeyToImGuiKeyTable[index] : ImGuiKey_None;
	}

	/// @brief
	_SingletonConstructor(ImGuiManager)
	: _updateJob(this, &ImGuiManager::Update)
	{
	}

	/// @brief
	ImGuiManager::~ImGuiManager()
	{
		DestroyAllWindow();

		// ImGui::DestroyContext(); // todo
		for (Texture* texure : _textures)
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
		style.FrameRounding = 4;
		style.PopupRounding = 4;
		style.ScrollbarRounding = 9;
		style.GrabRounding = 4;
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
		ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_RendererHasTextures | ImGuiBackendFlags_RendererHasViewports | ImGuiBackendFlags_PlatformHasViewports;
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

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
		MacOsWindow* macOsWindow = static_cast<MacOsWindow*>(window);
		icons_configProggyClean.RasterizerDensity = macOsWindow->GetScaleFactor();
#endif
		ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)Roboto_Regular_ttf, Roboto_Regular_ttf_size, 16.0f, &icons_configProggyClean);

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

		ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)MaterialDesignIcons_ttf, MaterialDesignIcons_ttf_size, 16.0f, &icons_configMDI, iconsRangesMDI);

		_mainWindow = window;
#if defined(PLATFORM_MACOS)
		ImGui_ImplOSX_Init(static_cast<MacOsWindow*>(_mainWindow)->GetNsView());
#else

		ImGui::GetPlatformIO().Platform_CreateWindow = &ImGuiManager::PlatformCreateWindow;
		ImGui::GetPlatformIO().Platform_DestroyWindow = &ImGuiManager::PlatformDestroyWindow;
		ImGui::GetPlatformIO().Platform_ShowWindow = &ImGuiManager::PlatformShowWindow;
		ImGui::GetPlatformIO().Platform_SetWindowPos = &ImGuiManager::PlatformSetWindowPos;
		ImGui::GetPlatformIO().Platform_GetWindowPos = &ImGuiManager::PlatformGetWindowPos;
		ImGui::GetPlatformIO().Platform_SetWindowSize = &ImGuiManager::PlatformSetWindowSize;
		ImGui::GetPlatformIO().Platform_GetWindowSize = &ImGuiManager::PlatformGetWindowSize;
		ImGui::GetPlatformIO().Platform_GetWindowFramebufferScale = &ImGuiManager::PlatformGetWindowFramebufferScale;
		ImGui::GetPlatformIO().Platform_SetWindowFocus = &ImGuiManager::PlatformSetWindowFocus;
		ImGui::GetPlatformIO().Platform_GetWindowFocus = &ImGuiManager::PlatformGetWindowFocus;
		ImGui::GetPlatformIO().Platform_GetWindowMinimized = &ImGuiManager::PlatformGetWindowMinimized;
		ImGui::GetPlatformIO().Platform_SetWindowTitle = &ImGuiManager::PlatformSetWindowTitle;

		ImGui::GetPlatformIO().Renderer_CreateWindow = &ImGuiManager::RendererCreateWindow;
		ImGui::GetPlatformIO().Renderer_DestroyWindow = &ImGuiManager::RendererDestroyWindow;
		ImGui::GetPlatformIO().Renderer_SetWindowSize = &ImGuiManager::RendererSetWindowSize;
		ImGui::GetPlatformIO().Renderer_RenderWindow = &ImGuiManager::RendererRenderWindow;
		ImGui::GetPlatformIO().Renderer_SwapBuffers = &ImGuiManager::RendererSwapBuffers;
#endif

		ImGuiViewport* mainViewport = ImGui::GetMainViewport();
		mainViewport->PlatformHandle = _mainWindow;
		mainViewport->PlatformUserData = _mainWindow;
		mainViewport->Pos.x = static_cast<DesktopWindow*>(_mainWindow)->GetPosition().GetX();
		mainViewport->Pos.y = static_cast<DesktopWindow*>(_mainWindow)->GetPosition().GetY();
		mainViewport->Size.x = static_cast<DesktopWindow*>(_mainWindow)->GetWidth();
		mainViewport->Size.y = static_cast<DesktopWindow*>(_mainWindow)->GetHeight();
		mainViewport->DpiScale = 1.0f;

		ImGui::GetPlatformIO().Monitors.resize(1);
		ImGui::GetPlatformIO().Monitors[0].DpiScale = 1.0f;
		ImGui::GetPlatformIO().Monitors[0].MainPos = ImVec2(0.0f, 0.0f);
		ImGui::GetPlatformIO().Monitors[0].MainSize = ImVec2(3440, 1440); // TODO
		ImGui::GetPlatformIO().Monitors[0].WorkPos = ImGui::GetPlatformIO().Monitors[0].MainPos;
		ImGui::GetPlatformIO().Monitors[0].WorkSize = ImGui::GetPlatformIO().Monitors[0].MainSize;

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
		DesktopWindow* window = static_cast<DesktopWindow*>(_mainWindow);
		ImGui_ImplOSX_NewFrame(static_cast<MacOsWindow*>(window)->GetNsView());
		// To avoid having animations (ping, translation, etc.) that lag too much (and to keep them at a reasonable duration) in the editor, even when there is lag.
		io.DeltaTime = std::min(io.DeltaTime, 1.0f / 30.0f);
#else
		static SystemTime::TimeStamp last = SystemTime::INVALID_TIMESTAMP;
		SystemTime::TimeStamp        now = SystemTime::Now();
		io.DeltaTime = SystemTime::ElapsedTimeInSeconds(last, now);
		last = now;

		// To avoid having animations (ping, translation, etc.) that lag too much (and to keep them at a reasonable duration) in the editor, even when there is lag.
		io.DeltaTime = std::min(io.DeltaTime, 1.0f / 30.0f);

		ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
		for (int i = 0; i < platformIO.Viewports.Size; i++)
		{
			ImGuiViewport* viewport = platformIO.Viewports[i];
			ProcessWindowEvent(static_cast<window::Window*>(viewport->PlatformHandle));
		}

		Vector2 resolution;

		PresentationSurface* presentationSurface = Renderer::GetInstance()->FindPresentationSurface(_mainWindow);
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

		static DesktopDisplayManager::BuiltinCursor builtinCursorMapping[ImGuiMouseCursor_COUNT] = {
			DesktopDisplayManager::BuiltinCursor::Arrow,    // ImGuiMouseCursor_Arrow
			DesktopDisplayManager::BuiltinCursor::Ibeam,    // ImGuiMouseCursor_TextInput
			DesktopDisplayManager::BuiltinCursor::SizeNS,   // ImGuiMouseCursor_ResizeAll
			DesktopDisplayManager::BuiltinCursor::SizeNS,   // ImGuiMouseCursor_ResizeNS
			DesktopDisplayManager::BuiltinCursor::SizeWE,   // ImGuiMouseCursor_ResizeEW
			DesktopDisplayManager::BuiltinCursor::SizeNESW, // ImGuiMouseCursor_ResizeNESW
			DesktopDisplayManager::BuiltinCursor::SizeNWSE, // ImGuiMouseCursor_ResizeNWSE
			DesktopDisplayManager::BuiltinCursor::Hand,     // ImGuiMouseCursor_Hand
			DesktopDisplayManager::BuiltinCursor::Wait,     // ImGuiMouseCursor_Wait
			DesktopDisplayManager::BuiltinCursor::Wait,     // ImGuiMouseCursor_Progress
			DesktopDisplayManager::BuiltinCursor::No,       // ImGuiMouseCursor_NotAllowed
		};
		static_cast<DesktopWindow*>(_mainWindow)->SetCursor(DesktopDisplayManager::GetInstance()->GetBultinCursor(builtinCursorMapping[mouseCursor]));
	#endif
#endif

		ImGuiViewport* mainViewport = ImGui::GetMainViewport();
		mainViewport->Pos.x = static_cast<DesktopWindow*>(_mainWindow)->GetPosition().GetX();
		mainViewport->Pos.y = static_cast<DesktopWindow*>(_mainWindow)->GetPosition().GetY();
		mainViewport->Size.x = static_cast<DesktopWindow*>(_mainWindow)->GetWidth();
		mainViewport->Size.y = static_cast<DesktopWindow*>(_mainWindow)->GetHeight();
		mainViewport->DpiScale = 1.0f;

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

		ProcessDrawData(drawData, _mainWindow);

		if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void ImGuiManager::ProcessWindowEvent(window::Window* window)
	{
		ImGuiIO& io = ImGui::GetIO();

		window::Event event;
		uint32_t      eventIndex = 0;
		while (window->PollEvent(eventIndex, event))
		{
			if (event.type == EventType::MouseMoved)
			{
				if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0)
				{
					Vector2 windowPos = static_cast<DesktopWindow*>(window)->GetPosition();
					io.AddMousePosEvent(event.data.mouseMove.x + windowPos.GetX(), event.data.mouseMove.y + windowPos.GetY());
				}
				else
				{
					io.AddMousePosEvent(event.data.mouseMove.x, event.data.mouseMove.y);
				}
			}
			else if (event.type == EventType::MouseButtonDown)
			{
				io.AddMouseButtonEvent(static_cast<ImGuiMouseButton>(event.data.mouseButton.button), true);
			}
			else if (event.type == EventType::MouseButtonUp)
			{
				io.AddMouseButtonEvent(static_cast<ImGuiMouseButton>(event.data.mouseButton.button), false);
			}
			else if (event.type == EventType::MouseScroll)
			{
				io.AddMouseWheelEvent(0.0f, event.data.mouseScroll.value);
			}
			else if (event.type == EventType::MouseHorizontalScroll)
			{
				io.AddMouseWheelEvent(event.data.mouseScroll.value, 0.0f);
			}
			else if (event.type == EventType::KeyPressed)
			{
				ImGuiKey imguiKey = KeyToImGuiKey(event.data.key.key);
				if (imguiKey == ImGuiKey_LeftShift)
				{
					io.AddKeyEvent(ImGuiMod_Shift, true);
				}
				else if (imguiKey == ImGuiKey_LeftCtrl)
				{
					io.AddKeyEvent(ImGuiMod_Ctrl, true);
				}
				else if (imguiKey == ImGuiKey_LeftAlt)
				{
					io.AddKeyEvent(ImGuiMod_Alt, true);
				}
				io.AddKeyEvent(imguiKey, true);
			}
			else if (event.type == EventType::KeyReleased)
			{
				ImGuiKey imguiKey = KeyToImGuiKey(event.data.key.key);
				if (imguiKey == ImGuiKey_LeftShift)
				{
					io.AddKeyEvent(ImGuiMod_Shift, false);
				}
				else if (imguiKey == ImGuiKey_LeftCtrl)
				{
					io.AddKeyEvent(ImGuiMod_Ctrl, false);
				}
				else if (imguiKey == ImGuiKey_LeftAlt)
				{
					io.AddKeyEvent(ImGuiMod_Alt, false);
				}
				io.AddKeyEvent(imguiKey, false);
			}
			else if (event.type == EventType::Char)
			{
				io.AddInputCharacter(event.data.character.c);
			}
			else if (event.type == EventType::FocusGained)
			{
				io.AddFocusEvent(true);
			}
			else if (event.type == EventType::FocusLost)
			{
				io.AddFocusEvent(false);
			}
		}
	}

	void ImGuiManager::ProcessDrawData(ImDrawData* drawData, window::Window* window)
	{
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
						Texture*                 texture = Renderer::GetInstance()->CreateTexture();
						hod::Texture::CreateInfo createInfo;
						createInfo._allowReadWrite = false;
						createInfo._filterMode = FilterMode::Linear;
						createInfo._wrapMode = WrapMode::Clamp;
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
						DefaultAllocator::GetInstance().Delete(textureData->TexID);
						textureData->SetTexID(nullptr);
						textureData->SetStatus(ImTextureStatus_Destroyed);

						Texture*                 texture = Renderer::GetInstance()->CreateTexture();
						hod::Texture::CreateInfo createInfo;
						createInfo._allowReadWrite = false;
						createInfo._filterMode = FilterMode::Linear;
						createInfo._wrapMode = WrapMode::Clamp;
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
						DefaultAllocator::GetInstance().Delete(textureData->TexID);
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

		RenderView* renderView = Renderer::GetInstance()->GetCurrentFrameResources().CreateRenderView();
		renderView->Init();
		if (renderView->Prepare(window) == false)
		{
			// Surface couldn't be acquired this frame (e.g. window just created, hidden or minimized): nothing to render into.
			return;
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
				command._texture = reinterpret_cast<Texture*>(imCommand.GetTexID());
				command._vertexOffset = imCommand.VtxOffset;
				command._indexOffset = imCommand.IdxOffset;
				command._elementCount = imCommand.ElemCount;
			}

			drawLists[drawListIndex] = drawList;
		}

		Rect viewport;
		viewport._position.SetX(0.0f);
		viewport._position.SetY(drawData->OwnerViewport->Size.y);
		viewport._size.SetX(drawData->OwnerViewport->Size.x);
		viewport._size.SetY(-drawData->OwnerViewport->Size.y);

		RenderCommandImGui* renderCommand = DefaultAllocator::GetInstance().New<RenderCommandImGui>(drawLists, viewport);

		renderView->SetupCamera(Matrix4::Identity, Matrix4::Identity, viewport);
		renderView->PushRenderCommand(renderCommand);
	}

	/// @brief
	/// @return
	Material* ImGuiManager::GetMaterial() const
	{
		return _material;
	}

	/// @brief
	/// @return
	bool ImGuiManager::CreateMaterial()
	{
		if (_material == nullptr)
		{
			Renderer* renderer = Renderer::GetInstance();

			VertexInput vertexInput[3] = {
				{0, 0, VertexInput::Format::R32G32_SFloat},
				{0, 8, VertexInput::Format::R32G32_SFloat},
				{0, 16, VertexInput::Format::R8G8B8A8_UNorm},
			};

			_vertexShader = renderer->CreateShader(Shader::ShaderType::Vertex);
			if (_vertexShader->LoadFromIR(ImGui_Vertex, ImGui_Vertex_size, ImGui_Vertex_reflection, ImGui_Vertex_reflection_size) == false)
			{
				DefaultAllocator::GetInstance().Delete(_vertexShader);
				return false;
			}

			_fragmentShader = renderer->CreateShader(Shader::ShaderType::Fragment);
			if (_fragmentShader->LoadFromIR(ImGui_Fragment, ImGui_Fragment_size, ImGui_Fragment_reflection, ImGui_Fragment_reflection_size) == false)
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
