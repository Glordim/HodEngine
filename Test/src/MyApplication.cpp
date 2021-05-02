#include "MyApplication.h"

#include <HodEngine/Core/Src/UID.h>
#include <HodEngine/Physics/src/Physics.h>
#include <HodEngine/Physics/src/Actor.h>
#include <HodEngine/Renderer/src/Renderer.h>
#include <HodEngine/Renderer/src/RHI/Vulkan/RendererVulkan.h>
#include <HodEngine/Renderer/src/GpuDeviceHelper.h>
#include <HodEngine/Renderer/src/MaterialManager.h>
#include <HodEngine/Renderer/src/RHI/Material.h>
#include <HodEngine/Renderer/src/RHI/MaterialInstance.h>
#include <HodEngine/Renderer/src/RHI/Texture.h>
#include <HodEngine/Renderer/src/RenderQueue.h>
#include <HodEngine/Renderer/src/SpriteAtlas.h>
#include <HodEngine/Renderer/src/Sprite.h>
#include <HodEngine/Game/src/Debug/ActorDebugWindow.h>
#include <HodEngine/Game/src/Game.h>
#include <HodEngine/Game/src/Scene.h>
#include <HodEngine/Game/src/Actor.h>
#include <HodEngine/Game/src/Component/SceneComponent.h>
#include <HodEngine/Game/src/Component/CameraComponent.h>
#include <HodEngine/Game/src/Component/ColliderComponent.h>
#include <HodEngine/Game/src/Component/SpriteComponent.h>
#include <HodEngine/Game/src/Component/Light/PointLightComponent.h>
#include <HodEngine/Game/src/Component/Light/DirLightComponent.h>
#include <HodEngine/Game/src/Component/Light/SpotLightComponent.h>
#include <HodEngine/Game/src/EditorBridge.h>

#include <HodEngine/DebugLayer/src/DebugLayer.h>
#include <HodEngine/ImGui/src/imgui.h>

#include "FreeCam.h"
#include "FlyingPointLight.h"

using namespace HOD;
using namespace GAME;
using namespace RENDERER;

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
MyApplication::MyApplication()
{

}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
MyApplication::~MyApplication()
{

}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool MyApplication::PreRun()
{
	DEBUG_LAYER::DebugLayer* pDebugLayer = DEBUG_LAYER::DebugLayer::CreateInstance();
	if (pDebugLayer->Init() == false)
	{
		return false;
	}

	PHYSICS::Physics* pPhysics = PHYSICS::Physics::CreateInstance();
	if (pPhysics->Init() == false)
	{
		return false;
	}

	MaterialManager* materialManager = MaterialManager::CreateInstance();

#ifdef NDEBUG
	bool bEnableValidationLayer = false;
#else
	bool bEnableValidationLayer = true;
#endif

	Renderer* pRenderer = new RendererVulkan();
	if (pRenderer->Init() == false)
	{
		return false;
	}

	if (pRenderer->Init(this, bEnableValidationLayer) == false)
	{
		return false;
	}

	GpuDevice* bestDevice = nullptr;

	if (GpuDeviceHelper::GetBestAvailableAndCompatibleDevice(&bestDevice) == false)
	{
		return 1;
	}

	if (pRenderer->BuildPipeline(bestDevice) == false)
	{
		return 1;
	}

	if (pRenderer->SetupImGui() == false)
	{
		return 1;
	}

	Game* pGame = Game::CreateInstance();
	if (pGame->Init() == false)
	{
		return false;
	}

	if (IsRunningInEditor() == true)
	{
		EditorBridge* editorBridge = EditorBridge::CreateInstance();
		if (editorBridge->Init() == false)
		{
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool MyApplication::LoadStartingScene(const CORE::UID& startingSceneUid)
{
	_scene = Game::GetInstance()->CreateScene();

	FreeCam* freeCam = _scene->SpawnActor<FreeCam>("FreeCam");
	{
		SceneComponent* sceneComponent = freeCam->GetComponent<SceneComponent>();

		sceneComponent->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		sceneComponent->SetParent(_scene->GetRoot());

		SpotLightComponent* spotLightComponent = freeCam->AddComponent<SpotLightComponent>();
		spotLightComponent->_data.radius = 15.0f;
		spotLightComponent->_data.color = CORE::Color(1.0f, 1.0f, 1.0f, 1.0f);
		spotLightComponent->_data.intensity = 1.0f;
		spotLightComponent->_data.outer = 10.0f;
	}

	MaterialManager* materialManager = MaterialManager::GetInstance();

	Material* spriteMaterialUnlit = materialManager->GetData(materialManager->CreateMaterial("SpriteUnlitTextureColor"));
	if (spriteMaterialUnlit == nullptr)
	{
		return false;
	}

	MaterialInstance* spriteMaterialUnlitInstance = Renderer::GetInstance()->CreateMaterialInstance(spriteMaterialUnlit);
	if (spriteMaterialUnlitInstance == nullptr)
	{
		return false;
	}

	SpriteAtlas* spriteAtlas = new SpriteAtlas(); // TODO SpriteAltasManager like to MaterialManager
	spriteAtlas->LoadFromFile("Texture/Trampo/Trampo.json");

	spriteMaterialUnlitInstance->SetVec4("matUbo.color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//spriteMaterialUnlitInstance->SetTexture("textureSampler", *wallTexture);

	Actor* sprite = _scene->SpawnActor<Actor>("sprite");
	{
		SceneComponent* sceneComponent = sprite->AddComponent<SceneComponent>();
		sceneComponent->SetPosition(glm::vec3(-0.25f, -0.15f, 0.0f));
		sceneComponent->SetRotation(-25);
		sceneComponent->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		sceneComponent->SetParent(_scene->GetRoot());

		SpriteComponent* spriteComponent = sprite->AddComponent<SpriteComponent>();
		spriteComponent->SetMaterialInstance(spriteMaterialUnlitInstance);
		spriteComponent->SetSprite(spriteAtlas->FindSprite("Jump (28x28)-0.png"));

		ColliderComponent* colliderComponent = sprite->AddComponent<ColliderComponent>();
		const BoundingBox& boundingBox = spriteComponent->GetSprite()->GetBoundingBox();
		colliderComponent->AddBoxShape(glm::vec2(boundingBox.center.x, boundingBox.center.y), glm::vec2(boundingBox.size.x, boundingBox.size.y) * 0.5f, 0.0f, 0.0f);

		sprite->GetPhysicActor()->SetType(PHYSICS::Actor::TYPE::Static);
	}

	Actor* ball = _scene->SpawnActor<Actor>("ball");
	{
		SceneComponent* sceneComponent = ball->AddComponent<SceneComponent>();
		sceneComponent->SetPosition(glm::vec3(-0.25f, 0.35f, 0.0f));
		sceneComponent->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		sceneComponent->SetParent(_scene->GetRoot());

		SpriteComponent* spriteComponent = ball->AddComponent<SpriteComponent>();
		spriteComponent->SetMaterialInstance(spriteMaterialUnlitInstance);
		spriteComponent->SetSprite(spriteAtlas->FindSprite("Jump (28x28)-0.png"));

		ColliderComponent* colliderComponent = ball->AddComponent<ColliderComponent>();
		const BoundingBox& boundingBox = spriteComponent->GetSprite()->GetBoundingBox();
		colliderComponent->AddBoxShape(glm::vec2(boundingBox.center.x, boundingBox.center.y), glm::vec2(boundingBox.size.x, boundingBox.size.y) * 0.5f, 0.0f, 1.0f);

		ball->GetPhysicActor()->SetType(PHYSICS::Actor::TYPE::Dynamic);
	}
	Actor* ball2 = _scene->SpawnActor<Actor>("ball");
	{
		SceneComponent* sceneComponent = ball2->AddComponent<SceneComponent>();
		sceneComponent->SetPosition(glm::vec3(-0.27f, 0.48f, 0.0f));
		sceneComponent->SetRotation(95);
		sceneComponent->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		sceneComponent->SetParent(_scene->GetRoot());

		SpriteComponent* spriteComponent = ball2->AddComponent<SpriteComponent>();
		spriteComponent->SetMaterialInstance(spriteMaterialUnlitInstance);
		spriteComponent->SetSprite(spriteAtlas->FindSprite("Jump (28x28)-0.png"));

		ColliderComponent* colliderComponent = ball2->AddComponent<ColliderComponent>();
		const BoundingBox& boundingBox = spriteComponent->GetSprite()->GetBoundingBox();
		colliderComponent->AddBoxShape(glm::vec2(boundingBox.center.x, boundingBox.center.y), glm::vec2(boundingBox.size.x, boundingBox.size.y) * 0.5f, 0.0f, 1.0f);

		ball2->GetPhysicActor()->SetType(PHYSICS::Actor::TYPE::Dynamic);
	}

	Actor* ground = _scene->SpawnActor<Actor>("ground");
	{
		SceneComponent* sceneComponent = ground->AddComponent<SceneComponent>();
		sceneComponent->SetPosition(glm::vec3(0.0f, -0.45f, 0.0f));
		sceneComponent->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));
		sceneComponent->SetParent(_scene->GetRoot());

		ColliderComponent* colliderComponent = ground->AddComponent<ColliderComponent>();
		colliderComponent->AddEdgeShape(glm::vec2(-1.0f, 0.0f), glm::vec2(1.0f, 0.0f));

		ground->GetPhysicActor()->SetType(PHYSICS::Actor::TYPE::Static);
	}

	return true;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool MyApplication::Loop(float deltaTime)
{
	EditorBridge* editorBridge = EditorBridge::GetInstance();
	if (editorBridge != nullptr)
	{
		editorBridge->Update(deltaTime);
	}

	CameraComponent* pCamera = nullptr;

	if (_scene != nullptr)
	{
		_scene->SimulatePhysic(deltaTime);

		_scene->Update(deltaTime);

		pCamera = _scene->GetRoot()->GetActor()->GetAllComponent<CameraComponent>()[0];
	}

	if (pCamera != nullptr)
	{
		glm::mat4 viewMatrix = glm::inverse(pCamera->GetActor()->GetComponent<SceneComponent>()->GetModelMatrix());
		glm::mat4 projectionMatrix = pCamera->GetProjectionMatrix();
		glm::vec4 viewport = glm::vec4(0, 1, 0, 1);

		DEBUG_LAYER::DebugLayer::GetInstance()->SetCameraMatrice(viewMatrix, projectionMatrix);
	}
	DEBUG_LAYER::DebugLayer::GetInstance()->Draw();

	ImGui::Render();
	
	RenderQueue* renderQueue = Renderer::GetInstance()->GetRenderQueue();
	pCamera->PushToRenderQueue(renderQueue);
	_scene->PushToRenderQueue(renderQueue);

	renderQueue->Execute();

	if (Renderer::GetInstance()->AcquireNextImageIndex() == true)
	{
		Renderer::GetInstance()->SwapBuffer();
	}

	return true;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool MyApplication::PostRun()
{
	return true;
}
