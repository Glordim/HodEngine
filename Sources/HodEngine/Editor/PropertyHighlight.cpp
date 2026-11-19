#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/PropertyHighlight.hpp"

namespace hod::inline editor
{
	/// @brief
	void PropertyHighlight::NewFrame()
	{
		_previous = std::move(_requested);
		_requested.Clear();
	}

	/// @brief
	/// @param owner
	/// @param property
	void PropertyHighlight::Request(const void* owner, std::string_view property)
	{
		if (owner == nullptr || property.empty() || Contains(_requested, owner, property))
		{
			return;
		}
		_requested.PushBack(Entry {owner, String(property)});
	}

	/// @brief
	/// @param owner
	/// @param property
	/// @return
	bool PropertyHighlight::IsHighlighted(const void* owner, std::string_view property) const
	{
		return Contains(_requested, owner, property) || Contains(_previous, owner, property);
	}

	/// @brief
	/// @param entries
	/// @param owner
	/// @param property
	/// @return
	bool PropertyHighlight::Contains(const Vector<Entry>& entries, const void* owner, std::string_view property)
	{
		for (const Entry& entry : entries)
		{
			if (entry._owner == owner && std::string_view(entry._property) == property)
			{
				return true;
			}
		}
		return false;
	}
}
