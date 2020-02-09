#include "ActorDebugWindow.h"

#include <ImGui/src/imgui.h>
#include <ImGui/src/ImGuizmo.h>

#include <Game/src/Actor.h>
#include <Game/src/Component/CameraComponent.h>
#include <Game/src/Component/SceneComponent.h>


#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

namespace HOD
{
    namespace GAME
    {
		ActorDebugWindow::ActorDebugWindow() : DebugWindow()
        {

        }

		ActorDebugWindow::~ActorDebugWindow()
		{
            
        }

        void ActorDebugWindow::Draw()
        {
            ImGui::Begin("Selected Actor");
            {
                if (pActor == nullptr)
                {
                    ImGui::Text("No selected actor...");
                }
                else
                {
                    ImGui::Separator();
                    ImGui::Text("Name: %s", pActor->getName().c_str());
                    ImGui::Separator();

                    pActor->DrawImGui();

                    std::vector<GAME::Component*> vComponent = pActor->getAllComponent();
                    for (GAME::Component* pComponent : vComponent)
                    {
                        if (ImGui::CollapsingHeader(pComponent->GetName(), actorComponentCollapseMap[pComponent->GetName()]) == true)
                        {
                            pComponent->DrawImGui();
                        }
                    }

					static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
					static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

					GAME::SceneComponent* pSceneComponent = pActor->getComponent<GAME::SceneComponent>();

					glm::mat4 modelMatrix = pSceneComponent->getModelMatrix();
					glm::mat4 viewMatrix; // glm::inverse(pCamera->GetActor()->getComponent<GAME::SceneComponent>()->getModelMatrix());
					glm::mat4 projectionMatrix; // = pCamera->getProjectionMatrix();

					ImGuiIO& io = ImGui::GetIO();
					ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
					ImGuizmo::Manipulate(&viewMatrix[0][0], &projectionMatrix[0][0], mCurrentGizmoOperation, mCurrentGizmoMode, &modelMatrix[0][0], nullptr, nullptr);

					glm::vec3 skew;
					glm::vec4 perspective;
					glm::vec3 scale;
					glm::quat rotation;
					glm::vec3 position;

					glm::decompose(modelMatrix, scale, rotation, position, skew, perspective);

					pSceneComponent->setPosition(position);
					pSceneComponent->setRotation(rotation);
					pSceneComponent->setScale(scale);
                }
            }
            ImGui::End();
        }

        void ActorDebugWindow::ShowActor(GAME::Actor* pActor)
        {
            this->pActor = pActor;
        }
    }
}
