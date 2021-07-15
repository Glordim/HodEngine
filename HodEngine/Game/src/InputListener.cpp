#include "InputListener.h"

#include <SDL2/SDL.h>

namespace HOD
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	InputListener::KeyAxis::KeyAxis(int negative, int positive)
		: _negative(negative)
		, _positive(positive)
	{
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	InputListener::InternalKeyAxis::InternalKeyAxis(const KeyAxis& axis)
		: _negativeKey(axis._negative)
		, _usingNegative(false)
		, _positiveKey(axis._positive)
		, _usingPositive(false)
	{

	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	InputListener::InputListener()
	{
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	InputListener::~InputListener()
	{
		auto it = _axisCallbackList.begin();
		auto itEnd = _axisCallbackList.end();

		while (it != itEnd)
		{
			delete it->first;

			++it;
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	void InputListener::InjectMouseButtonInput(int mouseButtonId, int action, int mods)
	{
		if (action == SDL_PRESSED)
		{
			size_t callbackCount = _mouseButtonPressCallbackList[mouseButtonId].size();

			for (int i = 0; i < callbackCount; ++i)
			{
				_mouseButtonPressCallbackList[mouseButtonId][i]();
			}
		}
		else if (action == SDL_RELEASED)
		{
			size_t callbackCount = _mouseButtonReleaseCallbackList[mouseButtonId].size();

			for (int i = 0; i < callbackCount; ++i)
			{
				_mouseButtonReleaseCallbackList[mouseButtonId][i]();
			}
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	void InputListener::InjectMouseMoveInput(int x, int y)
	{
		size_t callbackCount = _mouseMoveCallbackList.size();

		for (int i = 0; i < callbackCount; ++i)
		{
			_mouseMoveCallbackList[i](x, y);
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	void InputListener::InjectKeyInput(int key, int scancode, int action, int mods)
	{
		auto it = _axisCallbackList.begin();
		auto itEnd = _axisCallbackList.end();

		if (action == SDL_RELEASED)
		{
			while (it != itEnd)
			{
				if (key == it->first->_negativeKey)
				{
					it->first->_usingNegative = false;
					/*
					if (it->first->usingPositive == false)
					{
						auto itUsed = usedAxis.begin();
						auto itUsedEnd = usedAxis.begin();

						while (itUsed != itUsedEnd)
						{
							if (*itUsed == it->first)
							{
								usedAxis.erase(itUsed);
								break;
							}

							++itUsed;
						}
					}
					*/
				}
				else if (key == it->first->_positiveKey)
				{
					it->first->_usingPositive = false;
					/*
					if (it->first->usingNegative == false)
					{
						auto itUsed = usedAxis.begin();
						auto itUsedEnd = usedAxis.begin();

						while (itUsed != itUsedEnd)
						{
							if (*itUsed == it->first)
							{
								>usedAxis.erase(itUsed);
								break;
							}

							++itUsed;
						}
					}
					*/
				}

				++it;
			}
		}
		else if (action == SDL_PRESSED)
		{
			while (it != itEnd)
			{
				if (key == it->first->_negativeKey)
				{
					it->first->_usingNegative = true;
					/*
					auto itUsed = usedAxis.begin();
					auto itUsedEnd = usedAxis.begin();

					bool exist = false;

					while (itUsed != itUsedEnd)
					{
						if (*itUsed == it->first)
						{
							exist = true;
							break;
						}

						++itUsed;
					}

					if (exist == false)
					{
						usedAxis.push_back(it->first);
					}
					*/
				}
				else if (key == it->first->_positiveKey)
				{
					it->first->_usingPositive = true;
					/*
					auto itUsed = usedAxis.begin();
					auto itUsedEnd = usedAxis.begin();

					bool exist = false;

					while (itUsed != itUsedEnd)
					{
						if (*itUsed == it->first)
						{
							exist = true;
							break;
						}

						++itUsed;
					}

					if (exist == false)
					{
						usedAxis.push_back(it->first);
					}
					*/
				}

				++it;
			}
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	void InputListener::RegisterMouseButtonEvent(int mouseButtonId, int action, std::function<void()> callback)
	{
		if (action == SDL_PRESSED)
		{
			_mouseButtonPressCallbackList[mouseButtonId].push_back(callback);
		}
		else if (action == SDL_RELEASED)
		{
			_mouseButtonReleaseCallbackList[mouseButtonId].push_back(callback);
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	void InputListener::RegisterMouseMoveEvent(std::function<void(int, int)> callback)
	{
		_mouseMoveCallbackList.push_back(callback);
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	void InputListener::RegisterAxisEvent(const InputListener::KeyAxis& axis, std::function<void(float)> callback)
	{
		_axisCallbackList[new InputListener::InternalKeyAxis(axis)] = callback;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	void InputListener::Process()
	{
		auto it = _axisCallbackList.begin();
		auto itEnd = _axisCallbackList.end();

		while (it != itEnd)
		{
			float axisValue = 0.0f;

			if (it->first->_usingNegative == true)
			{
				axisValue += -1.0f;
			}
			if (it->first->_usingPositive == true)
			{
				axisValue += 1.0f;
			}

			it->second(axisValue);

			++it;
		}
	}
}
