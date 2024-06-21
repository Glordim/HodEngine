#pragma once

#include "HodEngine/Input/InputId.hpp"

namespace hod::input
{
	class Device;

	/// @brief 
	class Input
	{
		friend class Device;

	public:

		/// @brief 
		struct State
		{
						State(float value = 0.0f, uint8_t flags = 0, InputId inputId = InputId::Unknown);
			void		Merge(const State& state);
			bool		IsPressed() const;
			bool		IsJustPressed() const;
			bool		IsReleased() const;
			bool		IsJustReleased() const;
			bool		IsJustRepeat() const;
			bool		IsJustPressedOrRepeat() const;

			float		_value;
			uint8_t		_flags;
			InputId		_inputId;
			//UID	_deviceUid;
		};

		enum Flag : uint8_t // Keep an order by Priority (see STATE::Merge)
		{
			Released = 1 << 0,
			Pressed = 1 << 1,
			JustReleased = 1 << 2,
			JustPressed = 1 << 3,
			JustRepeat = 1 << 4,
		};

	private:

		static constexpr float RepeatThreshold = 0.75f;
		static constexpr float RepeatTimestep = 0.10f;

	public:

										Input(InputId inputId);
										Input(const Input& other) = default;
										Input(Input&& other) = default;
										~Input() = default;

		Input&							operator = (const Input& other) = default;
		Input&							operator = (Input&& other) = default;

		InputId							GetInputId() const;
		float							GetValue() const;
		uint8_t							GetFlags() const;
		State							GetState() const;

	private:

		void							SetValue(float newValue);

		void							ClearInputFlags();
		void							UpdateInputFlags();

	private:

		const InputId					_inputId;
		float							_value = 0.0f;
		uint8_t							_flags = Flag::Released;

		double							_elapsedTime = 0.0f;
		uint32_t						_repeatCount = 0;
	};
}
