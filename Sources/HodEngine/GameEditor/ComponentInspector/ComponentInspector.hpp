#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Editor/EditorReflectedObject.hpp>
#include <HodEngine/Game/Component.hpp>

namespace hod::inline editor
{
	template<typename _Component_>
	class ComponentInspector;

	/// @brief What ComponentInspector<T> doesn't need its type for: called by the inspector (InspectorWindow) with components whose type is known
	/// at runtime only. Not derived from directly: see ComponentInspector<T>
	class HOD_GAME_EDITOR_API ComponentInspectorBase
	{
	public:
		virtual ~ComponentInspectorBase() = default;

		ComponentInspectorBase(const ComponentInspectorBase&) = delete;
		ComponentInspectorBase(ComponentInspectorBase&&) = delete;

		void operator=(const ComponentInspectorBase&) = delete;
		void operator=(ComponentInspectorBase&&) = delete;

	public:
		/// @brief reflectedObject: components of the type the inspector is registered for (or deriving from it)
		/// @return true if they changed (a step of the tab's undo history)
		virtual bool Draw(EditorReflectedObject& reflectedObject) = 0;

	protected:
		/// @brief Their properties, as the inspector shows them without a ComponentInspector
		/// @return true if they changed
		static bool DrawProperties(EditorReflectedObject& reflectedObject);

	private:
		template<typename _Component_>
		friend class ComponentInspector;

		ComponentInspectorBase() = default;
	};

	/// @brief What the inspector shows for the components of type _Component_ (or deriving from it): their properties as by default
	/// (DrawProperties) completed (a button, a warning on inconsistent values...), or something else in their place. Registered by an editor
	/// module (GameEditorRegistrar::RegisterComponentInspector), one instance for every component of the type
	/// @tparam _Component_ The component type
	template<typename _Component_>
	class ComponentInspector : public ComponentInspectorBase
	{
	public:
		using ComponentType = _Component_;

	protected:
		ComponentInspector() = default;

		/// @brief Each frame the inspector shows them: their properties by default (DrawProperties). Overridden, their properties are drawn only
		/// if it calls ComponentInspector::OnDraw (or DrawProperties): to keep them and add to them, call it first
		/// @param components The ones edited: several for a multi-selection, never empty
		/// @param reflectedObject The same, for the property drawers (DrawProperties, PropertyDrawer)
		/// @return true if they changed (a step of the tab's undo history)
		virtual bool OnDraw(const Vector<_Component_*>& components, EditorReflectedObject& reflectedObject);

	private:
		bool Draw(EditorReflectedObject& reflectedObject) final;
	};

	/// @brief
	/// @tparam _Component_
	/// @param components
	/// @param reflectedObject
	/// @return
	template<typename _Component_>
	bool ComponentInspector<_Component_>::OnDraw(const Vector<_Component_*>& /*components*/, EditorReflectedObject& reflectedObject)
	{
		return DrawProperties(reflectedObject);
	}

	/// @brief
	/// @tparam _Component_
	/// @param reflectedObject
	/// @return
	template<typename _Component_>
	bool ComponentInspector<_Component_>::Draw(EditorReflectedObject& reflectedObject)
	{
		const Vector<void*>& instances = reflectedObject.GetInstances();
		if (instances.Empty())
		{
			return false;
		}

		Vector<_Component_*> components;
		components.Reserve(instances.Size());
		for (void* instance : instances)
		{
			components.PushBack(static_cast<_Component_*>(static_cast<Component*>(instance)));
		}
		return OnDraw(components, reflectedObject);
	}
}
