#include <limits>
#include <array>

#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/API/WindowLib/DeviceKeyboardWindowLib.hpp"
#include "HodEngine/Input/InputIdHelper.hpp"
#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Core/Type.hpp"

#undef max

namespace hod::input
{
	DeviceKeyboardWindowLib::DeviceKeyboardWindowLib()
	: DeviceKeyboard(UID::INVALID_UID, "Keyboard", Product::UNKNOWN)
	, _a(Identifier(), "a", StateView(0, 0, 1), &_currentState, &_previousState)
	, _b(Identifier(), "b", StateView(1, 3, 1), &_currentState, &_previousState)
	, _c(Identifier(), "c", StateView(1, 0, 1), &_currentState, &_previousState)
	, _d(Identifier(), "d", StateView(0, 2, 1), &_currentState, &_previousState)
	, _e(Identifier(), "e", StateView(1, 6, 1), &_currentState, &_previousState)
	, _f(Identifier(), "f", StateView(0, 3, 1), &_currentState, &_previousState)
	, _g(Identifier(), "g", StateView(0, 5, 1), &_currentState, &_previousState)
	, _h(Identifier(), "h", StateView(0, 4, 1), &_currentState, &_previousState)
	, _i(Identifier(), "i", StateView(4, 2, 1), &_currentState, &_previousState)
	, _j(Identifier(), "j", StateView(4, 6, 1), &_currentState, &_previousState)
	, _k(Identifier(), "k", StateView(5, 0, 1), &_currentState, &_previousState)
	, _l(Identifier(), "l", StateView(4, 5, 1), &_currentState, &_previousState)
	, _m(Identifier(), "m", StateView(0, 0, 1), &_currentState, &_previousState)
	, _n(Identifier(), "n", StateView(0, 0, 1), &_currentState, &_previousState)
	, _o(Identifier(), "o", StateView(0, 0, 1), &_currentState, &_previousState)
	, _p(Identifier(), "p", StateView(0, 0, 1), &_currentState, &_previousState)
	, _q(Identifier(), "q", StateView(0, 0, 1), &_currentState, &_previousState)
	, _r(Identifier(), "r", StateView(0, 0, 1), &_currentState, &_previousState)
	, _s(Identifier(), "s", StateView(0, 0, 1), &_currentState, &_previousState)
	, _t(Identifier(), "t", StateView(0, 0, 1), &_currentState, &_previousState)
	, _u(Identifier(), "u", StateView(0, 0, 1), &_currentState, &_previousState)
	, _v(Identifier(), "v", StateView(0, 0, 1), &_currentState, &_previousState)
	, _w(Identifier(), "w", StateView(0, 0, 1), &_currentState, &_previousState)
	, _x(Identifier(), "x", StateView(0, 0, 1), &_currentState, &_previousState)
	, _y(Identifier(), "y", StateView(0, 0, 1), &_currentState, &_previousState)
	, _z(Identifier(), "z", StateView(0, 0, 1), &_currentState, &_previousState)
	{
		std::memset(&_nextState, false, sizeof(_nextState));
		std::memset(&_currentState, false, sizeof(_currentState));
		std::memset(&_previousState, false, sizeof(_previousState));

		AddInput(&_a);
		AddInput(&_b);
		AddInput(&_c);
		AddInput(&_d);
		AddInput(&_e);
		AddInput(&_f);
		AddInput(&_g);
		AddInput(&_h);
		AddInput(&_i);
		AddInput(&_j);
		AddInput(&_k);
		AddInput(&_l);
		AddInput(&_m);
		AddInput(&_n);
		AddInput(&_o);
		AddInput(&_p);
		AddInput(&_q);
		AddInput(&_r);
		AddInput(&_s);
		AddInput(&_t);
		AddInput(&_u);
		AddInput(&_v);
		AddInput(&_w);
		AddInput(&_x);
		AddInput(&_y);
		AddInput(&_z);
	}

	bool DeviceKeyboardWindowLib::ApplyFeedback(Feedback& feedback)
	{
		(void)feedback;
		return false;
	}

	void DeviceKeyboardWindowLib::OnKeyPressed(int keycode)
	{
		uint8_t* bytePtr = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(&_nextState) + (keycode / 8));
		*bytePtr |= (1 << (keycode % 8));
	}

	void DeviceKeyboardWindowLib::OnKeyReleased(int keycode)
	{
		uint8_t* bytePtr = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(&_nextState) + (keycode / 8));
		*bytePtr &= ~(1 << (keycode % 8));
	}

	const State* DeviceKeyboardWindowLib::GetState() const
	{
		return &_currentState;
	}

	uint32_t DeviceKeyboardWindowLib::GetStateSize() const
	{
		return sizeof(_currentState);
	}

	void DeviceKeyboardWindowLib::UpdateState()
	{
		std::memcpy(&_previousState, &_currentState, sizeof(_currentState));
		_stateLock.Lock();
		std::memcpy(&_currentState, &_nextState, sizeof(_currentState));
		_stateLock.Unlock();
	}
}
