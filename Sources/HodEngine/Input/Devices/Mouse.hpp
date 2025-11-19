#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/Device.hpp"
#include "HodEngine/Input/State.hpp"

#include "HodEngine/Input/InputAxis.hpp"
#include "HodEngine/Input/InputButton.hpp"

#include "HodEngine/Core/Math/Vector2.hpp"

namespace hod::input
{
	/// @brief
	class HOD_INPUT_API Mouse : public Device
	{
	public:
		Mouse(const UID& uid, const std::string_view& name, Product product);
		Mouse(const Mouse&) = delete;
		Mouse(Mouse&&) = delete;
		~Mouse() override = default;

		Mouse& operator=(const Mouse&) = delete;
		Mouse& operator=(Mouse&&) = delete;

	public:
		static inline const Mouse* Current()
		{
			return _current;
		}

		// inline const InputAxis&	GetAxis() const { return _axis; }
		inline const InputAxis& GetAxisX() const
		{
			return _axisX;
		}

		inline const InputAxis& GetAxisY() const
		{
			return _axisY;
		}

		inline const InputAxis& GetAxisWheel() const
		{
			return _axisWheel;
		}

		inline const InputButton& GetButtonLeft() const
		{
			return _buttonLeft;
		}

		inline const InputButton& GetButtonRight() const
		{
			return _buttonRight;
		}

		inline const InputButton& GetButtonWheel() const
		{
			return _buttonWheel;
		}

		inline const InputButton& GetButtonForward() const
		{
			return _buttonForward;
		}

		inline const InputButton& GetButtonBackward() const
		{
			return _buttonBackward;
		}

	protected:
		inline void MakeCurrent() override
		{
			_current = this;
		}

	protected:
		static Mouse* _current;

	protected:
		// InputAxis		_axis;
		InputAxis _axisX;
		InputAxis _axisY;

		InputAxis _axisWheel;

		InputButton _buttonLeft;
		InputButton _buttonRight;
		InputButton _buttonWheel;

		InputButton _buttonForward;
		InputButton _buttonBackward;
	};
}
