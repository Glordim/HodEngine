#include <iostream>

#include "glad/glad.h"
//#include <GLFW/glfw3.h>

#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_vulkan.h>

#include <algorithm>
#include <cstring>
#include <vector>

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "Shader.h"
#include "MaterialManager.h"

#include "Scene.hpp"

#include "Actor.h"
#include "SceneComponent.h"
#include "StaticMeshComponent.h"
#include "CameraComponent.h"
#include "LightComponent.h"
#include "ColliderComponent.h"

#include "InputListener.h"

#include "FreeCam.h"

#include "glm/gtc/matrix_transform.hpp"

#include "Allocator.h"
#include "Error.h"

#include <AntTweakBar.h>

#include <tchar.h>

#include <D3d12.h>
#include <dxgi1_4.h>

#include <vulkan.h>

#include "GpuDeviceHelper.h"
#include "ScreenHelper.h"
#include "GraphicsSettings.h"
#include "Application.h"

#include "Renderer.h"

int __cdecl _tmain()
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
    graphicsSettings.fullscreenType = GraphicsSettings::FullscreenType::FullscreenWindow;

    if (application.CreateWindowAndContext("HodEngine", graphicsSettings) == false)
        return 1;

    GpuDevice* bestDevice = nullptr;

    if (GpuDeviceHelper::GetBestAvailableAndCompatibleDevice(&bestDevice) == false)
        return 1;

    Renderer* renderer = Renderer::GetInstance();

    if (renderer->BuildPipeline(bestDevice) == false)
        return 1;

    Shader* vertexShader = renderer->CreateShader("Shader/UnlitVertexColor.vert.vulk", Shader::ShaderType::Vertex);
    Shader* fragmentShader = renderer->CreateShader("Shader/UnlitVertexColor.frag.vulk", Shader::ShaderType::Fragment);

    Material* mat = renderer->CreateMaterial(vertexShader, fragmentShader);
    MaterialInstance* matInstance = renderer->CreateMaterialInstance(mat);

    Mesh* mesh = renderer->CreateMesh("Gizmos/sphere.obj");

    Scene* scene = new Scene();

    FreeCam* freeCam = scene->spawnActor<FreeCam>("FreeCam");
    {
        SceneComponent* sceneComponent = freeCam->getComponent<SceneComponent>();

        sceneComponent->lookAt(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        sceneComponent->setParent(scene->getRoot());

        //freeCam->setupInputListener(&inputListener);
    }

    Actor* sphereActor = scene->spawnActor<Actor>("Sphere");
    {
        SceneComponent* sceneComponent = sphereActor->addComponent<SceneComponent>();
        sceneComponent->position = glm::vec3(-2.0f, 3.0f, -5.5f);
        sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f;
        sceneComponent->setParent(scene->getRoot());

        StaticMeshComponent* staticMeshComponent = sphereActor->addComponent<StaticMeshComponent>();
        staticMeshComponent->setMesh(mesh);
        staticMeshComponent->setMaterialInstance(matInstance);

        ColliderComponent* colliderComponent = sphereActor->addComponent<ColliderComponent>();
        colliderComponent->setShape(ColliderComponent::Shape::Sphere);
    }

    int ret = 0;

    if (application.Run(scene) == false)
        ret = 1;

    delete scene;

    delete mesh;

    delete mat;

    delete vertexShader;
    delete fragmentShader;

    return ret;
}

/*
    TwInit(TW_OPENGL_CORE, NULL);
    TwWindowSize(1920, 1080);

    bool boolcpp = false;

    TwBar *myBar;
    myBar = TwNewBar("MainTweakBar");
    TwAddVarRW(myBar, "superVar", TW_TYPE_BOOLCPP, &boolcpp, "");

    Mesh sphere;
    if (sphere.loadObj("Gizmos/sphere.obj") == false)
        return 1;

    Mesh mesh;
    if (mesh.loadObj("Wall/wall.obj") == false) //if (mesh.loadObj("Canon/M_Canon.obj") == false)
        return 1;

    Texture texture;
    if (texture.load("Wall/brickwall.jpg") == false) //if (texture.load("Canon/T_Canon_C_AO.tga") == false)
        return 1;

    Texture textureNormal;
    if (textureNormal.load("Wall/brickwall_normal.jpg") == false)
        return 1;

    Texture textureSpecular;
    if (textureSpecular.load("Wall/brickwall_Specular.png") == false)
        return 1;

    MaterialManager* materialManager = MaterialManager::getInstance();
    Material* materialLit = materialManager->getMaterial("Lit");    
    materialLit->use();
    materialLit->setTexture("textureSampler", texture);
    materialLit->setFloat("specularStrength", 1.5f);
    Material* materialLitSpecular = materialManager->getMaterial("LitSpecularNormal");
    materialLitSpecular->use();
    materialLitSpecular->setTexture("textureSampler", texture);
    materialLitSpecular->setTexture("specularTextureSampler", textureSpecular);
    materialLitSpecular->setTexture("normalTextureSampler", textureNormal);
    materialLitSpecular->setFloat("specularStrength", 1.5f);
    Material* materialUnlit = materialManager->getMaterial("UnlitColor");
    materialUnlit->use();
    materialUnlit->setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    Scene* scene = new Scene;

    Actor* gun = scene->spawnActor<Actor>("Gun");
    {
        SceneComponent* sceneComponent = gun->addComponent<SceneComponent>();
        sceneComponent->position = glm::vec3(3.5f, 0.0f, 0.0f);
        sceneComponent->setRotation(glm::vec3(0.0f, -150.0f, 0.0f));
        sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 3.0f;
        sceneComponent->setParent(scene->getRoot());

        StaticMeshComponent* staticMeshComponent = gun->addComponent<StaticMeshComponent>();
        staticMeshComponent->setMesh(&mesh);
        staticMeshComponent->setMaterial(materialLit);

        ColliderComponent* colliderComponent = gun->addComponent<ColliderComponent>();
        colliderComponent->setShape(ColliderComponent::Shape::Mesh);
    }

    Actor* gun2 = scene->spawnActor<Actor>("Gun_2");
    {
        SceneComponent* sceneComponent = gun2->addComponent<SceneComponent>();
        sceneComponent->position = glm::vec3(-3.5f, 0.0f, 0.0f);
        sceneComponent->setRotation(glm::vec3(0.0f, -30.0f, 0.0f));
        sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 3.0f;
        sceneComponent->setParent(scene->getRoot());

        StaticMeshComponent* staticMeshComponent = gun2->addComponent<StaticMeshComponent>();
        staticMeshComponent->setMesh(&mesh);
        staticMeshComponent->setMaterial(materialLitSpecular);

        ColliderComponent* colliderComponent = gun2->addComponent<ColliderComponent>();
        colliderComponent->setShape(ColliderComponent::Shape::Mesh);
    }

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
