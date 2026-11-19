#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <string_view>

namespace hod::inline editor
{
	/// @brief The properties shown highlighted: the ones deciding why a disabled property (or component) hovered is (see
	/// ReflectionTraitEnabledIf), of its owner. A request lasts its frame and the next one: the property deciding may be drawn before the one
	/// hovered. ReflectionDrawer keeps the editor's one (RequestHighlight, IsHighlighted)
	class HOD_EDITOR_API PropertyHighlight
	{
	public:
		/// @brief A new frame: the requests of the previous one still shown, then forgotten
		void NewFrame();

		/// @param owner The object having the property (the first of the ones edited together)
		/// @param property Its name (as written in its class, see HOD_PROPERTY)
		void Request(const void* owner, std::string_view property);
		/// @return Whether the property of owner was requested in this frame or the previous one
		bool IsHighlighted(const void* owner, std::string_view property) const;

	private:
		struct Entry
		{
			const void* _owner = nullptr;
			String      _property;
		};

		static bool Contains(const Vector<Entry>& entries, const void* owner, std::string_view property);

	private:
		Vector<Entry> _requested;
		Vector<Entry> _previous;
	};
}
