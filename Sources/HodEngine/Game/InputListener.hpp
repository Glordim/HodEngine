#pragma once
#include "HodEngine/Game/Export.hpp"

#include <functional>
#include <unordered_map>

namespace hod
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class InputListener
	{
	public:
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct HOD_GAME_API KeyAxis
		{
			KeyAxis(int negative, int positive);

			int	_negative;
			int	_positive;
		};

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct HOD_GAME_API InternalKeyAxis
		{
			InternalKeyAxis(const KeyAxis& axis);

			int		_negativeKey;
			bool	_usingNegative;

			int		_positiveKey;
			bool	_usingPositive;
		};

	public:
					InputListener();
					InputListener(const InputListener&) = delete;
					InputListener(InputListener&&) = delete;
		virtual		~InputListener();

		void		operator=(const InputListener&) = delete;
		void		operator=(InputListener&&) = delete;

	public:

		void		Process();

		void		InjectKeyInput(int key, int scancode, int action, int mods);
		void		InjectMouseButtonInput(int mouseButtonId, int action, int mods);
		void		InjectMouseMoveInput(int x, int y);

		void		RegisterMouseMoveEvent(std::function<void(int, int)> callback);
		void		RegisterMouseButtonEvent(int mouseButtonId, int action, std::function<void()> callback);

		void		RegisterAxisEvent(const InputListener::KeyAxis& axis, std::function<void(float)> callback);

	private:

		Vector<std::function<void(int, int)>>										_mouseMoveCallbackList;
		std::unordered_map<int, Vector<std::function<void()>>>						_mouseButtonPressCallbackList;
		std::unordered_map<int, Vector<std::function<void()>>>						_mouseButtonReleaseCallbackList;
		std::unordered_map<InputListener::InternalKeyAxis*, std::function<void(float)>>	_axisCallbackList;
	};
}
