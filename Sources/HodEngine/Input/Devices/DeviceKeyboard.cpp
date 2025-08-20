#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/Devices/DeviceKeyboard.hpp"
#include "HodEngine/Input/State.hpp"

namespace hod::input
{
	struct KeyboardState : public State
	{
		uint8_t _keys[16]; // 128 bits, 1 key = 1 bit
	};

	/// @brief 
	/// @param uid 
	/// @param name 
	/// @param product 
	DeviceKeyboard::DeviceKeyboard(const UID& uid, const std::string_view& name, Product product)
	: Device(Type::KEYBOARD, uid, name, product, sizeof(KeyboardState))
	, _a(Identifier(), "a", StateView(0, 0, 1))
	, _b(Identifier(), "b", StateView(1, 3, 1))
	, _c(Identifier(), "c", StateView(1, 0, 1))
	, _d(Identifier(), "d", StateView(0, 2, 1))
	, _e(Identifier(), "e", StateView(1, 6, 1))
	, _f(Identifier(), "f", StateView(0, 3, 1))
	, _g(Identifier(), "g", StateView(0, 5, 1))
	, _h(Identifier(), "h", StateView(0, 4, 1))
	, _i(Identifier(), "i", StateView(4, 2, 1))
	, _j(Identifier(), "j", StateView(4, 6, 1))
	, _k(Identifier(), "k", StateView(5, 0, 1))
	, _l(Identifier(), "l", StateView(4, 5, 1))
	, _m(Identifier(), "m", StateView(0, 0, 1))
	, _n(Identifier(), "n", StateView(0, 0, 1))
	, _o(Identifier(), "o", StateView(0, 0, 1))
	, _p(Identifier(), "p", StateView(0, 0, 1))
	, _q(Identifier(), "q", StateView(0, 0, 1))
	, _r(Identifier(), "r", StateView(0, 0, 1))
	, _s(Identifier(), "s", StateView(0, 0, 1))
	, _t(Identifier(), "t", StateView(0, 0, 1))
	, _u(Identifier(), "u", StateView(0, 0, 1))
	, _v(Identifier(), "v", StateView(0, 0, 1))
	, _w(Identifier(), "w", StateView(0, 0, 1))
	, _x(Identifier(), "x", StateView(0, 0, 1))
	, _y(Identifier(), "y", StateView(0, 0, 1))
	, _z(Identifier(), "z", StateView(0, 0, 1))
	{
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

	/// @brief 
	void DeviceKeyboard::ClearBufferedTextIfNeeded()
	{
		if (_readed == true)
		{
			_readed = false;
			_bufferedText.clear();
		}
	}

	/// @brief 
	/// @param characters 
	void DeviceKeyboard::AppendCharactersToBufferedText(const std::string_view& characters)
	{
		_bufferedText += characters;
	}

	/// @brief 
	/// @return 
	const String& DeviceKeyboard::GetBufferedText()
	{
		_readed = true;
		return _bufferedText;
	}
}
