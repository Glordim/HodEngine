#pragma once
#include "HodEngine/ImGui/Export.hpp"

#include <HodEngine/Core/Event.hpp>
#include <HodEngine/GameSystems/Job/MemberFunctionJob.hpp>
#include <HodEngine/Core/Singleton.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Window/Key.hpp>

#include <stdint.h>

#if defined(PLATFORM_WINDOWS)
	#undef max
	#undef FindWindow
#endif

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline window
{
	class Window;
}

namespace hod::inline renderer
{
	class Shader;
	class Material;
	class Texture;
}

namespace hod::inline imgui
{
	class MainBar;
	class Window;
	class WindowDescription;

	/// @brief
	class HOD_IMGUI_API ImGuiManager
	{
		_Singleton(ImGuiManager)

	public:
		bool Init(window::Window* window);
		void Update();

		window::Window* GetMainWindow() const;

		void SetMainBar(MainBar* mainBar);

		template<typename Window_, typename... Args>
		Window_* OpenWindow(Args&&... args);
		void     OpenWindow(Window* window);

		template<typename Window_>
		Window_* FindWindow() const;
		Window*  FindWindow(ReflectionDescriptor& windowDescription) const;

		template<typename Window_>
		Vector<Window*> FindWindows() const;
		Vector<Window*> FindWindows(ReflectionDescriptor& windowDescription) const;

		void CloseAllWindow();
		void DestroyAllWindow();

		Material* GetMaterial() const;

		ImGuiID GetCentralDockSpace() const;

		void SetDrawCallback(const std::function<void()>& drawCallback)
		{
			_callback = drawCallback;
		}

	protected:
		~ImGuiManager();

	private:
		bool CreateMaterial();

		static ImGuiKey KeyToImGuiKey(Key key);

		void ProcessDrawData(ImDrawData* drawData, window::Window* window);
		void ProcessWindowEvent(window::Window* window);

		static void PlatformCreateWindow(ImGuiViewport* vp);
		static void PlatformDestroyWindow(ImGuiViewport* vp);
		static void PlatformShowWindow(ImGuiViewport* vp);
		static void PlatformSetWindowPos(ImGuiViewport* vp, ImVec2 pos);
		static ImVec2 PlatformGetWindowPos(ImGuiViewport* vp);
		static void PlatformSetWindowSize(ImGuiViewport* vp, ImVec2 size);
		static ImVec2 PlatformGetWindowSize(ImGuiViewport* vp);
		static ImVec2 PlatformGetWindowFramebufferScale(ImGuiViewport* vp);
		static void PlatformSetWindowFocus(ImGuiViewport* vp);
		static bool PlatformGetWindowFocus(ImGuiViewport* vp);
		static bool PlatformGetWindowMinimized(ImGuiViewport* vp);
		static void PlatformSetWindowTitle(ImGuiViewport* vp, const char* str);

		static void RendererCreateWindow(ImGuiViewport* vp);
		static void RendererDestroyWindow(ImGuiViewport* vp);
		static void RendererSetWindowSize(ImGuiViewport* vp, ImVec2 size);
		static void RendererRenderWindow(ImGuiViewport* vp, void* render_arg);
		static void RendererSwapBuffers(ImGuiViewport* vp, void* render_arg);

	private:
		MemberFunctionJob<ImGuiManager> _updateJob;

		MainBar*        _mainBar = nullptr;
		Vector<Window*> _windows;

		Vector<Texture*> _textures;

		Material* _material = nullptr;
		Shader*   _vertexShader = nullptr;
		Shader*   _fragmentShader = nullptr;

		window::Window* _mainWindow = nullptr;

		ImGuiID _centralDockSpace;

		std::function<void()> _callback;
	};
}

#include "HodEngine/ImGui/ImGuiManager.inl"
