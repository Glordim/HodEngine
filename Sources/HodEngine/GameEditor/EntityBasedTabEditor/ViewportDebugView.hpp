#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include <HodEngine/Core/Vector.hpp>

namespace hod::inline renderer
{
	class RenderView;
}

namespace hod::inline game
{
	class World;
}

namespace hod::inline editor
{
	/// @brief Draws a world its own way in a viewport, in place of its render (the physics shapes...). One instance per viewport where it is
	/// enabled (see ViewportDebugViewRegistry), one debug view enabled at a time
	class HOD_GAME_EDITOR_API ViewportDebugView
	{
	public:
		ViewportDebugView() = default;
		ViewportDebugView(const ViewportDebugView&) = delete;
		ViewportDebugView(ViewportDebugView&&) = delete;
		virtual ~ViewportDebugView() = default;

		ViewportDebugView& operator=(const ViewportDebugView&) = delete;
		ViewportDebugView& operator=(ViewportDebugView&&) = delete;

	public:
		virtual void Draw(RenderView& renderView, game::World& world) = 0;
	};

	/// @brief A kind of ViewportDebugView, registered by an editor module (a toggle in the viewports' toolbar). Identified by its address:
	/// define it once, in the module's code (a static of a function)
	struct HOD_GAME_EDITOR_API ViewportDebugViewType
	{
		const char* _label = nullptr;    // Of the toggle ("<icon> Physics")
		const char* _itemName = nullptr; // What it shows, for the tooltips ("physics debug")
		ViewportDebugView* (*_create)() = nullptr;

		/// @brief Optional: the content of its settings popup (an arrow next to the toggle), enabled or not
		void (*_drawSettings)(game::World& world) = nullptr;
	};

	/// @brief The registered ViewportDebugViewType, and their instances in the viewports. The instances are owned here: unregistering a type
	/// (its module unloading) destroys them, the viewports don't keep any pointer on them
	class HOD_GAME_EDITOR_API ViewportDebugViewRegistry
	{
	public:
		/// @return false (with an error) if registered already
		static bool Register(const ViewportDebugViewType& type);
		static void Unregister(const ViewportDebugViewType& type);

		static const Vector<const ViewportDebugViewType*>& GetTypes();

		/// @return The type enabled in this viewport, nullptr if none
		static const ViewportDebugViewType* GetEnabledType(const void* viewport);

		/// @return The instance enabled in this viewport, nullptr if none
		static ViewportDebugView* GetEnabled(const void* viewport);

		/// @param type nullptr: none. The previous one is destroyed
		static void SetEnabled(const void* viewport, const ViewportDebugViewType* type);
	};
}
