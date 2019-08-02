#ifndef __COMPONENT_HPP__
#define __COMPONENT_HPP__

class Actor;
typedef struct CTwBar TwBar;

class Component
{
protected:
    Component() = delete;
    Component(const Component& copy) = delete;

    Component(Actor* actor);
    virtual ~Component();

    friend class Actor;

private:
    Actor* actor;

public:
    Actor* getActor() const;

    virtual void setupTweakBar(TwBar* bar);
};

#endif
