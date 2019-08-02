#ifndef __INPUT_LISTENER_HPP__
#define __INPUT_LISTENER_HPP__

#include <functional>
#include <unordered_map>

class InputListener
{
public:
    struct KeyAxis
    {
        KeyAxis(int negative, int positive)
            : negative(negative)
            , positive(positive)
        {
        }

        int negative;
        int positive;
    };

    struct InternalKeyAxis
    {
        InternalKeyAxis(const KeyAxis& axis)
            : negativeKey(axis.negative)
            , usingNegative(false)
            , positiveKey(axis.positive)
            , usingPositive(false)
        {

        }

        int negativeKey;
        bool usingNegative;

        int positiveKey;
        bool usingPositive;
    };

public:
    InputListener();
    virtual ~InputListener();

    void process();

    void injectKeyInput(int key, int scancode, int action, int mods);
    void injectMouseButtonInput(int mouseButtonId, int action, int mods);
    void injectMouseMoveInput(int x, int y);

    void registerMouseMoveEvent(std::function<void(int, int)> callback);
    void registerMouseButtonEvent(int mouseButtonId, int action, std::function<void()> callback);

    void registerAxisEvent(const InputListener::KeyAxis& axis, std::function<void(float)> callback);

private:
    std::vector<std::function<void(int, int)>> mouseMoveCallbackList;
    std::unordered_map<int, std::vector<std::function<void()>>> mouseButtonPressCallbackList;
    std::unordered_map<int, std::vector<std::function<void()>>> mouseButtonReleaseCallbackList;
    std::unordered_map<InputListener::InternalKeyAxis*, std::function<void(float)>> axisCallbackList;
};

#endif
