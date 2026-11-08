#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/ViewportGamePointer.hpp"

#include <algorithm>

namespace hod::inline editor
{
	namespace
	{
		Vector<ViewportGamePointer::Listener>& GamePointerListeners()
		{
			static Vector<ViewportGamePointer::Listener> listeners;
			return listeners;
		}
	}

	/// @brief
	/// @param listener
	/// @return
	bool ViewportGamePointer::Register(Listener listener)
	{
		if (std::find(GamePointerListeners().Begin(), GamePointerListeners().End(), listener) != GamePointerListeners().End())
		{
			OUTPUT_ERROR("ViewportGamePointer: listener registered already");
			return false;
		}
		GamePointerListeners().PushBack(listener);
		return true;
	}

	/// @brief
	/// @param listener
	void ViewportGamePointer::Unregister(Listener listener)
	{
		auto it = std::find(GamePointerListeners().Begin(), GamePointerListeners().End(), listener);
		if (it != GamePointerListeners().End())
		{
			GamePointerListeners().Erase(it);
		}
	}

	/// @brief
	/// @param position
	/// @param resolution
	/// @param pressed
	/// @param scroll
	void ViewportGamePointer::Dispatch(const Vector2& position, const Vector2& resolution, bool pressed, const Vector2& scroll)
	{
		for (Listener listener : GamePointerListeners())
		{
			listener(position, resolution, pressed, scroll);
		}
	}
}
