#include "InputListener.h"

#include <SDL.h>

InputListener::InputListener()
{
}

InputListener::~InputListener()
{
    auto it = this->axisCallbackList.begin();
    auto itEnd = this->axisCallbackList.end();

    while (it != itEnd)
    {
        delete it->first;

        ++it;
    }
}

void InputListener::injectMouseButtonInput(int mouseButtonId, int action, int mods)
{
    if (action == SDL_PRESSED)
    {
        size_t callbackCount = this->mouseButtonPressCallbackList[mouseButtonId].size();

        for (int i = 0; i < callbackCount; ++i)
        {
            this->mouseButtonPressCallbackList[mouseButtonId][i]();
        }
    }
    else if (action == SDL_RELEASED)
    {
        size_t callbackCount = this->mouseButtonReleaseCallbackList[mouseButtonId].size();

        for (int i = 0; i < callbackCount; ++i)
        {
            this->mouseButtonReleaseCallbackList[mouseButtonId][i]();
        }
    }
}

void InputListener::injectMouseMoveInput(int x, int y)
{
    size_t callbackCount = this->mouseMoveCallbackList.size();

    for (int i = 0; i < callbackCount; ++i)
    {
        this->mouseMoveCallbackList[i](x, y);
    }
}

void InputListener::injectKeyInput(int key, int scancode, int action, int mods)
{
    auto it = this->axisCallbackList.begin();
    auto itEnd = this->axisCallbackList.end();

    if (action == SDL_RELEASED)
    {
        while (it != itEnd)
        {
            if (scancode == it->first->negativeKey)
            {
                it->first->usingNegative = false;
                /*
                if (it->first->usingPositive == false)
                {
                    auto itUsed = this->usedAxis.begin();
                    auto itUsedEnd = this->usedAxis.begin();

                    while (itUsed != itUsedEnd)
                    {
                        if (*itUsed == it->first)
                        {
                            this->usedAxis.erase(itUsed);
                            break;
                        }

                        ++itUsed;
                    }
                }
                */
            }
            else if (scancode == it->first->positiveKey)
            {
                it->first->usingPositive = false;
                /*
                if (it->first->usingNegative == false)
                {
                    auto itUsed = this->usedAxis.begin();
                    auto itUsedEnd = this->usedAxis.begin();

                    while (itUsed != itUsedEnd)
                    {
                        if (*itUsed == it->first)
                        {
                            this->usedAxis.erase(itUsed);
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
            if (scancode == it->first->negativeKey)
            {
                it->first->usingNegative = true;
                /*
                auto itUsed = this->usedAxis.begin();
                auto itUsedEnd = this->usedAxis.begin();

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
                    this->usedAxis.push_back(it->first);
                }
                */
            }
            else if (scancode == it->first->positiveKey)
            {
                it->first->usingPositive = true;
                /*
                auto itUsed = this->usedAxis.begin();
                auto itUsedEnd = this->usedAxis.begin();

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
                    this->usedAxis.push_back(it->first);
                }
                */
            }

            ++it;
        }
    }
}

void InputListener::registerMouseButtonEvent(int mouseButtonId, int action, std::function<void()> callback)
{
    if (action == SDL_PRESSED)
        this->mouseButtonPressCallbackList[mouseButtonId].push_back(callback);
    else if (action == SDL_RELEASED)
        this->mouseButtonReleaseCallbackList[mouseButtonId].push_back(callback);
}

void InputListener::registerMouseMoveEvent(std::function<void(int, int)> callback)
{
    this->mouseMoveCallbackList.push_back(callback);
}

void InputListener::registerAxisEvent(const InputListener::KeyAxis& axis, std::function<void(float)> callback)
{
    this->axisCallbackList[new InputListener::InternalKeyAxis(axis)] = callback;
}

void InputListener::process()
{
    auto it = this->axisCallbackList.begin();
    auto itEnd = this->axisCallbackList.end();

    while (it != itEnd)
    {
        float axisValue = 0.0f;

        if (it->first->usingNegative == true)
            axisValue += -1.0f;
        if (it->first->usingPositive == true)
            axisValue += 1.0f;

        it->second(axisValue);

        ++it;
    }
}
