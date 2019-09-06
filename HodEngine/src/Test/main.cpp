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
#include "../Component/LightComponent.h"
#include "../Component/ColliderComponent.h"

#include "../InputListener.h"

#include "FreeCam.h"
#include "FlyingPointLight.h"

#include "glm/gtc/matrix_transform.hpp"

#include "../Allocator.h"
#include "../Error.h"

#include <AntTweakBar.h>

#include <tchar.h>

#include "../Renderer/GpuDeviceHelper.h"
#include "../Renderer/GraphicsSettings.h"
#include "../ScreenHelper.h"
#include "../Application.h"

#include "../Renderer/Renderer.h"

#include "../AllocationTracker.hpp"

InputListener inputListener;

int __cdecl _tmain()
{
    int ret = 0;

    //AllocationTracker::Start_Tracking();
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

        Mesh* sphereMesh = renderer->CreateMesh("Mesh/sphere.obj");
        if (sphereMesh == nullptr)
            return 1;

        Mesh* wallMesh = renderer->CreateMesh("Mesh/wall.obj");
        if (wallMesh == nullptr)
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
        /*
            Texture* gunTexture = renderer->CreateTexture("Texture/brickwall.jpg");
            if (gunTexture == nullptr)
                return 1;
        */

        MaterialManager* materialManager = MaterialManager::getInstance();
        
        Material* materialLit = materialManager->getMaterial("Lit");
        if (materialLit == nullptr)
            return 1;

        MaterialInstance* materialLitInstance = renderer->CreateMaterialInstance(materialLit);
        if (materialLitInstance == nullptr)
            return 1;

        materialLitInstance->SetTexture("textureSampler", *wallTexture);
        //materialLitInstance->SetFloat("matUbo", "specularStrength", 1.5f);

        Material* materialLitSpecular = materialManager->getMaterial("LitSpecularNormal");
        if (materialLitSpecular == nullptr)
            return 1;

        MaterialInstance* materialLitSpecularInstance = renderer->CreateMaterialInstance(materialLit);
        if (materialLitSpecularInstance == nullptr)
            return 1;

        materialLitSpecularInstance->SetTexture("textureSampler", *wallTexture);
        materialLitSpecularInstance->SetTexture("specularTextureSampler", *wallTextureSpecular);
        materialLitSpecularInstance->SetTexture("normalTextureSampler", *wallTextureNormal);
        materialLitSpecularInstance->SetFloat("matUbo", "specularStrength", 1.5f);
        
        Material* materialUnlit = materialManager->getMaterial("UnlitColor");
        if (materialUnlit == nullptr)
            return 1;

        MaterialInstance* materialUnlitInstance = renderer->CreateMaterialInstance(materialUnlit);
        if (materialUnlitInstance == nullptr)
            return 1;

        materialUnlitInstance->SetVec4("matUbo", "color", glm::vec4(0.65f, 0.65f, 0.65f, 1.0f));

        Material* materialUnlitTexture = materialManager->getMaterial("UnlitTexture");
        if (materialUnlitTexture == nullptr)
            return 1;

        MaterialInstance* materialUnlitTextureInstance = renderer->CreateMaterialInstance(materialUnlitTexture);
        if (materialUnlitTextureInstance == nullptr)
            return 1;

        materialUnlitTextureInstance->SetTexture("textureSampler", *wallTexture);

        Scene* scene = new Scene();

        FreeCam* freeCam = scene->spawnActor<FreeCam>("FreeCam");
        {
            SceneComponent* sceneComponent = freeCam->getComponent<SceneComponent>();

            sceneComponent->lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            sceneComponent->setParent(scene->getRoot());

            freeCam->setupInputListener(application.GetInputListenner());
        }

        Actor* wall1 = scene->spawnActor<Actor>("wall1");
        {
            SceneComponent* sceneComponent = wall1->addComponent<SceneComponent>();
            sceneComponent->position = glm::vec3(-3.5f, 0.0f, 0.0f);
            sceneComponent->setRotation(glm::vec3(0.0f, -150.0f, 0.0f));
            sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 3.0f;
            sceneComponent->setParent(scene->getRoot());

            StaticMeshComponent* staticMeshComponent = wall1->addComponent<StaticMeshComponent>();
            staticMeshComponent->setMesh(wallMesh);
            //staticMeshComponent->setMaterialInstance(materialUnlitInstance);
            //staticMeshComponent->setMaterialInstance(materialUnlitTextureInstance);
            staticMeshComponent->setMaterialInstance(materialLitInstance);

            ColliderComponent* colliderComponent = wall1->addComponent<ColliderComponent>();
            colliderComponent->setShape(ColliderComponent::Shape::Mesh);
        }

        Actor* wall2 = scene->spawnActor<Actor>("wall2");
        {
            SceneComponent* sceneComponent = wall2->addComponent<SceneComponent>();
            sceneComponent->position = glm::vec3(3.5f, 0.0f, 0.0f);
            sceneComponent->setRotation(glm::vec3(0.0f, -30.0f, 0.0f));
            sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 3.0f;
            sceneComponent->setParent(scene->getRoot());

            StaticMeshComponent* staticMeshComponent = wall2->addComponent<StaticMeshComponent>();
            staticMeshComponent->setMesh(wallMesh);
            staticMeshComponent->setMaterialInstance(materialUnlitInstance);
            //staticMeshComponent->setMaterialInstance(materialLitSpecularInstance);

            ColliderComponent* colliderComponent = wall2->addComponent<ColliderComponent>();
            colliderComponent->setShape(ColliderComponent::Shape::Mesh);
        }

        Actor* sphereActor = scene->spawnActor<FlyingPointLight>("FlyingPointLight");
        {
            SceneComponent* sceneComponent = sphereActor->getComponent<SceneComponent>();
            sceneComponent->position = glm::vec3(-2.0f, 3.0f, -5.5f);
            sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f;
            sceneComponent->setParent(scene->getRoot());

            LightComponent* pointLightComponent = sphereActor->addComponent<LightComponent>();
            pointLightComponent->data.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
            pointLightComponent->data.intensity = 1.0f;
            pointLightComponent->data.range = 2.5f;

            StaticMeshComponent* staticMeshComponent = sphereActor->addComponent<StaticMeshComponent>();
            staticMeshComponent->setMesh(sphereMesh);
            staticMeshComponent->setMaterialInstance(materialUnlitInstance);

            ColliderComponent* colliderComponent = sphereActor->addComponent<ColliderComponent>();
            colliderComponent->setShape(ColliderComponent::Shape::Sphere);
        }

        if (application.Run(scene) == false)
            ret = 1;

        delete scene;

        delete sphereMesh;

        delete materialUnlitInstance;
    }

    //AllocationTracker::End_Tracking_And_Dump();

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

        LightComponent* lightComponent = light->addComponent<LightComponent>();

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

        LightComponent* lightComponent = light2->addComponent<LightComponent>();

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
