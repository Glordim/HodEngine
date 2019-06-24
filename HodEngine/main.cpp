#include <iostream>

#include "glad/glad.h"
//#include <GLFW/glfw3.h>

#include <SDL.h>
//#include <SDL_vulkan.h>

#include <algorithm>
#include <cstring>
#include <vector>

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "VertexShader.h"
#include "FragmentShader.h"
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

#include <PxPhysicsAPI.h>

#include <tchar.h>

void MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    /*
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);

    fflush(stderr);
    */
}

void onErrorCallback(int errorCode, const char* errorDescription)
{
    /*
    fprintf(stderr, "Error %d : %s\n", errorCode, errorDescription);
    fflush(stderr);
    */
}

void onResizeWindowCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0.0f, 0.0f, width, height);

    TwWindowSize(width, height);
}

InputListener inputListener;

int __cdecl _tmain()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Toto", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL);

    if (window == nullptr)
    {
        SDL_Log("Unable to create Window: %s", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    /*
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    */
    SDL_GLContext context = SDL_GL_CreateContext(window);

    if (context == nullptr)
    {
        SDL_Log("Unable to create Context: %s", SDL_GetError());
        return 1;
    }

    SDL_GL_SetSwapInterval(1); // Syncrho vertical

    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

    std::cout << "OpenGL version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL renderer : " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL vendor : " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL shader version : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    Allocator allocator;
    Error error;

    physx::PxDefaultAllocator defaultAllocator;
    physx::PxDefaultErrorCallback defaultErrorCallback;

    physx::PxFoundation* pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAllocator, defaultErrorCallback);

    physx::PxTolerancesScale tolerancesScale;

    physx::PxPhysics* pxPhysics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *pxFoundation, tolerancesScale);

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

    float dt = 0.0f;
    Uint32 lastTime = SDL_GetTicks();

    bool shouldExit = false;

    while (shouldExit == false)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event) != 0)
        {
            if (TwEventSDL(&event, SDL_MAJOR_VERSION, SDL_MINOR_VERSION) != 0)
                continue;

            if (event.type == SDL_QUIT)
                shouldExit = true;
            else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
                inputListener.injectKeyInput(event.key.keysym.sym, event.key.keysym.scancode, event.key.state, event.key.keysym.mod);
            //else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
            //    inputListener.injectMouseButtonInput(mouseButtonId, action, mods);
        }

        inputListener.process();

        //if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
        //    glfwSetCursorPos(window, 1920.0f * 0.5f, 1080.0f * 0.5f);

        Uint32 time = SDL_GetTicks();

        dt = std::min((float)(time - lastTime) / 1000.0f, 0.2f);

        lastTime = time;

        // Physic
        scene->simulatePhysic(dt);

        // Gameplay
        scene->update(dt);

        // TODO move in actor::update or component
        // Rotate Gun
        {
            SceneComponent* sceneComponent = gun->getComponent<SceneComponent>();
            //sceneComponent->rotate(glm::radians(25.0f * dt), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        {
            SceneComponent* sceneComponent = gun2->getComponent<SceneComponent>();
            //sceneComponent->rotate(glm::radians(25.0f * dt), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        // TODO move in actor::update or component
        // Move light
        {
            SceneComponent* sceneComponent = light->getComponent<SceneComponent>();
            sceneComponent->setPosition(sceneComponent->getPosition() + glm::vec3(0.0f, 0.0035f, 0.0f) * sin((float)time));
        }
        {
            SceneComponent* sceneComponent = light2->getComponent<SceneComponent>();
            sceneComponent->setPosition(sceneComponent->getPosition() + glm::vec3(0.0f, 0.0035f, 0.0f) * sin((float)time));
        }

        // TODO scene->render (process all camera)
        // Render
        CameraComponent* cameraComponent = freeCam->getComponent<CameraComponent>();
        cameraComponent->drawScene(*scene);        

        scene->drawDebugPhysics(cameraComponent, dt);

        TwDraw();

        SDL_GL_SwapWindow(window);
    }

    delete scene;

    pxPhysics->release();

    TwTerminate();

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
