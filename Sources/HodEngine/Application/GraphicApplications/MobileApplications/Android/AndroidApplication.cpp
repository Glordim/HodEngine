#include "HodEngine/Application/Pch.hpp"
#include "HodEngine/Application/GraphicApplications/MobileApplications/Android/AndroidApplication.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <HodEngine/Core/Frame/FrameSequencer.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/PlatformRenderer.hpp>

#include <HodEngine/Window/PlatformWindow.hpp>
#include <HodEngine/Window/PlatformDisplayManager.hpp>

#include <HodEngine/Audio/PlatformAudioManager.hpp>

#include <HodEngine/Input/InputManager.hpp>

#include <HodEngine/ImGui/ImGuiManager.hpp>

#include "HodEngine/Core/Time/SystemTime.hpp"

#include "HodEngine/Core/FileSystem/FileSystem.hpp"

#include <HodEngine/Core/Job/JobScheduler.hpp>
#include <HodEngine/Core/Frame/FrameSequencer.hpp>

#include "HodEngine/Game/World.hpp"
#include "HodEngine/Game/Builtin.hpp"
#include "HodEngine/Game/ComponentFactory.hpp"
#include "HodEngine/Game/SerializedDataFactory.hpp"

#include "HodEngine/Physics/Physics.hpp"

#include "HodEngine/Core/Time/SystemTime.hpp"

#include "HodEngine/Window/Window.hpp"

#include <game-activity/GameActivity.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>

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

namespace hod::application
{
	/// @brief 
	/// @param androidApp 
	/// @return 
	bool AndroidApplication::Init(android_app* androidApp)
	{
		if (androidApp == nullptr)
		{
			OUTPUT_ERROR("android_app is nullptr");
			return false;
		}

		_androidApp = androidApp;
		_androidApp->userData = this;
		_androidApp->onAppCmd = &AndroidApplication::CommandHandleStatic;

		if (FileSystem::CreateInstance()->Init(_androidApp->activity->assetManager) == false)
		{
			return false;
		}

		JobScheduler::CreateInstance();
		FrameSequencer::CreateInstance();

		ResourceManager::CreateInstance();

		physics::Physics::CreatePhysicsInstance()->Init();

		PlatformAudioManager::CreateInstance();
		if (PlatformAudioManager::GetInstance()->Initialize() == false)
		{
			return false;
		}

		window::AndroidDisplayManager* androidDisplayManager = window::AndroidDisplayManager::CreateInstance();
		if (androidDisplayManager->Initialize() == false)
		{
			return false;
		}

		while (_androidApp->window == nullptr)
		{
            int timeout = 0; // non-blocking
            int events;
            android_poll_source* source;
            int result = ALooper_pollOnce(timeout, nullptr, &events, reinterpret_cast<void**>(&source));
            switch (result)
			{
                case ALOOPER_POLL_TIMEOUT:
                    [[fallthrough]];
                case ALOOPER_POLL_WAKE:
                    // No events occurred before the timeout or explicit wake. Stop checking for events.
                    break;
                case ALOOPER_EVENT_ERROR:
                    OUTPUT_ERROR("ALooper_pollOnce returned an error");
                    break;
                case ALOOPER_POLL_CALLBACK:
                    break;
                default:
                    if (source)
					{
                        source->process(_androidApp, source);
                    }
            }	
		}

		_window = androidDisplayManager->CreateMainWindow(_androidApp->window);

		input::InputManager::CreateInstance();
		if (input::InputManager::GetInstance()->Initialize() == false)
		{
			return false;
		}

		PlatformRenderer::CreateInstance();
		if (PlatformRenderer::GetInstance()->Init(_window) == false)
		{
			return false;
		}

		game::ComponentFactory::CreateInstance();
		game::SerializedDataFactory::CreateInstance();
		game::RegisterBuiltin();

		game::World::CreateInstance()->Init();

		hod::Document document;
		hod::DocumentReaderJson reader;
		reader.Read(document, "Boot.json");

		hod::game::BootInfo bootInfo;
		hod::Serializer::Deserialize(bootInfo, document.GetRootNode());

		hod::ResourceManager::GetInstance()->SetResourceDirectory("Datas");
		
		_gameModule.Init(bootInfo._gameModule, false);
		if (_gameModule.Load() == false)
		{
			return false;
		}

		std::shared_ptr<hod::game::SceneResource> sceneResource = hod::ResourceManager::GetInstance()->GetResource<hod::game::SceneResource>(bootInfo._startupScene);
		hod::game::World::GetInstance()->AddScene(&sceneResource->GetScene());
		hod::game::World::GetInstance()->SetEditorPlaying(true);

		return true;
	}

	/// @brief 
	/// @return 
	bool AndroidApplication::Run()
	{
		return GraphicApplication::Run();
	}

	/// @brief 
	/// @param androidApp 
	/// @param command 
	void AndroidApplication::CommandHandleStatic(android_app* androidApp, int32_t command)
	{
		static_cast<AndroidApplication*>(androidApp->userData)->CommandHandle(command);
	}

	/// @brief 
	/// @param command 
	void AndroidApplication::CommandHandle(int32_t command)
	{
		switch (command)
		{
			case APP_CMD_INIT_WINDOW:
				// A new window is created, associate a renderer with it. You may replace this with a
				// "game" class if that suits your needs. Remember to change all instances of userData
				// if you change the class here as a reinterpret_cast is dangerous this in the
				// android_main function and the APP_CMD_TERM_WINDOW handler case.
				break;
			case APP_CMD_TERM_WINDOW:
				// The window is being destroyed. Use this to clean up your userData to avoid leaking
				// resources.
				// TODO
				break;
			default:
				break;
		}
    }
}
