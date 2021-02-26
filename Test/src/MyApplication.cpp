#include "MyApplication.h"

#include <HodEngine/Physics/src/Physics.h>
#include <HodEngine/Physics/src/Actor.h>
#include <HodEngine/Renderer/src/Renderer.h>
#include <HodEngine/Renderer/src/Vulkan/RendererVulkan.h>
#include <HodEngine/Renderer/src/GpuDeviceHelper.h>
#include <HodEngine/Renderer/src/MaterialManager.h>
#include <HodEngine/Renderer/src/Material.h>
#include <HodEngine/Renderer/src/MaterialInstance.h>
#include <HodEngine/Renderer/src/Texture.h>
#include <HodEngine/Renderer/src/Mesh.h>
#include <HodEngine/Game/src/Debug/ActorDebugWindow.h>
#include <HodEngine/Game/src/Game.h>
#include <HodEngine/Game/src/Scene.h>
#include <HodEngine/Game/src/Actor.h>
#include <HodEngine/Game/src/Component/SceneComponent.h>
#include <HodEngine/Game/src/Component/CameraComponent.h>
#include <HodEngine/Game/src/Component/ColliderComponent.h>
#include <HodEngine/Game/src/Component/StaticMeshComponent.h>
#include <HodEngine/Game/src/Component/Light/PointLightComponent.h>
#include <HodEngine/Game/src/Component/Light/DirLightComponent.h>
#include <HodEngine/Game/src/Component/Light/SpotLightComponent.h>

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

	Mesh* sphereMesh = pRenderer->CreateMesh("Mesh/sphere.fbx");
	if (sphereMesh == nullptr)
	{
		return 1;
	}

	Mesh* wallMesh = pRenderer->CreateMesh("Mesh/wall.obj");
	if (wallMesh == nullptr)
	{
		return 1;
	}

	Mesh* boxMesh = pRenderer->CreateMesh("Mesh/SM_Box_1.fbx");
	if (boxMesh == nullptr)
	{
		return 1;
	}

	Texture* hdriTexture = pRenderer->CreateTexture("Texture/pond_bridge_night_2k.hdr");
	if (hdriTexture == nullptr)
	{
		return 1;
	}

	Texture* grayTexture = pRenderer->CreateTexture("Texture/black.png");
	if (grayTexture == nullptr)
	{
		return 1;
	}

	Texture* wallTexture = pRenderer->CreateTexture("Texture/brickwall.jpg");
	if (wallTexture == nullptr)
	{
		return 1;
	}

	Texture* wallTextureNormal = pRenderer->CreateTexture("Texture/brickwall_normal.jpg");
	if (wallTextureNormal == nullptr)
	{
		return 1;
	}

	Texture* wallTextureSpecular = pRenderer->CreateTexture("Texture/brickwall_Specular.png");
	if (wallTextureSpecular == nullptr)
	{
		return 1;
	}

	Texture* boxTexture = pRenderer->CreateTexture("Texture/T_Box_1_C.tga");
	if (boxTexture == nullptr)
	{
		return 1;
	}

	Texture* boxTextureNormal = pRenderer->CreateTexture("Texture/T_Box_1_N.tga");
	if (boxTextureNormal == nullptr)
	{
		return 1;
	}

	Texture* boxTextureMetalness = pRenderer->CreateTexture("Texture/T_Box_1_M.tga");
	if (boxTextureMetalness == nullptr)
	{
		return 1;
	}

	Texture* groundTexture = pRenderer->CreateTexture("Texture/T_Ground_Gravel_C.tga");
	if (groundTexture == nullptr)
	{
		return 1;
	}

	Texture* groundTextureNormal = pRenderer->CreateTexture("Texture/T_Ground_Gravel_N.tga");
	if (groundTextureNormal == nullptr)
	{
		return 1;
	}

	Texture* marbreTexture = pRenderer->CreateTexture("Texture/marble_01_diff_2k.jpg");
	if (marbreTexture == nullptr)
	{
		return 1;
	}

	Texture* marbreTextureNormal = pRenderer->CreateTexture("Texture/marble_01_nor_2k.jpg");
	if (marbreTextureNormal == nullptr)
	{
		return 1;
	}

	Texture* marbreTextureSpec = pRenderer->CreateTexture("Texture/marble_01_spec_2k.jpg");
	if (marbreTextureSpec == nullptr)
	{
		return 1;
	}

	//Texture* gunTexture = renderer->CreateTexture("Texture/brickwall.jpg");
	//if (gunTexture == nullptr)
	//{
	//	return 1;
	//}


	MaterialManager* materialManager = MaterialManager::GetInstance();

	Material* materialHdri = materialManager->GetMaterial("Hdri", false);
	if (materialHdri == nullptr)
	{
		return 1;
	}

	MaterialInstance* materialHdriInstance = pRenderer->CreateMaterialInstance(materialHdri);
	if (materialHdriInstance == nullptr)
	{
		return 1;
	}

	materialHdriInstance->SetTexture("textureSampler", *hdriTexture);
	materialHdriInstance->SetFloat("exposure", 1.0f);

	Material* materialLit = materialManager->GetMaterial("Lit");
	if (materialLit == nullptr)
	{
		return 1;
	}

	MaterialInstance* materialLitInstance = pRenderer->CreateMaterialInstance(materialLit);
	if (materialLitInstance == nullptr)
	{
		return 1;
	}

	materialLitInstance->SetTexture("textureSampler", *wallTexture);

	Material* materialLitSpecular = materialManager->GetMaterial("LitSpecular");
	if (materialLitSpecular == nullptr)
	{
		return 1;
	}

	MaterialInstance* materialLitSpecularInstance = pRenderer->CreateMaterialInstance(materialLitSpecular);
	if (materialLitSpecularInstance == nullptr)
	{
		return 1;
	}

	materialLitSpecularInstance->SetTexture("textureSampler", *wallTexture);
	materialLitSpecularInstance->SetTexture("specularTextureSampler", *wallTextureSpecular);
	//materialLitSpecularInstance->SetTexture("normalTextureSampler", *wallTextureNormal);
	materialLitSpecularInstance->SetFloat("matUbo.specularStrength", 1.5f);
	materialLitSpecularInstance->SetFloat("matUbo.shininess", 16.0f);

	Material* materialLitSpecularNormal = materialManager->GetMaterial("LitSpecularNormal");
	if (materialLitSpecularNormal == nullptr)
	{
		return 1;
	}

	MaterialInstance* materialLitSpecularNormalInstance = pRenderer->CreateMaterialInstance(materialLitSpecularNormal);
	if (materialLitSpecularNormalInstance == nullptr)
	{
		return 1;
	}

	materialLitSpecularNormalInstance->SetTexture("textureSampler", *wallTexture);
	materialLitSpecularNormalInstance->SetTexture("specularTextureSampler", *wallTextureSpecular);
	materialLitSpecularNormalInstance->SetTexture("normalTextureSampler", *wallTextureNormal);
	materialLitSpecularNormalInstance->SetFloat("matUbo.specularStrength", 1.5f);
	materialLitSpecularNormalInstance->SetFloat("matUbo.shininess", 16.0f);
	materialLitSpecularNormalInstance->SetVec4("matUbo.tilingOffset", glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));

	Material* materialUnlit = materialManager->GetMaterial("UnlitColor");
	if (materialUnlit == nullptr)
	{
		return 1;
	}

	MaterialInstance* materialUnlitInstance = pRenderer->CreateMaterialInstance(materialUnlit);
	if (materialUnlitInstance == nullptr)
	{
		return 1;
	}

	materialUnlitInstance->SetVec4("matUbo.color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	Material* materialUnlitTexture = materialManager->GetMaterial("UnlitTexture");
	if (materialUnlitTexture == nullptr)
	{
		return 1;
	}

	MaterialInstance* materialUnlitTextureInstance = pRenderer->CreateMaterialInstance(materialUnlitTexture);
	if (materialUnlitTextureInstance == nullptr)
	{
		return 1;
	}

	materialUnlitTextureInstance->SetTexture("textureSampler", *wallTexture);

	MaterialInstance* boxMaterialLitSpecularNormalInstance = pRenderer->CreateMaterialInstance(materialLitSpecularNormal);
	if (boxMaterialLitSpecularNormalInstance == nullptr)
	{
		return 1;
	}

	boxMaterialLitSpecularNormalInstance->SetTexture("textureSampler", *boxTexture);
	boxMaterialLitSpecularNormalInstance->SetTexture("specularTextureSampler", *boxTextureMetalness);
	boxMaterialLitSpecularNormalInstance->SetTexture("normalTextureSampler", *boxTextureNormal);
	boxMaterialLitSpecularNormalInstance->SetFloat("matUbo.specularStrength", 1.5f);
	boxMaterialLitSpecularNormalInstance->SetFloat("matUbo.shininess", 16.0f);
	boxMaterialLitSpecularNormalInstance->SetVec4("matUbo.tilingOffset", glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));

	MaterialInstance* groundMaterialLitSpecularNormalInstance = pRenderer->CreateMaterialInstance(materialLitSpecularNormal);
	if (groundMaterialLitSpecularNormalInstance == nullptr)
	{
		return 1;
	}

	groundMaterialLitSpecularNormalInstance->SetTexture("textureSampler", *groundTexture);
	groundMaterialLitSpecularNormalInstance->SetTexture("specularTextureSampler", *grayTexture);
	groundMaterialLitSpecularNormalInstance->SetTexture("normalTextureSampler", *groundTextureNormal);
	groundMaterialLitSpecularNormalInstance->SetFloat("matUbo.specularStrength", 1.5f);
	groundMaterialLitSpecularNormalInstance->SetFloat("matUbo.shininess", 16.0f);
	groundMaterialLitSpecularNormalInstance->SetVec4("matUbo.tilingOffset", glm::vec4(4.0f, 4.0f, 0.0f, 0.0f));

	MaterialInstance* marbreMaterialLitSpecularNormalInstance = pRenderer->CreateMaterialInstance(materialLitSpecularNormal);
	if (marbreMaterialLitSpecularNormalInstance == nullptr)
		return 1;

	marbreMaterialLitSpecularNormalInstance->SetTexture("textureSampler", *marbreTexture);
	marbreMaterialLitSpecularNormalInstance->SetTexture("specularTextureSampler", *marbreTextureSpec);
	marbreMaterialLitSpecularNormalInstance->SetTexture("normalTextureSampler", *marbreTextureNormal);
	marbreMaterialLitSpecularNormalInstance->SetFloat("matUbo.specularStrength", 1.5f);
	marbreMaterialLitSpecularNormalInstance->SetFloat("matUbo.shininess", 16.0f);
	marbreMaterialLitSpecularNormalInstance->SetVec4("matUbo.tilingOffset", glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));

	_scene = pGame->CreateScene();

	FreeCam* freeCam = _scene->SpawnActor<FreeCam>("FreeCam");
	{
		SceneComponent* sceneComponent = freeCam->GetComponent<SceneComponent>();

		sceneComponent->SetPosition(glm::vec3(0.0f, 10.0f, 9.0f));
		sceneComponent->LookAt(sceneComponent->GetPosition(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		sceneComponent->SetParent(_scene->GetRoot());

		//freeCam->setupInputListener(application.GetInputListenner());

		SpotLightComponent* spotLightComponent = freeCam->AddComponent<SpotLightComponent>();
		spotLightComponent->_data.radius = 15.0f;
		spotLightComponent->_data.color = CORE::Color(1.0f, 1.0f, 1.0f, 1.0f);
		spotLightComponent->_data.intensity = 1.0f;
		spotLightComponent->_data.outer = 10.0f;

		freeCam->GetComponent<GAME::CameraComponent>()->SetHdriMaterial(materialHdriInstance, hdriTexture);
	}

	Actor* wall1 = _scene->SpawnActor<Actor>("wall1");
	{
		SceneComponent* sceneComponent = wall1->AddComponent<SceneComponent>();
		sceneComponent->_position = glm::vec3(-3.5f, 3.0f, 0.0f);
		sceneComponent->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
		sceneComponent->_scale = glm::vec3(1.0f, 1.0f, 1.0f) * 3.0f;
		sceneComponent->SetParent(_scene->GetRoot());

		StaticMeshComponent* staticMeshComponent = wall1->AddComponent<StaticMeshComponent>();
		staticMeshComponent->SetMesh(wallMesh);
		staticMeshComponent->SetMaterialInstance(materialLitSpecularNormalInstance);
		staticMeshComponent->EnableDebugTangent(true);

		ColliderComponent* colliderComponent = wall1->AddComponent<ColliderComponent>();
		colliderComponent->SetShape(PHYSICS::SHAPE::BOX);
	}

	Actor* wall2 = _scene->SpawnActor<Actor>("wall2");
	{
		SceneComponent* sceneComponent = wall2->AddComponent<SceneComponent>();
		sceneComponent->_position = glm::vec3(3.5f, 3.0f, 0.0f);
		sceneComponent->SetRotation(glm::vec3(0.0f, -90.0f, 0.0f));
		sceneComponent->_scale = glm::vec3(1.0f, 1.0f, 1.0f) * 3.0f;
		sceneComponent->SetParent(_scene->GetRoot());

		StaticMeshComponent* staticMeshComponent = wall2->AddComponent<StaticMeshComponent>();
		staticMeshComponent->SetMesh(wallMesh);
		staticMeshComponent->SetMaterialInstance(materialLitSpecularNormalInstance);
		staticMeshComponent->EnableDebugTangent(true);

		ColliderComponent* colliderComponent = wall2->AddComponent<ColliderComponent>();
		colliderComponent->SetShape(PHYSICS::SHAPE::BOX);
	}

	Actor* sphereActor = _scene->SpawnActor<FlyingPointLight>("FlyingPointLight");
	{
		SceneComponent* sceneComponent = sphereActor->GetComponent<SceneComponent>();
		sceneComponent->_position = glm::vec3(0.0f, 3.0f, 0.0f);
		sceneComponent->_scale = glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f;
		sceneComponent->SetParent(_scene->GetRoot());

		StaticMeshComponent* staticMeshComponent = sphereActor->AddComponent<StaticMeshComponent>();
		staticMeshComponent->SetMesh(sphereMesh);
		staticMeshComponent->SetMaterialInstance(pRenderer->CreateMaterialInstance(materialUnlit));

		ColliderComponent* colliderComponent = sphereActor->AddComponent<ColliderComponent>();
		colliderComponent->SetShape(PHYSICS::SHAPE::SPHERE);

		PointLightComponent* pointLightComponent = sphereActor->AddComponent<PointLightComponent>();
		pointLightComponent->_data.color = CORE::Color(1.0f, 1.0f, 1.0f, 1.0f);
		pointLightComponent->_data.intensity = 1.0f;
		pointLightComponent->_data.range = 2.5f;

		sphereActor->Start();
	}

	Actor* dirLight = _scene->SpawnActor<Actor>("dirLight");
	{
		SceneComponent* sceneComponent = dirLight->AddComponent<SceneComponent>();
		sceneComponent->SetPosition(glm::vec3(-10.0f, 5.0f, -10.0f));
		sceneComponent->Rotate(45.0f, glm::vec3(1.0f, 1.0f, 1.0f));
		sceneComponent->_scale = glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f;
		sceneComponent->SetParent(_scene->GetRoot());

		StaticMeshComponent* staticMeshComponent = dirLight->AddComponent<StaticMeshComponent>();
		staticMeshComponent->SetMesh(sphereMesh);
		staticMeshComponent->SetMaterialInstance(pRenderer->CreateMaterialInstance(materialUnlit));

		ColliderComponent* colliderComponent = dirLight->AddComponent<ColliderComponent>();
		colliderComponent->SetShape(PHYSICS::SHAPE::SPHERE);

		DirLightComponent* dirLightComponent = dirLight->AddComponent<DirLightComponent>();
		dirLightComponent->_data.color = CORE::Color(1.0f, 1.0f, 1.0f, 1.0f);
		dirLightComponent->_data.intensity = 1.0f;

		dirLight->Start();
	}

	Actor* box = _scene->SpawnActor<Actor>("box");
	{
		SceneComponent* sceneComponent = box->AddComponent<SceneComponent>();
		sceneComponent->_position = glm::vec3(0.0f, 0.0f, -5.0f);
		sceneComponent->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
		sceneComponent->_scale = glm::vec3(1.0f, 1.0f, 1.0f) * 0.03f;
		sceneComponent->SetParent(_scene->GetRoot());

		StaticMeshComponent* staticMeshComponent = box->AddComponent<StaticMeshComponent>();
		staticMeshComponent->SetMesh(boxMesh);
		staticMeshComponent->SetMaterialInstance(boxMaterialLitSpecularNormalInstance);
		//staticMeshComponent->EnableDebugTangent(true);

		ColliderComponent* colliderComponent = box->AddComponent<ColliderComponent>();
		colliderComponent->SetShape(PHYSICS::SHAPE::BOX);
	}

	Actor* ground = _scene->SpawnActor<Actor>("ground");
	{
		SceneComponent* sceneComponent = ground->AddComponent<SceneComponent>();
		sceneComponent->_position = glm::vec3(0.0f, 0.0f, 0.0f);
		sceneComponent->SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
		sceneComponent->_scale = glm::vec3(1.0f, 1.0f, 1.0f) * 10.0f;
		sceneComponent->SetParent(_scene->GetRoot());

		StaticMeshComponent* staticMeshComponent = ground->AddComponent<StaticMeshComponent>();
		staticMeshComponent->SetMesh(wallMesh);
		staticMeshComponent->SetMaterialInstance(groundMaterialLitSpecularNormalInstance);
		//staticMeshComponent->EnableDebugTangent(true);

		ColliderComponent* colliderComponent = ground->AddComponent<ColliderComponent>();
		colliderComponent->SetShape(PHYSICS::SHAPE::BOX);
	}

	return true;
}

//-----------------------------------------------------------------------------
//! @brief		
//-----------------------------------------------------------------------------
bool MyApplication::Loop(float deltaTime)
{
	_scene->SimulatePhysic(deltaTime);

	_scene->Update(deltaTime);

	CameraComponent* pCamera = _scene->GetRoot()->GetActor()->GetAllComponent<CameraComponent>()[0];

	glm::mat4 viewMatrix = glm::inverse(pCamera->GetActor()->GetComponent<SceneComponent>()->GetModelMatrix());
	glm::mat4 projectionMatrix = pCamera->GetProjectionMatrix();

	DEBUG_LAYER::DebugLayer::GetInstance()->SetCameraMatrice(viewMatrix, projectionMatrix);

	DEBUG_LAYER::DebugLayer::GetInstance()->Draw();

	ImGui::Render();

	pCamera->Render(*_scene);

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
