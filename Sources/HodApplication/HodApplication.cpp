#include "HodApplication.hpp"

#include <HodEngine/Core/ArgumentParser.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>

#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>

#include <HodEngine/Core/Resource/Resource.hpp>
#include <HodEngine/Core/Resource/ResourceManager.hpp>

#include <HodEngine/Game/BootInfo.hpp>
#include <HodEngine/Game/World.hpp>
#include <HodEngine/Game/Scene.hpp>
#include <HodEngine/Game/SceneResource.hpp>

#include <HodEngine/Application/GraphicApplications/GraphicApplication.hpp>
#include <HodEngine/Window/Window.hpp>
#include <HodEngine/Window/Desktop/DesktopWindow.hpp>

_SingletonOverrideConstructor(HodApplication)
{

}

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

	std::filesystem::path buildPath;

	const hod::Argument* datasPathArgument = argumentParser.GetArgument('p', "BuildPath");
	if (datasPathArgument != nullptr && datasPathArgument->_values[0] != nullptr)
	{
		buildPath = datasPathArgument->_values[0];
	}

	hod::Document document;
	hod::DocumentReaderJson reader;
	reader.Read(document, buildPath / "Boot.json");

	hod::game::BootInfo bootInfo;
	hod::Serializer::Deserialize(bootInfo, document.GetRootNode());

	hod::ResourceManager::GetInstance()->SetResourceDirectory(buildPath / "Datas");
	_gameModule.Init(buildPath / bootInfo._gameModule, false);
	if (_gameModule.Load() == false)
	{
		return false;
	}

	std::shared_ptr<hod::game::SceneResource> sceneResource = hod::ResourceManager::GetInstance()->GetResource<hod::game::SceneResource>(bootInfo._startupScene);
	if (sceneResource == nullptr)
	{
		return false;
	}

	hod::game::Scene* startupScene = sceneResource->CreateScene();
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
	static_cast<hod::window::DesktopWindow*>(hod::application::GraphicApplication::GetInstance()->GetWindow())->SetVisible(true);
	//

	return true;
}

/// @brief 
void HodApplication::Terminate()
{
	hod::DefaultAllocator::GetInstance().Delete(_world);
	_world = nullptr;

	PlatformApplication::Terminate();
}
