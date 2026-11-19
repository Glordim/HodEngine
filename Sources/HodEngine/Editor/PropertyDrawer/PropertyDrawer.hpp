#pragma once
#include "HodEngine/Editor/Export.hpp"

namespace hod::inline editor
{
	class EditorReflectedProperty;

	template<typename _Type_>
	class PropertyDrawer;

	/// @brief What PropertyDrawer<T> doesn't need its type for: called by ReflectionDrawer with properties whose type is known at runtime only.
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
		/// @brief editorReflectedProperty: values of the type the drawer is registered for (or deriving from it)
		/// @return true if they changed
		virtual bool Draw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue) = 0;

	private:
		template<typename _Type_>
		friend class PropertyDrawer;

		PropertyDrawerBase() = default;
	};

	/// @brief How a value of type _Type_ (or deriving from it) is drawn wherever it is a property or an element of an array (the inspector, the
	/// asset tabs, the project settings...), in place of ReflectionDrawer's default (its properties under a collapsing header). Registered by an
	/// editor module (EditorRegistrar::RegisterPropertyDrawer), one instance for every value of the type
	/// @tparam _Type_ The type drawn: a reflected class or struct (Vector2, Color, WeakResourceBase...)
	template<typename _Type_>
	class PropertyDrawer : public PropertyDrawerBase
	{
	public:
		using PropertyType = _Type_;

	protected:
		PropertyDrawer() = default;

		/// @brief Each frame the property is shown
		/// @param editorReflectedProperty The values edited (several for a multi-selection: GetObjectOf, EditObjects), its label and context menu
		/// with ReflectionDrawer::BeginProperty
		/// @param onlyValue true: the value alone, without its label (an element of an array, a cell of a table...)
		/// @return true if they changed
		virtual bool OnDraw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue) = 0;

	private:
		bool Draw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue) final;
	};

	/// @brief
	/// @tparam _Type_
	/// @param editorReflectedProperty
	/// @param onlyValue
	/// @return
	template<typename _Type_>
	bool PropertyDrawer<_Type_>::Draw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue)
	{
		return OnDraw(editorReflectedProperty, onlyValue);
	}
}
