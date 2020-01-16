#include <iostream>

#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_vulkan.h>

#include <algorithm>
#include <cstring>
#include <vector>

#include "../Renderer/Mesh.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Material.h"
#include "../Renderer/MaterialManager.h"
#include "../Renderer/MaterialInstance.h"

#include "../Scene.hpp"

#include "../Actor.h"
#include "../Component/SceneComponent.h"
#include "../Component/StaticMeshComponent.h"
#include "../Component/CameraComponent.h"
#include "../Component/Light/DirLightComponent.h"
#include "../Component/Light/PointLightComponent.h"
#include "../Component/Light/SpotLightComponent.h"
#include "../Component/ColliderComponent.h"

#include "../InputListener.h"

#include "FreeCam.h"
#include "FlyingPointLight.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/gtc/matrix_transform.hpp"

#include "../Allocator.h"
#include "../Error.h"

//#include <AntTweakBar.h>

#include <tchar.h>

#include "../Renderer/GpuDeviceHelper.h"
#include "../Renderer/GraphicsSettings.h"
#include "../ScreenHelper.h"
#include "../Application.h"

#include "../Renderer/Renderer.h"

InputListener inputListener;

int main(int argc, char** argv)
{
    int ret = 0;
    {
        Application application;

        if (application.Init() == false)
            return false;

        int selectedMonitor = 0;

        // Debug
        if (ScreenHelper::GetMonitorCount() > 1)
            selectedMonitor = 1;

        ScreenHelper::Resolution nativeResolution;

        if (ScreenHelper::GetNativeResoltion(&nativeResolution, selectedMonitor) == false)
            return 1;

        GraphicsSettings graphicsSettings;
        graphicsSettings.api = GraphicsSettings::API::Vulkan;
        graphicsSettings.monitor = selectedMonitor;
        graphicsSettings.width = nativeResolution.width;
        graphicsSettings.height = nativeResolution.height;
        graphicsSettings.fullscreenType = GraphicsSettings::FullscreenType::Window;

        if (application.CreateWindowAndContext("HodEngine", graphicsSettings) == false)
            return 1;

        GpuDevice* bestDevice = nullptr;

        if (GpuDeviceHelper::GetBestAvailableAndCompatibleDevice(&bestDevice) == false)
            return 1;

        Renderer* renderer = Renderer::GetInstance();

        if (renderer->BuildPipeline(bestDevice) == false)
            return 1;

        Mesh* sphereMesh = renderer->CreateMesh("Mesh/sphere.fbx");
        if (sphereMesh == nullptr)
            return 1;

        Mesh* wallMesh = renderer->CreateMesh("Mesh/wall.obj");
        if (wallMesh == nullptr)
            return 1;
        
        Mesh* boxMesh = renderer->CreateMesh("Mesh/SM_Box_1.fbx");
        if (boxMesh == nullptr)
            return 1;
            
        Texture* hdriTexture = renderer->CreateTexture("Texture/pond_bridge_night_2k.hdr");
        if (hdriTexture == nullptr)
            return 1;

        Texture* grayTexture = renderer->CreateTexture("Texture/black.png");
        if (grayTexture == nullptr)
            return 1;

        Texture* wallTexture = renderer->CreateTexture("Texture/brickwall.jpg");
        if (wallTexture == nullptr)
            return 1;

        Texture* wallTextureNormal = renderer->CreateTexture("Texture/brickwall_normal.jpg");
        if (wallTextureNormal == nullptr)
            return 1;

        Texture* wallTextureSpecular = renderer->CreateTexture("Texture/brickwall_Specular.png");
        if (wallTextureSpecular == nullptr)
            return 1;
        
        Texture* boxTexture = renderer->CreateTexture("Texture/T_Box_1_C.tga");
        if (boxTexture == nullptr)
            return 1;

        Texture* boxTextureNormal = renderer->CreateTexture("Texture/T_Box_1_N.tga");
        if (boxTextureNormal == nullptr)
            return 1;

        Texture* boxTextureMetalness = renderer->CreateTexture("Texture/T_Box_1_M.tga");
        if (boxTextureMetalness == nullptr)
            return 1;

        Texture* groundTexture = renderer->CreateTexture("Texture/T_Ground_Gravel_C.tga");
        if (groundTexture == nullptr)
            return 1;

        Texture* groundTextureNormal = renderer->CreateTexture("Texture/T_Ground_Gravel_N.tga");
        if (groundTextureNormal == nullptr)
            return 1;

        Texture* marbreTexture = renderer->CreateTexture("Texture/marble_01_diff_2k.jpg");
        if (marbreTexture == nullptr)
            return 1;

        Texture* marbreTextureNormal = renderer->CreateTexture("Texture/marble_01_nor_2k.jpg");
        if (marbreTextureNormal == nullptr)
            return 1;

        Texture* marbreTextureSpec = renderer->CreateTexture("Texture/marble_01_spec_2k.jpg");
        if (marbreTextureSpec == nullptr)
            return 1;


        /*
            Texture* gunTexture = renderer->CreateTexture("Texture/brickwall.jpg");
            if (gunTexture == nullptr)
                return 1;
        */

        MaterialManager* materialManager = MaterialManager::getInstance();
        
        Material* materialHdri = materialManager->getMaterial("Hdri", false);
        if (materialHdri == nullptr)
            return 1;
        
        MaterialInstance* materialHdriInstance = renderer->CreateMaterialInstance(materialHdri);
        if (materialHdriInstance == nullptr)
            return 1;

        materialHdriInstance->SetTexture("textureSampler", *hdriTexture);
        materialHdriInstance->SetFloat("exposure", 1.0f);

        Material* materialLit = materialManager->getMaterial("Lit");
        if (materialLit == nullptr)
            return 1;

        MaterialInstance* materialLitInstance = renderer->CreateMaterialInstance(materialLit);
        if (materialLitInstance == nullptr)
            return 1;

        materialLitInstance->SetTexture("textureSampler", *wallTexture);

        Material* materialLitSpecular = materialManager->getMaterial("LitSpecular");
        if (materialLitSpecular == nullptr)
            return 1;

        MaterialInstance* materialLitSpecularInstance = renderer->CreateMaterialInstance(materialLitSpecular);
        if (materialLitSpecularInstance == nullptr)
            return 1;

        materialLitSpecularInstance->SetTexture("textureSampler", *wallTexture);
        materialLitSpecularInstance->SetTexture("specularTextureSampler", *wallTextureSpecular);
        //materialLitSpecularInstance->SetTexture("normalTextureSampler", *wallTextureNormal);
        materialLitSpecularInstance->SetFloat("matUbo.specularStrength", 1.5f);
        materialLitSpecularInstance->SetFloat("matUbo.shininess", 16.0f);

        Material* materialLitSpecularNormal = materialManager->getMaterial("LitSpecularNormal");
        if (materialLitSpecularNormal == nullptr)
            return 1;

        MaterialInstance* materialLitSpecularNormalInstance = renderer->CreateMaterialInstance(materialLitSpecularNormal);
        if (materialLitSpecularNormalInstance == nullptr)
            return 1;

        materialLitSpecularNormalInstance->SetTexture("textureSampler", *wallTexture);
        materialLitSpecularNormalInstance->SetTexture("specularTextureSampler", *wallTextureSpecular);
        materialLitSpecularNormalInstance->SetTexture("normalTextureSampler", *wallTextureNormal);
        materialLitSpecularNormalInstance->SetFloat("matUbo.specularStrength", 1.5f);
        materialLitSpecularNormalInstance->SetFloat("matUbo.shininess", 16.0f);
        materialLitSpecularNormalInstance->SetVec4("matUbo.tilingOffset", glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));
        
        Material* materialUnlit = materialManager->getMaterial("UnlitColor");
        if (materialUnlit == nullptr)
            return 1;

        MaterialInstance* materialUnlitInstance = renderer->CreateMaterialInstance(materialUnlit);
        if (materialUnlitInstance == nullptr)
            return 1;

        materialUnlitInstance->SetVec4("matUbo.color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        Material* materialUnlitTexture = materialManager->getMaterial("UnlitTexture");
        if (materialUnlitTexture == nullptr)
            return 1;

        MaterialInstance* materialUnlitTextureInstance = renderer->CreateMaterialInstance(materialUnlitTexture);
        if (materialUnlitTextureInstance == nullptr)
            return 1;

        materialUnlitTextureInstance->SetTexture("textureSampler", *wallTexture);
        
        MaterialInstance* boxMaterialLitSpecularNormalInstance = renderer->CreateMaterialInstance(materialLitSpecularNormal);
        if (boxMaterialLitSpecularNormalInstance == nullptr)
            return 1;

        boxMaterialLitSpecularNormalInstance->SetTexture("textureSampler", *boxTexture);
        boxMaterialLitSpecularNormalInstance->SetTexture("specularTextureSampler", *boxTextureMetalness);
        boxMaterialLitSpecularNormalInstance->SetTexture("normalTextureSampler", *boxTextureNormal);
        boxMaterialLitSpecularNormalInstance->SetFloat("matUbo.specularStrength", 1.5f);
        boxMaterialLitSpecularNormalInstance->SetFloat("matUbo.shininess", 16.0f);
        boxMaterialLitSpecularNormalInstance->SetVec4("matUbo.tilingOffset", glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));

        MaterialInstance* groundMaterialLitSpecularNormalInstance = renderer->CreateMaterialInstance(materialLitSpecularNormal);
        if (groundMaterialLitSpecularNormalInstance == nullptr)
            return 1;

        groundMaterialLitSpecularNormalInstance->SetTexture("textureSampler", *groundTexture);
        groundMaterialLitSpecularNormalInstance->SetTexture("specularTextureSampler", *grayTexture);
        groundMaterialLitSpecularNormalInstance->SetTexture("normalTextureSampler", *groundTextureNormal);
        groundMaterialLitSpecularNormalInstance->SetFloat("matUbo.specularStrength", 1.5f);
        groundMaterialLitSpecularNormalInstance->SetFloat("matUbo.shininess", 16.0f);
        groundMaterialLitSpecularNormalInstance->SetVec4("matUbo.tilingOffset", glm::vec4(4.0f, 4.0f, 0.0f, 0.0f));

        MaterialInstance* marbreMaterialLitSpecularNormalInstance = renderer->CreateMaterialInstance(materialLitSpecularNormal);
        if (marbreMaterialLitSpecularNormalInstance == nullptr)
            return 1;

        marbreMaterialLitSpecularNormalInstance->SetTexture("textureSampler", *marbreTexture);
        marbreMaterialLitSpecularNormalInstance->SetTexture("specularTextureSampler", *marbreTextureSpec);
        marbreMaterialLitSpecularNormalInstance->SetTexture("normalTextureSampler", *marbreTextureNormal);
        marbreMaterialLitSpecularNormalInstance->SetFloat("matUbo.specularStrength", 1.5f);
        marbreMaterialLitSpecularNormalInstance->SetFloat("matUbo.shininess", 16.0f);
        marbreMaterialLitSpecularNormalInstance->SetVec4("matUbo.tilingOffset", glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));
        
        Scene* scene = new Scene();

        FreeCam* freeCam = scene->spawnActor<FreeCam>("FreeCam");
        {
            SceneComponent* sceneComponent = freeCam->getComponent<SceneComponent>();

            sceneComponent->setPosition(glm::vec3(0.0f, 10.0f, 9.0f));
            sceneComponent->lookAt(sceneComponent->getPosition(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            sceneComponent->setParent(scene->getRoot());

            freeCam->setupInputListener(application.GetInputListenner());

            SpotLightComponent* spotLightComponent = freeCam->addComponent<SpotLightComponent>();
            spotLightComponent->data.radius = 15.0f;
            spotLightComponent->data.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
            spotLightComponent->data.intensity = 1.0f;
            spotLightComponent->data.outer = 10.0f;

            freeCam->getComponent<CameraComponent>()->SetHdriMaterial(materialHdriInstance, hdriTexture);
        }

        Actor* wall1 = scene->spawnActor<Actor>("wall1");
        {
            SceneComponent* sceneComponent = wall1->addComponent<SceneComponent>();
            sceneComponent->position = glm::vec3(-3.5f, 3.0f, 0.0f);
            sceneComponent->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
            sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 3.0f;
            sceneComponent->setParent(scene->getRoot());

            StaticMeshComponent* staticMeshComponent = wall1->addComponent<StaticMeshComponent>();
            staticMeshComponent->setMesh(wallMesh);
            staticMeshComponent->setMaterialInstance(materialLitSpecularNormalInstance);
            staticMeshComponent->EnableDebugTangent(true);

            ColliderComponent* colliderComponent = wall1->addComponent<ColliderComponent>();
            colliderComponent->setShape(ColliderComponent::Shape::Mesh);
        }

        Actor* wall2 = scene->spawnActor<Actor>("wall2");
        {
            SceneComponent* sceneComponent = wall2->addComponent<SceneComponent>();
            sceneComponent->position = glm::vec3(3.5f, 3.0f, 0.0f);
            sceneComponent->setRotation(glm::vec3(0.0f, -90.0f, 0.0f));
            sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 3.0f;
            sceneComponent->setParent(scene->getRoot());

            StaticMeshComponent* staticMeshComponent = wall2->addComponent<StaticMeshComponent>();
            staticMeshComponent->setMesh(wallMesh);
            staticMeshComponent->setMaterialInstance(materialLitSpecularNormalInstance);
            staticMeshComponent->EnableDebugTangent(true);

            ColliderComponent* colliderComponent = wall2->addComponent<ColliderComponent>();
            colliderComponent->setShape(ColliderComponent::Shape::Mesh);
        }

        Actor* sphereActor = scene->spawnActor<FlyingPointLight>("FlyingPointLight");
        {
            SceneComponent* sceneComponent = sphereActor->getComponent<SceneComponent>();
            sceneComponent->position = glm::vec3(0.0f, 6.0f, 0.0f);
            sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f;
            sceneComponent->setParent(scene->getRoot());

            PointLightComponent* pointLightComponent = sphereActor->addComponent<PointLightComponent>();
            pointLightComponent->data.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
            pointLightComponent->data.intensity = 1.0f;
            pointLightComponent->data.range = 2.5f;

            StaticMeshComponent* staticMeshComponent = sphereActor->addComponent<StaticMeshComponent>();
            staticMeshComponent->setMesh(sphereMesh);
            staticMeshComponent->setMaterialInstance(materialUnlitInstance);

            ColliderComponent* colliderComponent = sphereActor->addComponent<ColliderComponent>();
            colliderComponent->setShape(ColliderComponent::Shape::Sphere);

            sphereActor->start();
        }

        Actor* dirLight = scene->spawnActor<Actor>("dirLight");
        {
            SceneComponent* sceneComponent = dirLight->addComponent<SceneComponent>();
            sceneComponent->setPosition(glm::vec3(0.0f, 3.0f, 0.0f));
            sceneComponent->rotate(45.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            sceneComponent->setParent(scene->getRoot());

            DirLightComponent* dirLightComponent = dirLight->addComponent<DirLightComponent>();
            dirLightComponent->data.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
            dirLightComponent->data.intensity = 1.0f;

            StaticMeshComponent* staticMeshComponent = dirLight->addComponent<StaticMeshComponent>();
            staticMeshComponent->setMesh(sphereMesh);
            staticMeshComponent->setMaterialInstance(marbreMaterialLitSpecularNormalInstance);

            ColliderComponent* colliderComponent = dirLight->addComponent<ColliderComponent>();
            colliderComponent->setShape(ColliderComponent::Shape::Sphere);

            dirLight->start();
        }

        Actor* box = scene->spawnActor<Actor>("box");
        {
            SceneComponent* sceneComponent = box->addComponent<SceneComponent>();
            sceneComponent->position = glm::vec3(0.0f, 0.0f, -5.0f);
            sceneComponent->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
            sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 0.03f;
            sceneComponent->setParent(scene->getRoot());

            StaticMeshComponent* staticMeshComponent = box->addComponent<StaticMeshComponent>();
            staticMeshComponent->setMesh(boxMesh);
            staticMeshComponent->setMaterialInstance(boxMaterialLitSpecularNormalInstance);
            //staticMeshComponent->EnableDebugTangent(true);

            ColliderComponent* colliderComponent = box->addComponent<ColliderComponent>();
            colliderComponent->setShape(ColliderComponent::Shape::Mesh);
        }
        
        Actor* ground = scene->spawnActor<Actor>("ground");
        {
            SceneComponent* sceneComponent = ground->addComponent<SceneComponent>();
            sceneComponent->position = glm::vec3(0.0f, 0.0f, 0.0f);
            sceneComponent->setRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
            sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 10.0f;
            sceneComponent->setParent(scene->getRoot());

            StaticMeshComponent* staticMeshComponent = ground->addComponent<StaticMeshComponent>();
            staticMeshComponent->setMesh(wallMesh);
            staticMeshComponent->setMaterialInstance(groundMaterialLitSpecularNormalInstance);
            //staticMeshComponent->EnableDebugTangent(true);

            ColliderComponent* colliderComponent = ground->addComponent<ColliderComponent>();
            colliderComponent->setShape(ColliderComponent::Shape::Mesh);
        }
        

        if (application.Run(scene) == false)
            ret = 1;

        delete scene;

        delete sphereMesh;

        delete materialUnlitInstance;
    }

    return ret;
}

/*
    

    

    Scene* scene = new Scene;

    

    FreeCam* freeCam = scene->spawnActor<FreeCam>("FreeCam");
    {
        SceneComponent* sceneComponent = freeCam->getComponent<SceneComponent>();

        sceneComponent->lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        sceneComponent->setParent(scene->getRoot());

        freeCam->setupInputListener(&inputListener);
    }

    Actor* light = scene->spawnActor<Actor>("Light_1");
    {
        SceneComponent* sceneComponent = light->addComponent<SceneComponent>();
        sceneComponent->position = glm::vec3(-2.0f, 3.0f, -5.5f);
        sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f;
        sceneComponent->setParent(scene->getRoot());

        StaticMeshComponent* staticMeshComponent = light->addComponent<StaticMeshComponent>();
        staticMeshComponent->setMesh(&sphere);
        staticMeshComponent->setMaterial(materialUnlit);

        PointLightComponent* lightComponent = light->addComponent<PointLightComponent>();

        ColliderComponent* colliderComponent = light->addComponent<ColliderComponent>();
        colliderComponent->setShape(ColliderComponent::Shape::Sphere);
    }

    Actor* light2 = scene->spawnActor<Actor>("Light_2");
    {
        SceneComponent* sceneComponent = light2->addComponent<SceneComponent>();
        sceneComponent->position = glm::vec3(2.0f, 3.0f, -5.5f);
        sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f;
        sceneComponent->setParent(scene->getRoot());

        StaticMeshComponent* staticMeshComponent = light2->addComponent<StaticMeshComponent>();
        staticMeshComponent->setMesh(&sphere);
        staticMeshComponent->setMaterial(materialUnlit);

        PointLightComponent* lightComponent = light2->addComponent<PointLightComponent>();

        ColliderComponent* colliderComponent = light2->addComponent<ColliderComponent>();
        colliderComponent->setShape(ColliderComponent::Shape::Sphere);
    }

    Actor* center = scene->spawnActor<Actor>("Center");
    {
        SceneComponent* sceneComponent = center->addComponent<SceneComponent>();
        sceneComponent->position = glm::vec3(0.0f, 0.0f, 0.0f);
        sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f;
        sceneComponent->setParent(scene->getRoot());

        StaticMeshComponent* staticMeshComponent = center->addComponent<StaticMeshComponent>();
        staticMeshComponent->setMesh(&sphere);
        staticMeshComponent->setMaterial(materialUnlit);

        ColliderComponent* colliderComponent = center->addComponent<ColliderComponent>();
        colliderComponent->setShape(ColliderComponent::Shape::Box);
    }

    // Loop

    delete scene;

    pxPhysics->release();

    TwTerminate();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
*/
