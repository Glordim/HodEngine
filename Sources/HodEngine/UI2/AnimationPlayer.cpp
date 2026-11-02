#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/AnimationPlayer.hpp"
#include "HodEngine/UI2/Node.hpp"

#include <HodEngine/Core/Math.hpp>

#include <cmath>

namespace hod::inline ui2
{
	namespace
	{
		bool Equals(const String& left, std::string_view right)
		{
			return std::string_view(left.CStr(), left.Length()) == right;
		}
	}

	/// @brief
	/// @param owner
	AnimationPlayer::AnimationPlayer(Node& owner)
	: _owner(&owner)
	{
	}

	/// @brief
	/// @param name
	/// @return
	const AnimationClip* AnimationPlayer::FindClip(std::string_view name) const
	{
		for (const AnimationClip& clip : _owner->GetAnimations())
		{
			if (Equals(clip._name, name))
			{
				return &clip;
			}
		}
		return nullptr;
	}

	/// @brief The node of the owner's prefab having that local id, null if none.
	/// @param target
	/// @return
	Node* AnimationPlayer::FindTarget(uint64_t target) const
	{
		return _owner->GetScopeRoot()->FindByLocalId(target);
	}

	/// @brief
	/// @param name
	/// @return
	AnimationPlayer::Playback* AnimationPlayer::FindPlayback(std::string_view name)
	{
		for (Playback& playback : _playbacks)
		{
			if (Equals(playback._name, name))
			{
				return &playback;
			}
		}
		return nullptr;
	}

	/// @brief
	/// @param clip
	/// @param playback
	void AnimationPlayer::Resolve(const AnimationClip& clip, Playback& playback)
	{
		playback._name = clip._name;
		playback._duration = Max(clip._duration, 0.0f);
		playback._wrapMode = clip._wrapMode;
		playback._tracks.Clear();

		for (const AnimationTrack& track : clip._tracks)
		{
			Node* target = FindTarget(track._target);
			if (target == nullptr || track._keys.Empty())
			{
				continue;
			}

			ResolvedTrack resolved;
			resolved._target = track._target;
			resolved._property = track._property;
			for (const AnimationKey& key : track._keys)
			{
				PropertyValue value;
				if (key.GetValue(*target, track._property, value) == false || (resolved._values.Empty() == false && value.IsSameType(resolved._values[0]) == false))
				{
					continue;
				}
				resolved._times.PushBack(key._time);
				resolved._easings.PushBack(key._easing);
				resolved._values.PushBack(std::move(value));
			}
			if (resolved._values.Empty())
			{
				continue;
			}

			bool known = false;
			for (const BaseValue& baseValue : _baseValues)
			{
				if (baseValue._target == track._target && baseValue._property == track._property)
				{
					known = true;
					break;
				}
			}
			if (known == false)
			{
				BaseValue baseValue;
				if (baseValue._value.Read(*target, track._property))
				{
					baseValue._target = track._target;
					baseValue._property = track._property;
					_baseValues.PushBack(std::move(baseValue));
				}
			}

			playback._tracks.PushBack(std::move(resolved));
		}
	}

	/// @brief The time within the clip's duration its unwrapped time gives, by its wrap mode.
	/// @param playback
	/// @return
	float AnimationPlayer::GetLocalTime(const Playback& playback) const
	{
		if (playback._duration <= 0.0f)
		{
			return 0.0f;
		}

		switch (playback._wrapMode)
		{
			case AnimationClip::WrapMode::Once: return Clamp(playback._time, 0.0f, playback._duration);

			case AnimationClip::WrapMode::Loop:
			{
				float time = std::fmod(playback._time, playback._duration);
				return time < 0.0f ? time + playback._duration : time;
			}

			case AnimationClip::WrapMode::PingPong:
			{
				float period = playback._duration * 2.0f;
				float time = std::fmod(playback._time, period);
				if (time < 0.0f)
				{
					time += period;
				}
				return time <= playback._duration ? time : period - time;
			}
		}
		return 0.0f;
	}

	/// @brief
	/// @param playback
	void AnimationPlayer::Apply(Playback& playback)
	{
		float time = GetLocalTime(playback);

		for (ResolvedTrack& track : playback._tracks)
		{
			Node* target = FindTarget(track._target);
			if (target == nullptr)
			{
				continue;
			}

			// The keys around time: before the first one or after the last one, that key's value is held
			uint32_t next = 0;
			while (next < track._times.Size() && track._times[next] <= time)
			{
				++next;
			}

			if (next == 0)
			{
				track._current = track._values[0];
			}
			else if (next == track._times.Size())
			{
				track._current = track._values[next - 1];
			}
			else
			{
				uint32_t previous = next - 1;
				float    span = track._times[next] - track._times[previous];
				float    t = span > 0.0f ? (time - track._times[previous]) / span : 1.0f;
				if (track._easings[previous] == AnimationKey::Easing::Smooth)
				{
					// Through the keys around the segment (themselves at the clip's ends)
					uint32_t before = (previous > 0) ? previous - 1 : previous;
					uint32_t after = (next + 1 < track._times.Size()) ? next + 1 : next;
					bool     flatStart = before == previous || track._values[previous].Equals(track._values[before]) || track._values[previous].Equals(track._values[next]);
					bool     flatEnd = after == next || track._values[next].Equals(track._values[after]) || track._values[next].Equals(track._values[previous]);

					float times[4] = {track._times[before], track._times[previous], track._times[next], track._times[after]};
					float weights[4];
					AnimationKey::SmoothWeights(times, flatStart, flatEnd, t, weights);
					const PropertyValue* values[4] = {&track._values[before], &track._values[previous], &track._values[next], &track._values[after]};
					PropertyValue::Blend(values, weights, t, track._current);
				}
				else
				{
					PropertyValue::Lerp(track._values[previous], track._values[next], AnimationKey::Ease(track._easings[previous], t), track._current);
				}
			}
			track._current.Write(*target, track._property);
		}
	}

	/// @brief
	/// @param name
	/// @param speed
	/// @return
	bool AnimationPlayer::Play(std::string_view name, float speed)
	{
		const AnimationClip* clip = FindClip(name);
		if (clip == nullptr)
		{
			return false;
		}

		Playback* playback = FindPlayback(name);
		if (playback == nullptr)
		{
			playback = &_playbacks.EmplaceBack();
		}
		Resolve(*clip, *playback);
		playback->_speed = speed;
		playback->_time = speed < 0.0f ? playback->_duration : 0.0f;
		Apply(*playback);
		return true;
	}

	/// @brief
	/// @param name
	/// @param restore
	void AnimationPlayer::Stop(std::string_view name, bool restore)
	{
		for (uint32_t index = 0; index < _playbacks.Size(); ++index)
		{
			if (Equals(_playbacks[index]._name, name))
			{
				_playbacks.Erase(index);
				break;
			}
		}

		const AnimationClip* clip = FindClip(name);
		if (restore == false || clip == nullptr)
		{
			return;
		}

		// Given back, the base values are forgotten: a property changed meanwhile is read again by the next clip touching it
		for (const AnimationTrack& track : clip->_tracks)
		{
			for (uint32_t index = 0; index < _baseValues.Size(); ++index)
			{
				BaseValue& baseValue = _baseValues[index];
				if (baseValue._target == track._target && baseValue._property == track._property)
				{
					if (Node* target = FindTarget(baseValue._target))
					{
						baseValue._value.Write(*target, baseValue._property);
					}
					_baseValues.Erase(index);
					break;
				}
			}
		}
	}

	/// @brief
	/// @param restore
	void AnimationPlayer::StopAll(bool restore)
	{
		_playbacks.Clear();
		if (restore)
		{
			for (const BaseValue& baseValue : _baseValues)
			{
				if (Node* target = FindTarget(baseValue._target))
				{
					baseValue._value.Write(*target, baseValue._property);
				}
			}
			_baseValues.Clear();
		}
	}

	/// @brief
	/// @param name
	/// @return
	bool AnimationPlayer::IsPlaying(std::string_view name) const
	{
		return const_cast<AnimationPlayer*>(this)->FindPlayback(name) != nullptr;
	}

	/// @brief
	/// @param name
	/// @param time
	/// @return
	bool AnimationPlayer::Sample(std::string_view name, float time)
	{
		const AnimationClip* clip = FindClip(name);
		if (clip == nullptr)
		{
			return false;
		}

		Playback playback;
		Resolve(*clip, playback);
		playback._time = time;
		Apply(playback);
		return true;
	}

	/// @brief
	/// @param deltaTime
	void AnimationPlayer::Update(float deltaTime)
	{
		Vector<String> finished;

		for (uint32_t index = 0; index < _playbacks.Size();)
		{
			Playback& playback = _playbacks[index];
			playback._time += deltaTime * playback._speed;

			bool ended = false;
			if (playback._wrapMode == AnimationClip::WrapMode::Once)
			{
				ended = playback._speed >= 0.0f ? playback._time >= playback._duration : playback._time <= 0.0f;
			}

			Apply(playback);

			if (ended)
			{
				finished.PushBack(playback._name);
				_playbacks.Erase(index);
				continue;
			}
			++index;
		}

		// Once the playbacks are settled: a listener may play another clip
		for (const String& name : finished)
		{
			_finishedEvent.Emit(name);
		}
	}

	/// @brief
	/// @return
	AnimationPlayer::FinishedEvent& AnimationPlayer::GetFinishedEvent()
	{
		return _finishedEvent;
	}
}
