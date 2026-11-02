#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <map>
#include <string_view>

namespace hod::inline ui2
{
	class Node;
	class PropertyValue;

	/// @brief Value a track gives its property at a time of the clip. Kept serialized like a PropertyOverride's: its type is the
	/// target's property's, known once resolved (see AnimationPlayer).
	struct HOD_UI2_API AnimationKey
	{
		REFLECTED_CLASS_NO_VIRTUAL(AnimationKey)

	public:

		// How the value goes from this key to the next one: t (0..1 between the two keys) mapped to the blend factor. Constant
		// holds this key's value until the next one. Back and Elastic overshoot (the floats go past the next key's value).
		// Smooth goes through the keys without a break: its curve depends on the keys around the segment (see SmoothWeights), a
		// smoothstep between two keys alone.
		enum class Easing : uint8_t
		{
			Linear,
			Constant,
			QuadIn,
			QuadOut,
			QuadInOut,
			CubicIn,
			CubicOut,
			CubicInOut,
			BackIn,
			BackOut,
			BackInOut,
			BounceOut,
			ElasticOut,
			Smooth,
		};
		REFLECTED_ENUM(HOD_UI2_API, Easing);

	public:

							AnimationKey() = default;
							AnimationKey(const AnimationKey& copy);
							AnimationKey(AnimationKey&& move); // copies: a Document can't move
							~AnimationKey() = default;

		AnimationKey&		operator=(const AnimationKey& copy);
		AnimationKey&		operator=(AnimationKey&& move);

	public:

		static float		Ease(Easing easing, float t);
		// A Smooth segment, at t (0..1) in it: the weights of the values of the key before it, its start key, its end key and the
		// key after it, whose times are given (a cubic Hermite with Catmull-Rom tangents). flatStart/flatEnd: no tangent at that
		// end (no key beyond it, or a value held: the same as a neighbor's), so neither a hold nor an extreme overshoots.
		static void			SmoothWeights(const float times[4], bool flatStart, bool flatEnd, float t, float outWeights[4]);

		// The value, as an instance of target's property type. False when target has no such property (anymore).
		bool				GetValue(const Node& target, std::string_view property, PropertyValue& outValue) const;
		void				SetValue(const PropertyValue& value);

	public:

		float			_time = 0.0f; // In seconds, from the clip's start
		Document		_value;
		Easing			_easing = Easing::Linear;
	};

	/// @brief The keys of one property of one node of the prefab, referenced by its local id (see Node::GetLocalId) and its
	/// property path (see PropertyValue). Keys sorted by time.
	struct HOD_UI2_API AnimationTrack
	{
		REFLECTED_CLASS_NO_VIRTUAL(AnimationTrack)

	public:

		// Adds a key at time, or replaces the value of the one already there (keeping its easing). Returns its index.
		uint32_t		SetKey(float time, const PropertyValue& value);
		// The key at time (within a tolerance), -1 if none
		int32_t			FindKey(float time) const;
		// After keys' times were edited
		void			SortKeys();

	public:

		uint64_t			_target = 0;
		String				_property; // path, see PropertyValue
		Vector<AnimationKey>	_keys;
	};

	/// @brief Named animation of a UI prefab: tracks played together over a duration (see AnimationPlayer). Held by the prefab's
	/// root (see Node::GetAnimations), its tracks targeting nodes of that prefab, so a nested prefab instance has its prefab's.
	struct HOD_UI2_API AnimationClip
	{
		REFLECTED_CLASS_NO_VIRTUAL(AnimationClip)

	public:

		// What happens once the duration is reached: Once stops (holding the last values), Loop starts again, PingPong goes
		// back and forth.
		enum class WrapMode : uint8_t
		{
			Once,
			Loop,
			PingPong,
		};
		REFLECTED_ENUM(HOD_UI2_API, WrapMode);

	public:

		AnimationTrack*			FindTrack(uint64_t target, std::string_view property);
		const AnimationTrack*	FindTrack(uint64_t target, std::string_view property) const;
		// Adds the key to the track of target's property, created if needed
		void					SetKey(uint64_t target, std::string_view property, float time, const PropertyValue& value);

		// Follows the targeted nodes getting new ids (see Node::RenewLocalIds): the ids found in oldToNew are replaced, the others
		// kept if keepOthers, their tracks dropped otherwise.
		void					RemapTargets(const std::map<uint64_t, uint64_t>& oldToNew, bool keepOthers);

	public:

		String					_name;
		float					_duration = 1.0f; // In seconds
		WrapMode				_wrapMode = WrapMode::Once;
		Vector<AnimationTrack>	_tracks;
	};
}
