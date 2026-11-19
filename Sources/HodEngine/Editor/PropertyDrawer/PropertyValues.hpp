#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/Vector.hpp>

#include <cstdint>
#include <type_traits>

namespace hod::inline editor
{
	class EditorReflectedProperty;

	/// @brief The values a property drawer edits, whatever their type (see PropertyValues<T>): one value (ReflectionDrawer::DrawValue), or a
	/// property's value in each of the instances edited together (a multi-selection, ReflectionDrawer::DrawProperty)
	class HOD_EDITOR_API PropertyValuesBase
	{
	public:
		using EditFunction = void (*)(void* value, void* userData);

	public:
		/// @brief One value, edited in place
		explicit PropertyValuesBase(void* value);
		/// @brief The property's value in each of its instances, edited through the property (its set method, if it has one)
		explicit PropertyValuesBase(EditorReflectedProperty& property);

		PropertyValuesBase(const PropertyValuesBase&) = delete;
		PropertyValuesBase(PropertyValuesBase&&) = delete;

		void operator=(const PropertyValuesBase&) = delete;
		void operator=(PropertyValuesBase&&) = delete;

	public:
		uint32_t GetCount() const;
		void*    GetValue(uint32_t index) const;
		/// @return Whether they don't all have the same value (shown as a mixed value)
		bool     IsMixed() const;

		/// @brief edit called on each value: on a copy given back to the property's set method when it has one (its owner reacts as to any
		/// edit), in place otherwise
		void Edit(EditFunction edit, void* userData) const;

	private:
		Vector<void*>            _values;
		bool                     _mixed = false;
		EditorReflectedProperty* _property = nullptr;
	};

	/// @brief The values of type _Type_ a PropertyDrawer<_Type_> edits: read the first one (Get), whether they differ (IsMixed), edit them all
	/// (Set, Edit). Nothing else: no label, no prefab, no override (ReflectionDrawer's business)
	template<typename _Type_>
	class PropertyValues
	{
	public:
		explicit PropertyValues(const PropertyValuesBase& values);

	public:
		uint32_t       GetCount() const;
		/// @return The value at index (the first one by default: the one shown)
		const _Type_&  Get(uint32_t index = 0) const;

		/// @return Whether they don't all have the same value
		bool IsMixed() const;
		/// @return Whether they don't all have the same part read by get (a component of a vector: get(const _Type_&) returns it)
		template<typename _Getter_>
		bool IsMixed(_Getter_&& get) const;

		/// @brief Each of them given value
		void Set(const _Type_& value) const;
		/// @brief edit(_Type_&) called on each of them: to change a part only (a component of a vector), keeping each one's other parts
		template<typename _Function_>
		void Edit(_Function_&& edit) const;

	private:
		const PropertyValuesBase& _values;
	};

	/// @brief
	/// @tparam _Type_
	/// @param values
	template<typename _Type_>
	PropertyValues<_Type_>::PropertyValues(const PropertyValuesBase& values)
	: _values(values)
	{
	}

	/// @brief
	/// @tparam _Type_
	/// @return
	template<typename _Type_>
	uint32_t PropertyValues<_Type_>::GetCount() const
	{
		return _values.GetCount();
	}

	/// @brief
	/// @tparam _Type_
	/// @param index
	/// @return
	template<typename _Type_>
	const _Type_& PropertyValues<_Type_>::Get(uint32_t index) const
	{
		return *static_cast<const _Type_*>(_values.GetValue(index));
	}

	/// @brief
	/// @tparam _Type_
	/// @return
	template<typename _Type_>
	bool PropertyValues<_Type_>::IsMixed() const
	{
		return _values.IsMixed();
	}

	/// @brief
	/// @tparam _Type_
	/// @tparam _Getter_
	/// @param get
	/// @return
	template<typename _Type_>
	template<typename _Getter_>
	bool PropertyValues<_Type_>::IsMixed(_Getter_&& get) const
	{
		for (uint32_t index = 1; index < GetCount(); ++index)
		{
			if ((get(Get(index)) == get(Get(0))) == false)
			{
				return true;
			}
		}
		return false;
	}

	/// @brief
	/// @tparam _Type_
	/// @param value
	template<typename _Type_>
	void PropertyValues<_Type_>::Set(const _Type_& value) const
	{
		Edit([&value](_Type_& edited) { edited = value; });
	}

	/// @brief
	/// @tparam _Type_
	/// @tparam _Function_
	/// @param edit
	template<typename _Type_>
	template<typename _Function_>
	void PropertyValues<_Type_>::Edit(_Function_&& edit) const
	{
		using Function = std::remove_reference_t<_Function_>;
		_values.Edit([](void* value, void* userData) { (*static_cast<Function*>(userData))(*static_cast<_Type_*>(value)); }, (void*)&edit);
	}
}
