#include "HodApplication.hpp"

#include <HodEngine/Core/ArgumentParser.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>

#include <HodEngine/GameSystems/Resource/Resource.hpp>
#include <HodEngine/GameSystems/Resource/ResourceManager.hpp>

#include <HodEngine/Game/BootInfo.hpp>
#include <HodEngine/Game/Scene.hpp>
#include <HodEngine/Game/SceneResource.hpp>
#include <HodEngine/Game/World.hpp>

#include <HodEngine/Application/GraphicApplications/GraphicApplication.hpp>
#include <HodEngine/Window/Desktop/DesktopWindow.hpp>
#include <HodEngine/Window/Window.hpp>

_SingletonOverrideConstructor(HodApplication) {}

#if defined(HOD_GAME_MODULE_STATIC)
extern "C"
{
	int StartupModule();
	int ShutdownModule();
}
#endif

/// @brief
/// @param argc
/// @param argv
/// @return
bool HodApplication::Init(const hod::ArgumentParser& argumentParser)
{
	bool platformApplicationResult = PlatformApplication::Init(argumentParser);
	if (platformApplicationResult == false)
	{
		return false;
	}

	hod::Path buildPath;

	const hod::Argument* datasPathArgument = argumentParser.GetArgument('p', "BuildPath");
	if (datasPathArgument != nullptr && datasPathArgument->_values[0] != nullptr)
	{
		buildPath = datasPathArgument->_values[0];
	}

	hod::Document           document;
	hod::DocumentReaderJson reader;
	reader.Read(document, buildPath / "Boot.json");

	hod::BootInfo bootInfo;
	hod::Serializer::Deserialize(bootInfo, document.GetRootNode());

	hod::ResourceManager::GetInstance()->SetResourceDirectory(buildPath / "Datas");

#if defined(HOD_GAME_MODULE_STATIC)
	StartupModule();
#else
	_gameModule.Init(buildPath / bootInfo._gameModule, false);
	if (_gameModule.Load() == false)
	{
		return false;
	}
	auto startupFunc = _gameModule.LoadFunction<int(*)()>("StartupModule");
	if (startupFunc == nullptr)
	{
		return false;
	}
	startupFunc();
#endif

	std::shared_ptr<hod::SceneResource> sceneResource = hod::ResourceManager::GetInstance()->GetResource<hod::SceneResource>(bootInfo._startupScene);
	if (sceneResource == nullptr)
	{
		return false;
	}

	hod::Scene* startupScene = sceneResource->CreateScene();
	if (startupScene == nullptr)
	{
		return false;
	}

	_world = hod::DefaultAllocator::GetInstance().New<hod::game::World>();
	_world->Init();
	_world->AddScene(startupScene);
	_world->SetEditorPlaying(true);
	startupScene->ProcessActivation();

	// todo remove
	static_cast<hod::DesktopWindow*>(hod::GraphicApplication::GetInstance()->GetWindow())->SetVisible(true);
	//

	return true;
}

/// @brief
void HodApplication::Terminate()
{
	hod::DefaultAllocator::GetInstance().Delete(_world);
	_world = nullptr;

#if defined(HOD_GAME_MODULE_STATIC)
	ShutdownModule();
#else
	auto shutdownFunc = _gameModule.LoadFunction<int(*)()>("ShutdownModule");
	if (shutdownFunc != nullptr)
	{
		shutdownFunc();
	}
#endif

	PlatformApplication::Terminate();
}
