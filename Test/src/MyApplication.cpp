#include "MyApplication.h"

#include <HodEngine/Physic/src/Physic.h>
#include <HodEngine/Physic/src/Actor.h>
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
using namespace HOD::GAME;

MyApplication::MyApplication()
{

}

MyApplication::~MyApplication()
{

}

bool MyApplication::PreRun()
{
    HOD::DEBUG_LAYER::DebugLayer* pDebugLayer = HOD::DEBUG_LAYER::DebugLayer::CreateInstance();
    if (pDebugLayer->Init() == false)
    {
        return false;
    }

    HOD::PHYSIC::Physic* pPhysics = HOD::PHYSIC::Physic::CreateInstance();
    if (pPhysics->Init() == false)
    {
        return false;
    }

#ifdef NDEBUG
    bool bEnableValidationLayer = false;
#else
    bool bEnableValidationLayer = true;
#endif

    HOD::Renderer* pRenderer = new HOD::RendererVulkan();
	if (pRenderer->Init() == false)
	{
		return false;
	}

    if (pRenderer->Init(this, bEnableValidationLayer) == false)
    {
        return false;
    }

    HOD::GpuDevice* bestDevice = nullptr;

    if (HOD::GpuDeviceHelper::GetBestAvailableAndCompatibleDevice(&bestDevice) == false)
        return 1;

    if (pRenderer->BuildPipeline(bestDevice) == false)
        return 1;

    if (pRenderer->SetupImGui() == false)
        return 1;

	HOD::GAME::Game* pGame = HOD::GAME::Game::CreateInstance();
	if (pGame->Init() == false)
	{
		return false;
	}

    HOD::Mesh* sphereMesh = pRenderer->CreateMesh("Mesh/sphere.fbx");
    if (sphereMesh == nullptr)
        return 1;

    HOD::Mesh* wallMesh = pRenderer->CreateMesh("Mesh/wall.obj");
    if (wallMesh == nullptr)
        return 1;

    HOD::Mesh* boxMesh = pRenderer->CreateMesh("Mesh/SM_Box_1.fbx");
    if (boxMesh == nullptr)
        return 1;

    HOD::Texture* hdriTexture = pRenderer->CreateTexture("Texture/pond_bridge_night_2k.hdr");
    if (hdriTexture == nullptr)
        return 1;

    HOD::Texture* grayTexture = pRenderer->CreateTexture("Texture/black.png");
    if (grayTexture == nullptr)
        return 1;

    HOD::Texture* wallTexture = pRenderer->CreateTexture("Texture/brickwall.jpg");
    if (wallTexture == nullptr)
        return 1;

    HOD::Texture* wallTextureNormal = pRenderer->CreateTexture("Texture/brickwall_normal.jpg");
    if (wallTextureNormal == nullptr)
        return 1;

    HOD::Texture* wallTextureSpecular = pRenderer->CreateTexture("Texture/brickwall_Specular.png");
    if (wallTextureSpecular == nullptr)
        return 1;

    HOD::Texture* boxTexture = pRenderer->CreateTexture("Texture/T_Box_1_C.tga");
    if (boxTexture == nullptr)
        return 1;

    HOD::Texture* boxTextureNormal = pRenderer->CreateTexture("Texture/T_Box_1_N.tga");
    if (boxTextureNormal == nullptr)
        return 1;

    HOD::Texture* boxTextureMetalness = pRenderer->CreateTexture("Texture/T_Box_1_M.tga");
    if (boxTextureMetalness == nullptr)
        return 1;

    HOD::Texture* groundTexture = pRenderer->CreateTexture("Texture/T_Ground_Gravel_C.tga");
    if (groundTexture == nullptr)
        return 1;

    HOD::Texture* groundTextureNormal = pRenderer->CreateTexture("Texture/T_Ground_Gravel_N.tga");
    if (groundTextureNormal == nullptr)
        return 1;

    HOD::Texture* marbreTexture = pRenderer->CreateTexture("Texture/marble_01_diff_2k.jpg");
    if (marbreTexture == nullptr)
        return 1;

    HOD::Texture* marbreTextureNormal = pRenderer->CreateTexture("Texture/marble_01_nor_2k.jpg");
    if (marbreTextureNormal == nullptr)
        return 1;

    HOD::Texture* marbreTextureSpec = pRenderer->CreateTexture("Texture/marble_01_spec_2k.jpg");
    if (marbreTextureSpec == nullptr)
        return 1;



    //Texture* gunTexture = renderer->CreateTexture("Texture/brickwall.jpg");
    //if (gunTexture == nullptr)
    //    return 1;


    HOD::MaterialManager* materialManager = HOD::MaterialManager::getInstance();

    HOD::Material* materialHdri = materialManager->getMaterial("Hdri", false);
    if (materialHdri == nullptr)
        return 1;

    HOD::MaterialInstance* materialHdriInstance = pRenderer->CreateMaterialInstance(materialHdri);
    if (materialHdriInstance == nullptr)
        return 1;

    materialHdriInstance->SetTexture("textureSampler", *hdriTexture);
    materialHdriInstance->SetFloat("exposure", 1.0f);

    HOD::Material* materialLit = materialManager->getMaterial("Lit");
    if (materialLit == nullptr)
        return 1;

    HOD::MaterialInstance* materialLitInstance = pRenderer->CreateMaterialInstance(materialLit);
    if (materialLitInstance == nullptr)
        return 1;

    materialLitInstance->SetTexture("textureSampler", *wallTexture);

    HOD::Material* materialLitSpecular = materialManager->getMaterial("LitSpecular");
    if (materialLitSpecular == nullptr)
        return 1;

    HOD::MaterialInstance* materialLitSpecularInstance = pRenderer->CreateMaterialInstance(materialLitSpecular);
    if (materialLitSpecularInstance == nullptr)
        return 1;

    materialLitSpecularInstance->SetTexture("textureSampler", *wallTexture);
    materialLitSpecularInstance->SetTexture("specularTextureSampler", *wallTextureSpecular);
    //materialLitSpecularInstance->SetTexture("normalTextureSampler", *wallTextureNormal);
    materialLitSpecularInstance->SetFloat("matUbo.specularStrength", 1.5f);
    materialLitSpecularInstance->SetFloat("matUbo.shininess", 16.0f);

    HOD::Material* materialLitSpecularNormal = materialManager->getMaterial("LitSpecularNormal");
    if (materialLitSpecularNormal == nullptr)
        return 1;

    HOD::MaterialInstance* materialLitSpecularNormalInstance = pRenderer->CreateMaterialInstance(materialLitSpecularNormal);
    if (materialLitSpecularNormalInstance == nullptr)
        return 1;

    materialLitSpecularNormalInstance->SetTexture("textureSampler", *wallTexture);
    materialLitSpecularNormalInstance->SetTexture("specularTextureSampler", *wallTextureSpecular);
    materialLitSpecularNormalInstance->SetTexture("normalTextureSampler", *wallTextureNormal);
    materialLitSpecularNormalInstance->SetFloat("matUbo.specularStrength", 1.5f);
    materialLitSpecularNormalInstance->SetFloat("matUbo.shininess", 16.0f);
    materialLitSpecularNormalInstance->SetVec4("matUbo.tilingOffset", glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));

    HOD::Material* materialUnlit = materialManager->getMaterial("UnlitColor");
    if (materialUnlit == nullptr)
        return 1;

    HOD::MaterialInstance* materialUnlitInstance = pRenderer->CreateMaterialInstance(materialUnlit);
    if (materialUnlitInstance == nullptr)
        return 1;

    materialUnlitInstance->SetVec4("matUbo.color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    HOD::Material* materialUnlitTexture = materialManager->getMaterial("UnlitTexture");
    if (materialUnlitTexture == nullptr)
        return 1;

    HOD::MaterialInstance* materialUnlitTextureInstance = pRenderer->CreateMaterialInstance(materialUnlitTexture);
    if (materialUnlitTextureInstance == nullptr)
        return 1;

    materialUnlitTextureInstance->SetTexture("textureSampler", *wallTexture);

    HOD::MaterialInstance* boxMaterialLitSpecularNormalInstance = pRenderer->CreateMaterialInstance(materialLitSpecularNormal);
    if (boxMaterialLitSpecularNormalInstance == nullptr)
        return 1;

    boxMaterialLitSpecularNormalInstance->SetTexture("textureSampler", *boxTexture);
    boxMaterialLitSpecularNormalInstance->SetTexture("specularTextureSampler", *boxTextureMetalness);
    boxMaterialLitSpecularNormalInstance->SetTexture("normalTextureSampler", *boxTextureNormal);
    boxMaterialLitSpecularNormalInstance->SetFloat("matUbo.specularStrength", 1.5f);
    boxMaterialLitSpecularNormalInstance->SetFloat("matUbo.shininess", 16.0f);
    boxMaterialLitSpecularNormalInstance->SetVec4("matUbo.tilingOffset", glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));

    HOD::MaterialInstance* groundMaterialLitSpecularNormalInstance = pRenderer->CreateMaterialInstance(materialLitSpecularNormal);
    if (groundMaterialLitSpecularNormalInstance == nullptr)
        return 1;

    groundMaterialLitSpecularNormalInstance->SetTexture("textureSampler", *groundTexture);
    groundMaterialLitSpecularNormalInstance->SetTexture("specularTextureSampler", *grayTexture);
    groundMaterialLitSpecularNormalInstance->SetTexture("normalTextureSampler", *groundTextureNormal);
    groundMaterialLitSpecularNormalInstance->SetFloat("matUbo.specularStrength", 1.5f);
    groundMaterialLitSpecularNormalInstance->SetFloat("matUbo.shininess", 16.0f);
    groundMaterialLitSpecularNormalInstance->SetVec4("matUbo.tilingOffset", glm::vec4(4.0f, 4.0f, 0.0f, 0.0f));

    HOD::MaterialInstance* marbreMaterialLitSpecularNormalInstance = pRenderer->CreateMaterialInstance(materialLitSpecularNormal);
    if (marbreMaterialLitSpecularNormalInstance == nullptr)
        return 1;

    marbreMaterialLitSpecularNormalInstance->SetTexture("textureSampler", *marbreTexture);
    marbreMaterialLitSpecularNormalInstance->SetTexture("specularTextureSampler", *marbreTextureSpec);
    marbreMaterialLitSpecularNormalInstance->SetTexture("normalTextureSampler", *marbreTextureNormal);
    marbreMaterialLitSpecularNormalInstance->SetFloat("matUbo.specularStrength", 1.5f);
    marbreMaterialLitSpecularNormalInstance->SetFloat("matUbo.shininess", 16.0f);
    marbreMaterialLitSpecularNormalInstance->SetVec4("matUbo.tilingOffset", glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));

    scene = pGame->CreateScene();

    FreeCam* freeCam = scene->spawnActor<FreeCam>("FreeCam");
    {
        HOD::GAME::SceneComponent* sceneComponent = freeCam->getComponent<HOD::GAME::SceneComponent>();

        sceneComponent->setPosition(glm::vec3(0.0f, 10.0f, 9.0f));
        sceneComponent->lookAt(sceneComponent->getPosition(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        sceneComponent->setParent(scene->getRoot());

        //freeCam->setupInputListener(application.GetInputListenner());

        HOD::GAME::SpotLightComponent* spotLightComponent = freeCam->addComponent<HOD::GAME::SpotLightComponent>();
        spotLightComponent->_data.radius = 15.0f;
        spotLightComponent->_data.color = CORE::Color(1.0f, 1.0f, 1.0f, 1.0f);
        spotLightComponent->_data.intensity = 1.0f;
        spotLightComponent->_data.outer = 10.0f;

        freeCam->getComponent<GAME::CameraComponent>()->SetHdriMaterial(materialHdriInstance, hdriTexture);
    }

    GAME::Actor* wall1 = scene->spawnActor<GAME::Actor>("wall1");
    {
        HOD::GAME::SceneComponent* sceneComponent = wall1->addComponent<HOD::GAME::SceneComponent>();
        sceneComponent->position = glm::vec3(-3.5f, 3.0f, 0.0f);
        sceneComponent->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
        sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 3.0f;
        sceneComponent->setParent(scene->getRoot());

        HOD::GAME::StaticMeshComponent* staticMeshComponent = wall1->addComponent<HOD::GAME::StaticMeshComponent>();
        staticMeshComponent->setMesh(wallMesh);
        staticMeshComponent->setMaterialInstance(materialLitSpecularNormalInstance);
        staticMeshComponent->EnableDebugTangent(true);

        HOD::GAME::ColliderComponent* colliderComponent = wall1->addComponent<HOD::GAME::ColliderComponent>();
        colliderComponent->SetShape(PHYSIC::SHAPE::BOX);
    }

    Actor* wall2 = scene->spawnActor<Actor>("wall2");
    {
        HOD::GAME::SceneComponent* sceneComponent = wall2->addComponent<HOD::GAME::SceneComponent>();
        sceneComponent->position = glm::vec3(3.5f, 3.0f, 0.0f);
        sceneComponent->setRotation(glm::vec3(0.0f, -90.0f, 0.0f));
        sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 3.0f;
        sceneComponent->setParent(scene->getRoot());

        HOD::GAME::StaticMeshComponent* staticMeshComponent = wall2->addComponent<HOD::GAME::StaticMeshComponent>();
        staticMeshComponent->setMesh(wallMesh);
        staticMeshComponent->setMaterialInstance(materialLitSpecularNormalInstance);
        staticMeshComponent->EnableDebugTangent(true);

        HOD::GAME::ColliderComponent* colliderComponent = wall2->addComponent<HOD::GAME::ColliderComponent>();
        colliderComponent->SetShape(PHYSIC::SHAPE::BOX);
    }

    Actor* sphereActor = scene->spawnActor<FlyingPointLight>("FlyingPointLight");
    {
        HOD::GAME::SceneComponent* sceneComponent = sphereActor->getComponent<HOD::GAME::SceneComponent>();
        sceneComponent->position = glm::vec3(0.0f, 3.0f, 0.0f);
        sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f;
        sceneComponent->setParent(scene->getRoot());

        HOD::GAME::StaticMeshComponent* staticMeshComponent = sphereActor->addComponent<HOD::GAME::StaticMeshComponent>();
        staticMeshComponent->setMesh(sphereMesh);
        staticMeshComponent->setMaterialInstance(pRenderer->CreateMaterialInstance(materialUnlit));

        HOD::GAME::ColliderComponent* colliderComponent = sphereActor->addComponent<HOD::GAME::ColliderComponent>();
        colliderComponent->SetShape(PHYSIC::SHAPE::SPHERE);

        HOD::GAME::PointLightComponent* pointLightComponent = sphereActor->addComponent<HOD::GAME::PointLightComponent>();
        pointLightComponent->_data.color = CORE::Color(1.0f, 1.0f, 1.0f, 1.0f);
        pointLightComponent->_data.intensity = 1.0f;
        pointLightComponent->_data.range = 2.5f;

        sphereActor->start();
    }

    Actor* dirLight = scene->spawnActor<Actor>("dirLight");
    {
        HOD::GAME::SceneComponent* sceneComponent = dirLight->addComponent<HOD::GAME::SceneComponent>();
        sceneComponent->setPosition(glm::vec3(-10.0f, 5.0f, -10.0f));
        sceneComponent->rotate(45.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f;
        sceneComponent->setParent(scene->getRoot());

        HOD::GAME::StaticMeshComponent* staticMeshComponent = dirLight->addComponent<HOD::GAME::StaticMeshComponent>();
        staticMeshComponent->setMesh(sphereMesh);
        staticMeshComponent->setMaterialInstance(pRenderer->CreateMaterialInstance(materialUnlit));

        HOD::GAME::ColliderComponent* colliderComponent = dirLight->addComponent<HOD::GAME::ColliderComponent>();
        colliderComponent->SetShape(PHYSIC::SHAPE::SPHERE);

        HOD::GAME::DirLightComponent* dirLightComponent = dirLight->addComponent<HOD::GAME::DirLightComponent>();
        dirLightComponent->_data.color = CORE::Color(1.0f, 1.0f, 1.0f, 1.0f);
        dirLightComponent->_data.intensity = 1.0f;

        dirLight->start();
    }

    Actor* box = scene->spawnActor<Actor>("box");
    {
        HOD::GAME::SceneComponent* sceneComponent = box->addComponent<HOD::GAME::SceneComponent>();
        sceneComponent->position = glm::vec3(0.0f, 0.0f, -5.0f);
        sceneComponent->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
        sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 0.03f;
        sceneComponent->setParent(scene->getRoot());

        HOD::GAME::StaticMeshComponent* staticMeshComponent = box->addComponent<HOD::GAME::StaticMeshComponent>();
        staticMeshComponent->setMesh(boxMesh);
        staticMeshComponent->setMaterialInstance(boxMaterialLitSpecularNormalInstance);
        //staticMeshComponent->EnableDebugTangent(true);

        HOD::GAME::ColliderComponent* colliderComponent = box->addComponent<HOD::GAME::ColliderComponent>();
        colliderComponent->SetShape(PHYSIC::SHAPE::BOX);
    }

    Actor* ground = scene->spawnActor<Actor>("ground");
    {
        HOD::GAME::SceneComponent* sceneComponent = ground->addComponent<HOD::GAME::SceneComponent>();
        sceneComponent->position = glm::vec3(0.0f, 0.0f, 0.0f);
        sceneComponent->setRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
        sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 10.0f;
        sceneComponent->setParent(scene->getRoot());

        HOD::GAME::StaticMeshComponent* staticMeshComponent = ground->addComponent<HOD::GAME::StaticMeshComponent>();
        staticMeshComponent->setMesh(wallMesh);
        staticMeshComponent->setMaterialInstance(groundMaterialLitSpecularNormalInstance);
        //staticMeshComponent->EnableDebugTangent(true);

        HOD::GAME::ColliderComponent* colliderComponent = ground->addComponent<HOD::GAME::ColliderComponent>();
        colliderComponent->SetShape(PHYSIC::SHAPE::BOX);
    }

    return true;
}

bool MyApplication::Loop(float deltaTime)
{
    scene->simulatePhysic(deltaTime);

    scene->update(deltaTime);

	CameraComponent* pCamera = scene->getRoot()->GetActor()->getAllComponent<CameraComponent>()[0];

	glm::mat4 viewMatrix = glm::inverse(pCamera->GetActor()->getComponent<SceneComponent>()->getModelMatrix());
	glm::mat4 projectionMatrix = pCamera->getProjectionMatrix();

	DEBUG_LAYER::DebugLayer::GetInstance()->SetCameraMatrice(viewMatrix, projectionMatrix);

    DEBUG_LAYER::DebugLayer::GetInstance()->Draw();

    ImGui::Render();

	pCamera->render(*scene);

    if (Renderer::GetInstance()->AcquireNextImageIndex() == true)
    {
        Renderer::GetInstance()->SwapBuffer();
    }

    return true;
}

bool MyApplication::PostRun()
{
    return true;
}
