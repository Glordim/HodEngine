#include <iostream>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstring>
#include <vector>

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "VertexShader.h"
#include "FragmentShader.h"

#include "Scene.hpp"

#include "Actor.h"
#include "SceneComponent.h"
#include "StaticMeshComponent.h"
#include "CameraComponent.h"
#include "LightComponent.h"

#include "InputListener.h"

#include "FreeCam.h"

#include "glm/gtc/matrix_transform.hpp"

#include "Allocator.h"
#include "Error.h"

#include <AntTweakBar.h>

#include <PxPhysicsAPI.h>

void MessageCallback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);

    fflush(stderr);
}

void onErrorCallback(int errorCode, const char* errorDescription)
{
    fprintf(stderr, "Error %d : %s\n", errorCode, errorDescription);
    fflush(stderr);
}

bool shouldExit;

void onCloseWindowCallback(GLFWwindow* window)
{
    shouldExit = true;
}

void onResizeWindowCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0.0f, 0.0f, width, height);

    TwWindowSize(width, height);
}

InputListener inputListener;

void onKeyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (TwEventKeyGLFW3(window, key, action, scancode, mods))
        return;

    inputListener.injectKeyInput(key, scancode, action, mods);
}

void onMouseButtonEventCallback(GLFWwindow* window, int mouseButtonId, int action, int mods)
{
    if (TwEventMouseButtonGLFW3(window, mouseButtonId, action, mods))
        return;

    inputListener.injectMouseButtonInput(mouseButtonId, action, mods);
}

void onMouseEventCallback(GLFWwindow* window, double x, double y)
{
    if (TwEventMousePosGLFW((int)x, (int)y))
        return;

    inputListener.injectMouseMoveInput(x, y);
}

int main()
{
    glfwInit();

    glfwSetErrorCallback(onErrorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Toto", monitor, nullptr);

    glfwSetWindowCloseCallback(window, onCloseWindowCallback);
    glfwSetWindowSizeCallback(window, onResizeWindowCallback);
    glfwSetKeyCallback(window, onKeyEventCallback);
    glfwSetCursorPosCallback(window, onMouseEventCallback);
    glfwSetMouseButtonCallback(window, onMouseButtonEventCallback);
    glfwSetCharCallback(window, (GLFWcharfun)TwEventCharModsGLFW3);
    

    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    std::cout << "OpenGL version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL renderer : " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL vendor : " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL shader version : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    shouldExit = false;

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    //glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    Allocator allocator;
    Error error;

    //physx::PxDefaultAllocator defaultAllocator;
    //physx::PxDefaultErrorCallback defaultErrorCallback;

    physx::PxFoundation* pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocator, error);

    physx::PxTolerancesScale tolerancesScale;

    physx::PxPhysics* pxPhysics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *pxFoundation, tolerancesScale);

    TwInit(TW_OPENGL_CORE, NULL);
    TwWindowSize(1920, 1080);

    bool boolcpp = false;

    TwBar *myBar;
    myBar = TwNewBar("MainTweakBar");
    TwAddVarRW(myBar, "superVar", TW_TYPE_BOOLCPP, &boolcpp, "");

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    Mesh sphere;
    if (sphere.loadObj("Gizmos/sphere.obj") == false)
        return 1;

    VertexShader unlitVertexShader;
    if (unlitVertexShader.load("Shader/UnlitColor.vert") == false)
        return 1;

    FragmentShader unlitFragmentShader;
    if (unlitFragmentShader.load("Shader/UnlitColor.frag") == false)
        return 1;

    Material unlitMaterial(unlitVertexShader, unlitFragmentShader);
    unlitMaterial.setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    Mesh mesh;
    if (mesh.loadObj("Wall/wall.obj") == false) //if (mesh.loadObj("Canon/M_Canon.obj") == false)
        return 1;

    Texture texture;
    if (texture.load("Wall/brickwall.jpg") == false) //if (texture.load("Canon/T_Canon_C_AO.tga") == false)
        return 1;

    Texture texture2;
    if (texture2.load("Wall/brickwall_normal.jpg") == false)
        return 1;

    VertexShader vertexShader;
    if (vertexShader.load("Shader/Lit.vert") == false)
        return 1;

    FragmentShader fragmentShader;
    if (fragmentShader.load("Shader/Lit.frag") == false)
        return 1;

    VertexShader vertexShader2;
    if (vertexShader2.load("Shader/LitNormal.vert") == false)
        return 1;

    FragmentShader fragmentShader2;
    if (fragmentShader2.load("Shader/LitNormal.frag") == false)
        return 1;

    Material material(vertexShader, fragmentShader);
    material.setTexture("textureSampler", texture);
    material.setFloat("specularStrength", 0.5f);

    Material material2(vertexShader, fragmentShader);
    material2.setTexture("textureSampler_Albedo", texture);
    material2.setFloat("specularStrength", 0.5f);
    //material2.setTexture("textureSampler_Normal", texture2);
    //material2.setFloat("specularStrength", 0.5f);

    Scene* scene = new Scene;

    Actor* gun = scene->spawnActor<Actor>("Gun");
    {
        SceneComponent* sceneComponent = gun->addComponent<SceneComponent>();
        sceneComponent->position = glm::vec3(3.5f, 0.0f, 0.0f);
        sceneComponent->setRotation(glm::vec3(90.0f, 30.0f, 0.0f));
        sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 3.0f;
        sceneComponent->setParent(scene->getRoot());
        StaticMeshComponent* staticMeshComponent = gun->addComponent<StaticMeshComponent>();
        staticMeshComponent->setMesh(&mesh);
        staticMeshComponent->setMaterial(&material);
    }

    Actor* gun2 = scene->spawnActor<Actor>("Gun_2");
    {
        SceneComponent* sceneComponent = gun2->addComponent<SceneComponent>();
        sceneComponent->position = glm::vec3(-3.5f, 0.0f, 0.0f);
        sceneComponent->setRotation(glm::vec3(90.0f, -30.0f, 0.0f));
        sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 3.0f;
        sceneComponent->setParent(scene->getRoot());
        StaticMeshComponent* staticMeshComponent = gun2->addComponent<StaticMeshComponent>();
        staticMeshComponent->setMesh(&mesh);
        staticMeshComponent->setMaterial(&material2);
    }

    FreeCam* freeCam = scene->spawnActor<FreeCam>("FreeCam");
    {
        SceneComponent* sceneComponent = freeCam->getComponent<SceneComponent>();

        sceneComponent->lookAt(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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
        staticMeshComponent->setMaterial(&unlitMaterial);

        LightComponent* lightComponent = light->addComponent<LightComponent>();
    }

    Actor* light2 = scene->spawnActor<Actor>("Light_2");
    {
        SceneComponent* sceneComponent = light2->addComponent<SceneComponent>();
        sceneComponent->position = glm::vec3(2.0f, 3.0f, -5.5f);
        sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f;
        sceneComponent->setParent(scene->getRoot());

        StaticMeshComponent* staticMeshComponent = light2->addComponent<StaticMeshComponent>();
        staticMeshComponent->setMesh(&sphere);
        staticMeshComponent->setMaterial(&unlitMaterial);

        LightComponent* lightComponent = light2->addComponent<LightComponent>();
    }

    Actor* center = scene->spawnActor<Actor>("Center");
    {
        SceneComponent* sceneComponent = center->addComponent<SceneComponent>();
        sceneComponent->position = glm::vec3(0.0f, 0.0f, 0.0f);
        sceneComponent->scale = glm::vec3(1.0f, 1.0f, 1.0f) * 0.1f;
        sceneComponent->setParent(scene->getRoot());

        StaticMeshComponent* staticMeshComponent = center->addComponent<StaticMeshComponent>();
        staticMeshComponent->setMesh(&sphere);
        staticMeshComponent->setMaterial(&unlitMaterial);

        glm::vec3 position = sceneComponent->getPosition();

        physx::PxMaterial* pxMaterial = pxPhysics->createMaterial(0.0f, 0.0f, 0.0f);

        physx::PxTransform pxTransform(physx::PxVec3(position.x, position.y, position.z));

        physx::PxRigidStatic* pxActor = pxPhysics->createRigidStatic(pxTransform);

        physx::PxGeometryHolder geometryHolder;
        const physx::PxSphereGeometry& geometry = geometryHolder.sphere();

        physx::PxShape* pxShape = pxPhysics->createShape(geometry, *pxMaterial);
        pxActor->attachShape(*pxShape);

        scene->addPxActor(center, pxActor);
    }

    float dt = 0.0f;
    float lastTime = glfwGetTime();

    while (shouldExit == false)
    {
        glfwPollEvents();
        inputListener.process();

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
            glfwSetCursorPos(window, 1920.0f * 0.5f, 1080.0f * 0.5f);

        double time = glfwGetTime();
        dt = time - lastTime;
        lastTime = time;

        dt = std::min(dt, 0.2f);

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

        TwDraw();

        glfwSwapBuffers(window);
    }

    delete scene;

    pxPhysics->release();

    TwTerminate();

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
