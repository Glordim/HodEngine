#pragma once

#include <HodEngine/Application/src/Application.h>

class MyApplication : public HOD::APPLICATION::Application
{
public:
    MyApplication();
    ~MyApplication() override;

protected:

    bool PreRun() override;
    bool Loop(float deltaTime) override;
    bool PostRun() override;

};
