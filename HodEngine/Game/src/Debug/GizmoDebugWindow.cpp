#include "GizmoDebugWindow.h"

#include <Game/src/Actor.h>
#include <Game/src/Component/CameraComponent.h>
#include <Game/src/Component/SceneComponent.h>

#include <DebugLayer/src/DebugLayer.h>

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

namespace HOD
{
    namespace GAME
    {
		GizmoDebugWindow::GizmoDebugWindow() : DebugWindow()
        {

        }

		GizmoDebugWindow::~GizmoDebugWindow()
		{
            
        }

        void GizmoDebugWindow::Draw()
        {
            ImGui::Begin("Gizmo");
            {
                if (_pActor == nullptr)
                {
                    ImGui::Text("No selected actor...");
                }
                else
                {
					ImGui::BeginGroup();
					{
						if (ImGui::RadioButton("Translate", _eOperation == ImGuizmo::TRANSLATE) == true)
						{
							_eOperation = ImGuizmo::TRANSLATE;
						}
						if (ImGui::RadioButton("Rotate", _eOperation == ImGuizmo::ROTATE) == true)
						{
							_eOperation = ImGuizmo::ROTATE;
						}
						if (ImGui::RadioButton("Scale", _eOperation == ImGuizmo::SCALE) == true)
						{
							_eOperation = ImGuizmo::SCALE;
						}
					}
					ImGui::EndGroup();

					ImGui::Separator();

					ImGui::BeginGroup();
					{
						if (ImGui::RadioButton("Local", _eMode == ImGuizmo::LOCAL) == true)
						{
							_eMode = ImGuizmo::LOCAL;
						}
						if (ImGui::RadioButton("World", _eMode == ImGuizmo::WORLD) == true)
						{
							_eMode = ImGuizmo::WORLD;
						}
					}
					ImGui::EndGroup();

					GAME::SceneComponent* pSceneComponent = _pActor->getComponent<GAME::SceneComponent>();

					glm::mat4 modelMatrix = pSceneComponent->getModelMatrix();
					const glm::mat4& viewMatrix = DEBUG_LAYER::DebugLayer::GetInstance()->GetViewMatrix();
					const glm::mat4& projectionMatrix = DEBUG_LAYER::DebugLayer::GetInstance()->GetProjectionMatrix();

					ImGuiIO& io = ImGui::GetIO();
					ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
					ImGuizmo::Manipulate(&viewMatrix[0][0], &projectionMatrix[0][0], _eOperation, _eMode, &modelMatrix[0][0], nullptr, nullptr);

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

        void GizmoDebugWindow::SetActor(Actor* pActor)
        {
            _pActor = pActor;
        }
    }
}
