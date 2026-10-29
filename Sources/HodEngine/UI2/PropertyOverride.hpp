#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <string_view>

namespace hod::inline core
{
	class ReflectionProperty;
}

namespace hod::inline ui2
{
	class Node;

	/// @brief Value of a node property an override can drive: only the types a transition can ease (interpolate).
	struct HOD_UI2_API PropertyValue
	{
		REFLECTED_CLASS_NO_VIRTUAL(PropertyValue)

	public:

		enum class Type : uint8_t
		{
			Float,
			Vector2,
			Color,
		};
		REFLECTED_ENUM(HOD_UI2_API, Type);

	public:

		// The type of the value property holds, when an override can drive it. Top-level reflected properties only.
		static bool			GetType(const ReflectionProperty& property, Type& outType);

		// The properties of node an override can drive, its type's own after its parents', without the hidden ones.
		static Vector<const ReflectionProperty*>	GetOverridableProperties(const Node& node);

		// node's property named propertyName (its own or inherited), when it exists and has an overridable type.
		static bool			Read(const Node& node, std::string_view propertyName, PropertyValue& outValue);
		// Through the property's setter when it has one (the node relayouts/redraws as on any edit). False when node has no
		// such property, or not of this value's type.
		bool				Write(Node& node, std::string_view propertyName) const;

		static PropertyValue	Lerp(const PropertyValue& from, const PropertyValue& to, float t);

		// Same type and same value of that type (the other types' fields are ignored)
		bool				Equals(const PropertyValue& other) const;

	public:

		Type		_type = Type::Float;
		// Only the one of _type is used
		float		_float = 0.0f;
		Vector2		_vector2 = Vector2::Zero;
		Color		_color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	};

	/// @brief Value given to a property of a node of the same tree (prefab) while some state is active, e.g. a Button's
	/// shape filled in another color while hovered. The target is referenced by its local id (see Node::GetLocalId), so it
	/// survives renames and moves.
	struct HOD_UI2_API PropertyOverride
	{
		REFLECTED_CLASS_NO_VIRTUAL(PropertyOverride)

	public:

		uint64_t		_target = 0;
		String			_property;
		PropertyValue	_value;
	};

	/// @brief Plays the switches between sets of PropertyOverride (one per state of an Interaction, see Button): every
	/// property targeted by any of the sets eases, over the transition's duration, from its current value towards the value
	/// the active set gives it, or back to its base value when that set doesn't override it. A property's base value is
	/// read the first time it is targeted, i.e. before any override touched it.
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
		};

		Track*	FindTrack(uint64_t target, const String& property);

	private:

		Vector<Track>	_tracks;
		float			_elapsed = 0.0f;
		float			_duration = 0.0f;
		bool			_running = false;
	};
}
