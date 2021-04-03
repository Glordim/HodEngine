#pragma once

#include <HodEngine/Application/src/Application.h>

namespace HOD
{
	namespace CORE
	{
		class UID;
	}

	namespace GAME
	{
		class Scene;
	}
}

class MyApplication : public HOD::APPLICATION::Application
{
public:

						MyApplication();
						~MyApplication() override;

protected:

	bool				PreRun() override;
	bool				Loop(float deltaTime) override;
	bool				PostRun() override;

	bool				LoadStartingScene(const HOD::CORE::UID& startingSceneUid) override;

private:

	HOD::GAME::Scene*	_scene;
};
