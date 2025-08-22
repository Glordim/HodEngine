#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/Devices/Keyboard.hpp"
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
	Keyboard::Keyboard(const UID& uid, const std::string_view& name, Product product)
	: Device(Type::KEYBOARD, uid, name, product, sizeof(KeyboardState))
	, _a(Identifier(), "a", StateView(StateView::Format::Bit, 0, 0))
	, _b(Identifier(), "b", StateView(StateView::Format::Bit, 1, 3))
	, _c(Identifier(), "c", StateView(StateView::Format::Bit, 1, 0))
	, _d(Identifier(), "d", StateView(StateView::Format::Bit, 0, 2))
	, _e(Identifier(), "e", StateView(StateView::Format::Bit, 1, 6))
	, _f(Identifier(), "f", StateView(StateView::Format::Bit, 0, 3))
	, _g(Identifier(), "g", StateView(StateView::Format::Bit, 0, 5))
	, _h(Identifier(), "h", StateView(StateView::Format::Bit, 0, 4))
	, _i(Identifier(), "i", StateView(StateView::Format::Bit, 4, 2))
	, _j(Identifier(), "j", StateView(StateView::Format::Bit, 4, 6))
	, _k(Identifier(), "k", StateView(StateView::Format::Bit, 5, 0))
	, _l(Identifier(), "l", StateView(StateView::Format::Bit, 4, 5))
	, _m(Identifier(), "m", StateView(StateView::Format::Bit, 0, 0))
	, _n(Identifier(), "n", StateView(StateView::Format::Bit, 0, 0))
	, _o(Identifier(), "o", StateView(StateView::Format::Bit, 0, 0))
	, _p(Identifier(), "p", StateView(StateView::Format::Bit, 0, 0))
	, _q(Identifier(), "q", StateView(StateView::Format::Bit, 0, 0))
	, _r(Identifier(), "r", StateView(StateView::Format::Bit, 0, 0))
	, _s(Identifier(), "s", StateView(StateView::Format::Bit, 0, 0))
	, _t(Identifier(), "t", StateView(StateView::Format::Bit, 0, 0))
	, _u(Identifier(), "u", StateView(StateView::Format::Bit, 0, 0))
	, _v(Identifier(), "v", StateView(StateView::Format::Bit, 0, 0))
	, _w(Identifier(), "w", StateView(StateView::Format::Bit, 0, 0))
	, _x(Identifier(), "x", StateView(StateView::Format::Bit, 0, 0))
	, _y(Identifier(), "y", StateView(StateView::Format::Bit, 0, 0))
	, _z(Identifier(), "z", StateView(StateView::Format::Bit, 0, 0))
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
	void Keyboard::ClearBufferedTextIfNeeded()
	{
		if (_readed == true)
		{
			_readed = false;
			_bufferedText.clear();
		}
	}

	/// @brief 
	/// @param characters 
	void Keyboard::AppendCharactersToBufferedText(const std::string_view& characters)
	{
		_bufferedText += characters;
	}

	/// @brief 
	/// @return 
	const String& Keyboard::GetBufferedText()
	{
		_readed = true;
		return _bufferedText;
	}
}
