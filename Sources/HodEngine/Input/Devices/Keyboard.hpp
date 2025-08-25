#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Devices/Device.hpp"
#include "HodEngine/Input/InputButton.hpp"

namespace hod::input
{
	/// @brief 
	class HOD_INPUT_API Keyboard : public Device
	{
	public:

						Keyboard(const UID& uid, const std::string_view& name, Product product);
						Keyboard(const Keyboard&) = delete;
						Keyboard(Keyboard&&) = delete;
						~Keyboard() override = default;

		Keyboard&		operator = (const Keyboard&) = delete;
		Keyboard&		operator = (Keyboard&&) = delete;

	public:

		static inline const Keyboard* Current() { return _current; }

		void				ClearBufferedTextIfNeeded();
		void				AppendCharactersToBufferedText(const std::string_view& characters);
		const String&		GetBufferedText();

	protected:

		inline void MakeCurrent() override { _current = this; }

	protected:

		static Keyboard* _current;

	private:

		std::string			_bufferedText;
		bool				_readed = false;

		InputButton							_a;
		InputButton							_b;
		InputButton							_c;
		InputButton							_d;
		InputButton							_e;
		InputButton							_f;
		InputButton							_g;
		InputButton							_h;
		InputButton							_i;
		InputButton							_j;
		InputButton							_k;
		InputButton							_l;
		InputButton							_m;
		InputButton							_n;
		InputButton							_o;
		InputButton							_p;
		InputButton							_q;
		InputButton							_r;
		InputButton							_s;
		InputButton							_t;
		InputButton							_u;
		InputButton							_v;
		InputButton							_w;
		InputButton							_x;
		InputButton							_y;
		InputButton							_z;
/*
		InputButton							_0;
		InputButton							_1;
		InputButton							_2;
		InputButton							_3;
		InputButton							_4;
		InputButton							_5;
		InputButton							_6;
		InputButton							_7;
		InputButton							_8;
		InputButton							_9;

		InputButton							_minus;
		InputButton							_equals;
		InputButton							_leftBracket;
		InputButton							_rightBracket;
		InputButton							_backslash;
		InputButton							_semicolon;
		InputButton							_quote;
		InputButton							_comma;
		InputButton							_period;
		InputButton							_slash;

		InputButton							_numpad0;
		InputButton							_numpad1;
		InputButton							_numpad2;
		InputButton							_numpad3;
		InputButton							_numpad4;
		InputButton							_numpad5;
		InputButton							_numpad6;
		InputButton							_numpad7;
		InputButton							_numpad8;
		InputButton							_numpad9;
		InputButton							_numpadLock;
		InputButton							_numpadDivide;
		InputButton							_numpadMultiply;
		InputButton							_numpadMinus;
		InputButton							_numpadPlus;
		InputButton							_numpadEnter;
		InputButton							_numpadPeriod;

		InputButton							_f1;
		InputButton							_f2;
		InputButton							_f3;
		InputButton							_f4;
		InputButton							_f5;
		InputButton							_f6;
		InputButton							_f7;
		InputButton							_f8;
		InputButton							_f9;
		InputButton							_f10;
		InputButton							_f11;
		InputButton							_f12;

		InputButton							_escape;
		InputButton							_backquote;
		InputButton							_tab;
		InputButton							_capslock;
		InputButton							_backspace;
		InputButton							_enter;

		InputButton							_leftShift;
		InputButton							_leftControl;
		InputButton							_leftAlt;
		InputButton							_leftMeta;

		InputButton							_rightShift;
		InputButton							_rightControl;
		InputButton							_rightAlt;
		InputButton							_rightMeta;

		InputButton							_leftArrow;
		InputButton							_rightArrow;
		InputButton							_upArrow;
		InputButton							_downArrow;

		InputButton							_pageUp;
		InputButton							_pageDown;
		InputButton							_home;
		InputButton							_end;
		InputButton							_insert;
		InputButton							_delete;

		InputButton							_scrollock;
		InputButton							_printScreen;
		InputButton							_pause;

		InputButton							_anyKey;
	*/
	};
}
