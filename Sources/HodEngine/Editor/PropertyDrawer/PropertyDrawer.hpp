#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/PropertyDrawer/PropertyValues.hpp"

namespace hod::inline editor
{
	template<typename _Type_>
	class PropertyDrawer;

	/// @brief What PropertyDrawer<T> doesn't need its type for: called by ReflectionDrawer with values whose type is known at runtime only.
	/// Not derived from directly: see PropertyDrawer<T>
	class HOD_EDITOR_API PropertyDrawerBase
	{
	public:
		virtual ~PropertyDrawerBase() = default;

		PropertyDrawerBase(const PropertyDrawerBase&) = delete;
		PropertyDrawerBase(PropertyDrawerBase&&) = delete;

		void operator=(const PropertyDrawerBase&) = delete;
		void operator=(PropertyDrawerBase&&) = delete;

	public:
		/// @brief values: of the type the drawer is registered for (or deriving from it)
		/// @return true if they changed
		virtual bool Draw(const PropertyValuesBase& values) = 0;

	private:
		template<typename _Type_>
		friend class PropertyDrawer;

		PropertyDrawerBase() = default;
	};

	/// @brief How a value of type _Type_ (or deriving from it) is drawn and edited, wherever it is: a property or an element of an array
	/// (ReflectionDrawer::DrawProperty: the inspector, the asset tabs, the project settings...), a value of its own (ReflectionDrawer::DrawValue:
	/// a material's parameter...). Only the value: its label, override marker and context menu are ReflectionDrawer's, around it. Registered by
	/// an editor module (EditorRegistrar::RegisterPropertyDrawer), one instance for every value of the type. What it needs from where it is drawn
	/// (the world of the scene tab, the nodes of the UI prefab...) is set around the drawing by the one knowing it (a scope of the drawer)
	/// @tparam _Type_ The type drawn: a reflected class or struct (Vector2, Color, WeakResourceBase...)
	template<typename _Type_>
	class PropertyDrawer : public PropertyDrawerBase
	{
	public:
		using PropertyType = _Type_;

	protected:
		PropertyDrawer() = default;

		/// @brief Each frame the value is shown, on the rest of its line
		/// @return true if they changed
		virtual bool OnDraw(const PropertyValues<_Type_>& values) = 0;

	private:
		bool Draw(const PropertyValuesBase& values) final;
	};

	/// @brief
	/// @tparam _Type_
	/// @param values
	/// @return
	template<typename _Type_>
	bool PropertyDrawer<_Type_>::Draw(const PropertyValuesBase& values)
	{
		return OnDraw(PropertyValues<_Type_>(values));
	}
}
