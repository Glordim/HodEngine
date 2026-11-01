#pragma once
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/AnimationClip.hpp"
#include "HodEngine/UI2/PropertyOverride.hpp"

#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <string_view>

namespace hod::inline ui2
{
	class Node;

	/// @brief Plays the animation clips of a node (see Node::GetAnimations, Node::GetAnimationPlayer), updated by Canvas::Update
	/// after the Interactions: a clip drives its properties over a state transition touching the same ones. Several clips can
	/// play at once, the last one started winning on a property they share.
	///
	/// The clip is resolved when it starts (its targets found, its keys' values typed): each frame only eases between them and
	/// writes them, so editing the clip doesn't change a playing one. A property's base value (what the prefab gives it) is
	/// read the first time a clip touches it, for Stop to put it back.
	class HOD_UI2_API AnimationPlayer
	{
	public:

		using FinishedEvent = Event<const String&>;

	public:

							AnimationPlayer(Node& owner);
							AnimationPlayer(const AnimationPlayer&) = delete;
							AnimationPlayer(AnimationPlayer&&) = delete;
							~AnimationPlayer() = default;

		AnimationPlayer&	operator=(const AnimationPlayer&) = delete;
		AnimationPlayer&	operator=(AnimationPlayer&&) = delete;

	public:

		// Starts the owner's clip of that name from its start, or from its end when speed is negative (e.g. a "Close" being "Open"
		// played backwards); restarted if already playing. Its first values are written right away. False if there is no such clip.
		bool				Play(std::string_view name, float speed = 1.0f);
		// Stops the clip if it is playing and, if restore, puts back the base values of the properties it drives (also once it
		// finished, holding its last values).
		void				Stop(std::string_view name, bool restore = true);
		void				StopAll(bool restore = true);
		bool				IsPlaying(std::string_view name) const;

		// Writes the values the clip gives at time, without playing it (the editor's timeline). False if there is no such clip.
		bool				Sample(std::string_view name, float time);

		void				Update(float deltaTime);

		// Emitted when a Once clip reaches its end (its start when played backwards), with its name. Not on Stop.
		FinishedEvent&		GetFinishedEvent();

	private:

		struct ResolvedTrack
		{
			uint64_t						_target = 0;
			String							_property;
			Vector<float>					_times;
			Vector<AnimationKey::Easing>	_easings;
			Vector<PropertyValue>			_values;
			PropertyValue					_current;
		};

		struct Playback
		{
			String					_name;
			float					_duration = 0.0f;
			AnimationClip::WrapMode	_wrapMode = AnimationClip::WrapMode::Once;
			float					_speed = 1.0f;
			float					_time = 0.0f; // Unwrapped
			Vector<ResolvedTrack>	_tracks;
		};

		struct BaseValue
		{
			uint64_t		_target = 0;
			String			_property;
			PropertyValue	_value;
		};

	private:

		const AnimationClip*	FindClip(std::string_view name) const;
		Node*					FindTarget(uint64_t target) const;
		// The clip's tracks whose target and keys are found, reading the base value of the properties not touched yet
		void					Resolve(const AnimationClip& clip, Playback& playback);
		// Writes playback's values at its current time
		void					Apply(Playback& playback);
		float					GetLocalTime(const Playback& playback) const;
		Playback*				FindPlayback(std::string_view name);

	private:

		friend class Node;
		Node*					_owner = nullptr; // non-owning, the node owning the player
		Vector<Playback>		_playbacks;
		Vector<BaseValue>		_baseValues;
		FinishedEvent			_finishedEvent;
	};
}
