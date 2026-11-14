#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/AnimationClip.hpp"
#include "HodEngine/UI/PropertyOverride.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

#include <algorithm>
#include <cmath>
#include <numbers>

namespace hod::inline ui
{
	DESCRIBE_REFLECTED_ENUM(AnimationKey::Easing)
	{
		HOD_ENUM_VALUE(Linear);
		HOD_ENUM_VALUE(Constant);
		HOD_ENUM_VALUE(QuadIn);
		HOD_ENUM_VALUE(QuadOut);
		HOD_ENUM_VALUE(QuadInOut);
		HOD_ENUM_VALUE(CubicIn);
		HOD_ENUM_VALUE(CubicOut);
		HOD_ENUM_VALUE(CubicInOut);
		HOD_ENUM_VALUE(BackIn);
		HOD_ENUM_VALUE(BackOut);
		HOD_ENUM_VALUE(BackInOut);
		HOD_ENUM_VALUE(BounceOut);
		HOD_ENUM_VALUE(ElasticOut);
		HOD_ENUM_VALUE(Smooth);
	}

	DESCRIBE_REFLECTED_CLASS(AnimationKey)
	{
		HOD_PROPERTY(_time);
		HOD_PROPERTY(_value);
		HOD_PROPERTY(_easing);
	}

	DESCRIBE_REFLECTED_CLASS(AnimationTrack)
	{
		HOD_PROPERTY(_target);
		HOD_PROPERTY(_property);
		HOD_PROPERTY(_keys);
	}

	DESCRIBE_REFLECTED_ENUM(AnimationClip::WrapMode)
	{
		HOD_ENUM_VALUE(Once);
		HOD_ENUM_VALUE(Loop);
		HOD_ENUM_VALUE(PingPong);
	}

	DESCRIBE_REFLECTED_CLASS(AnimationClip)
	{
		HOD_PROPERTY(_name);
		HOD_PROPERTY(_duration);
		HOD_PROPERTY(_wrapMode);
		HOD_PROPERTY(_tracks);
	}

	namespace
	{
		constexpr float KeyTimeTolerance = 0.0001f; // In seconds
	}

	/// @brief
	/// @param copy
	AnimationKey::AnimationKey(const AnimationKey& copy)
	{
		*this = copy;
	}

	/// @brief
	/// @param move
	AnimationKey::AnimationKey(AnimationKey&& move)
	{
		*this = move;
	}

	/// @brief
	/// @param copy
	/// @return
	AnimationKey& AnimationKey::operator=(const AnimationKey& copy)
	{
		if (this != &copy)
		{
			_time = copy._time;
			_value = copy._value;
			_easing = copy._easing;
		}
		return *this;
	}

	/// @brief
	/// @param move
	/// @return
	AnimationKey& AnimationKey::operator=(AnimationKey&& move)
	{
		if (this != &move)
		{
			_time = move._time;
			_value = move._value;
			_easing = move._easing;
		}
		return *this;
	}

	/// @brief The usual easing functions (easings.net)
	/// @param easing
	/// @param t
	/// @return
	float AnimationKey::Ease(Easing easing, float t)
	{
		constexpr float c1 = 1.70158f;
		constexpr float c2 = c1 * 1.525f;
		constexpr float c3 = c1 + 1.0f;

		switch (easing)
		{
			case Easing::Linear: return t;
			case Easing::Constant: return 0.0f;

			case Easing::QuadIn: return t * t;
			case Easing::QuadOut: return 1.0f - (1.0f - t) * (1.0f - t);
			case Easing::QuadInOut: return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) * 0.5f;

			case Easing::CubicIn: return t * t * t;
			case Easing::CubicOut: return 1.0f - std::pow(1.0f - t, 3.0f);
			case Easing::CubicInOut: return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) * 0.5f;

			case Easing::BackIn: return c3 * t * t * t - c1 * t * t;
			case Easing::BackOut: return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
			case Easing::BackInOut:
				return t < 0.5f ? (std::pow(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) * 0.5f
				                : (std::pow(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) * 0.5f;

			case Easing::BounceOut:
			{
				constexpr float n1 = 7.5625f;
				constexpr float d1 = 2.75f;
				if (t < 1.0f / d1)
				{
					return n1 * t * t;
				}
				if (t < 2.0f / d1)
				{
					t -= 1.5f / d1;
					return n1 * t * t + 0.75f;
				}
				if (t < 2.5f / d1)
				{
					t -= 2.25f / d1;
					return n1 * t * t + 0.9375f;
				}
				t -= 2.625f / d1;
				return n1 * t * t + 0.984375f;
			}

			case Easing::ElasticOut:
			{
				if (t <= 0.0f || t >= 1.0f)
				{
					return t <= 0.0f ? 0.0f : 1.0f;
				}
				constexpr float c4 = (2.0f * std::numbers::pi_v<float>) / 3.0f;
				return std::pow(2.0f, -10.0f * t) * std::sin((t * 10.0f - 0.75f) * c4) + 1.0f;
			}

			case Easing::Smooth: return t * t * (3.0f - 2.0f * t); // without neighbors, see SmoothWeights
		}
		return t;
	}

	/// @brief
	/// @param times
	/// @param flatStart
	/// @param flatEnd
	/// @param t
	/// @param outWeights
	void AnimationKey::SmoothWeights(const float times[4], bool flatStart, bool flatEnd, float t, float outWeights[4])
	{
		// The tangents (Catmull-Rom: towards the next key's value from the previous one's), scaled to the segment's duration
		float duration = times[2] - times[1];
		float startScale = (flatStart || times[2] - times[0] <= 0.0f) ? 0.0f : duration / (times[2] - times[0]);
		float endScale = (flatEnd || times[3] - times[1] <= 0.0f) ? 0.0f : duration / (times[3] - times[1]);

		float t2 = t * t;
		float t3 = t2 * t;
		float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
		float h10 = t3 - 2.0f * t2 + t;
		float h01 = -2.0f * t3 + 3.0f * t2;
		float h11 = t3 - t2;

		// h00 p1 + h10 startScale (p2 - p0) + h01 p2 + h11 endScale (p3 - p1)
		outWeights[0] = -h10 * startScale;
		outWeights[1] = h00 - h11 * endScale;
		outWeights[2] = h01 + h10 * startScale;
		outWeights[3] = h11 * endScale;
	}

	/// @brief
	/// @param target
	/// @param property
	/// @param outValue
	/// @return
	bool AnimationKey::GetValue(const Node& target, std::string_view property, PropertyValue& outValue) const
	{
		return outValue.Load(target, property, _value.GetRootNode());
	}

	/// @brief
	/// @param value
	void AnimationKey::SetValue(const PropertyValue& value)
	{
		value.Save(_value.GetRootNode());
	}

	/// @brief
	/// @param time
	/// @param value
	/// @return
	uint32_t AnimationTrack::SetKey(float time, const PropertyValue& value)
	{
		int32_t existing = FindKey(time);
		if (existing >= 0)
		{
			_keys[existing].SetValue(value);
			return (uint32_t)existing;
		}

		uint32_t index = 0;
		while (index < _keys.Size() && _keys[index]._time < time)
		{
			++index;
		}

		AnimationKey key;
		key._time = time;
		key.SetValue(value);
		_keys.Insert(index, std::move(key));
		return index;
	}

	/// @brief
	/// @param time
	/// @return
	int32_t AnimationTrack::FindKey(float time) const
	{
		for (uint32_t index = 0; index < _keys.Size(); ++index)
		{
			if (std::abs(_keys[index]._time - time) <= KeyTimeTolerance)
			{
				return (int32_t)index;
			}
		}
		return -1;
	}

	/// @brief
	void AnimationTrack::SortKeys()
	{
		std::stable_sort(_keys.Begin(), _keys.End(), [](const AnimationKey& left, const AnimationKey& right) { return left._time < right._time; });
	}

	/// @brief
	/// @param target
	/// @param property
	/// @return
	AnimationTrack* AnimationClip::FindTrack(uint64_t target, std::string_view property)
	{
		for (AnimationTrack& track : _tracks)
		{
			if (track._target == target && std::string_view(track._property.CStr(), track._property.Length()) == property)
			{
				return &track;
			}
		}
		return nullptr;
	}

	/// @brief
	/// @param target
	/// @param property
	/// @return
	const AnimationTrack* AnimationClip::FindTrack(uint64_t target, std::string_view property) const
	{
		return const_cast<AnimationClip*>(this)->FindTrack(target, property);
	}

	/// @brief
	/// @param target
	/// @param property
	/// @param time
	/// @param value
	void AnimationClip::SetKey(uint64_t target, std::string_view property, float time, const PropertyValue& value)
	{
		AnimationTrack* track = FindTrack(target, property);
		if (track == nullptr)
		{
			track = &_tracks.EmplaceBack();
			track->_target = target;
			track->_property = String(property.data(), (uint32_t)property.size());
		}
		track->SetKey(time, value);
	}

	/// @brief
	/// @param oldToNew
	/// @param keepOthers
	void AnimationClip::RemapTargets(const std::map<uint64_t, uint64_t>& oldToNew, bool keepOthers)
	{
		for (uint32_t index = 0; index < _tracks.Size();)
		{
			AnimationTrack& track = _tracks[index];
			auto            it = oldToNew.find(track._target);
			if (it != oldToNew.end())
			{
				track._target = it->second;
			}
			else if (keepOthers == false)
			{
				_tracks.Erase(index);
				continue;
			}
			++index;
		}
	}
}
