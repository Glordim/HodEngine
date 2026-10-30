#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <string_view>

namespace hod::inline core
{
	class ReflectionDescriptor;
	class ReflectionProperty;
	class ReflectionPropertyVariable;
}

namespace hod::inline ui2
{
	class Node;

	/// @brief Value of a node property an override can drive, held as an instance of the property's own type: any reflected
	/// type works without being known here (floats, enums, strings, Color, Vector2, CornerRadii, Margin...), only through
	/// reflection. The property is named by a path: its name for one of the node's own properties, prefixed by "LayoutParams/"
	/// or "Layout/" for one of its LayoutParams' or Layout's (e.g. "LayoutParams/Margin").
	class HOD_UI2_API PropertyValue
	{
	public:

							PropertyValue() = default;
							PropertyValue(const PropertyValue& copy);
							PropertyValue(PropertyValue&& move);
							~PropertyValue();

		PropertyValue&		operator=(const PropertyValue& copy);
		PropertyValue&		operator=(PropertyValue&& move);

	public:

		// The paths of the properties of node an override can drive: its own (its type's after its parents'), then its
		// LayoutParams' and its Layout's. Without the hidden and non serialized ones, nor the arrays (or objects holding some).
		static Vector<String>	GetOverridableProperties(const Node& node);

		// Takes the type and the current value of node's property at propertyPath. False when there is none (or not overridable).
		bool				Read(const Node& node, std::string_view propertyPath);
		// Through the property's setter when it has one (the node relayouts/redraws as on any edit). False when node has no
		// such property, or not of this value's type.
		bool				Write(Node& node, std::string_view propertyPath) const;

		// Takes the type of node's property at propertyPath, and the value serialized in documentNode (see Save).
		bool				Load(const Node& node, std::string_view propertyPath, const DocumentNode& documentNode);
		// As the node's own serialization writes the property
		void				Save(DocumentNode& documentNode) const;

		// Read or Loaded
		bool				IsValid() const;
		bool				IsSameType(const PropertyValue& other) const;
		bool				Equals(const PropertyValue& other) const;

		// Between two values of the same type, into out: the floats (also inside objects, e.g. a color's channels) are eased,
		// the other variables (integers, booleans, enums, strings, ids) switch from from to to halfway. out takes to's type.
		static void			Lerp(const PropertyValue& from, const PropertyValue& to, float t, PropertyValue& out);

	private:

		// Makes the value an instance of property's type (kept if already of it)
		void				SetType(const ReflectionProperty& property);
		void				Clear();

	private:

		const ReflectionProperty*	_property = nullptr; // whose type the value has (a property of a node, LayoutParams or Layout type)
		ReflectionPropertyVariable*	_variable = nullptr; // for a variable: the value seen as that variable at offset 0 of _storage, without setter
		void*						_storage = nullptr; // an instance of the object's type, or the variable's
	};

	/// @brief Value given to a property of a node of the same tree (prefab) while some state is active, e.g. a Button's
	/// shape filled in another color while hovered. The target is referenced by its local id (see Node::GetLocalId), so it
	/// survives renames and moves. The value is kept serialized: its type is the target's property's, known once resolved.
	struct HOD_UI2_API PropertyOverride
	{
		REFLECTED_CLASS_NO_VIRTUAL(PropertyOverride)

	public:

							PropertyOverride() = default;
							PropertyOverride(const PropertyOverride& copy);
							PropertyOverride(PropertyOverride&& move); // copies: a Document can't move
							~PropertyOverride() = default;

		PropertyOverride&	operator=(const PropertyOverride& copy);
		PropertyOverride&	operator=(PropertyOverride&& move);

	public:

		// The value, as an instance of target's property type. False when target has no such property (anymore).
		bool				GetValue(const Node& target, PropertyValue& outValue) const;
		void				SetValue(const PropertyValue& value);

	public:

		uint64_t		_target = 0;
		String			_property; // path, see PropertyValue
		Document		_value;
	};

	/// @brief Plays the switches between sets of PropertyOverride (one per state of an Interaction, see Button): every
	/// property targeted by any of the sets eases, over the transition's duration, from its current value towards the value
	/// the active set gives it, or back to its base value when that set doesn't override it. A property's base value is
	/// read the first time it is targeted, i.e. before any override touched it. The values are resolved when a transition
	/// starts: each frame only eases them and writes them.
	class HOD_UI2_API PropertyOverrideTransition
	{
	public:

		// active: the overrides of the new state, null for none (every property back to its base value). allSets: the
		// overrides of every state, active included. duration <= 0 applies the values on the next Update.
		void	Start(Node& owner, const Vector<PropertyOverride>* active, const Vector<const Vector<PropertyOverride>*>& allSets, float duration);
		void	Update(Node& owner, float deltaTime);

		// Forgets the base values: re-read on the next Start (e.g. after the overrides were edited).
		void	Reset();

	private:

		struct Track
		{
			uint64_t		_target = 0;
			String			_property;
			PropertyValue	_base;
			PropertyValue	_from;
			PropertyValue	_to;
			PropertyValue	_current;
		};

		Track*	FindTrack(uint64_t target, const String& property);

	private:

		Vector<Track>	_tracks;
		float			_elapsed = 0.0f;
		float			_duration = 0.0f;
		bool			_running = false;
	};
}
