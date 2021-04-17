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
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		GizmoDebugWindow::GizmoDebugWindow() : DebugWindow()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		GizmoDebugWindow::~GizmoDebugWindow()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void GizmoDebugWindow::Draw()
		{
			ImGui::Begin("Gizmo");
			{
				if (_actor == nullptr)
				{
					ImGui::Text("No selected actor...");
				}
				else
				{
					ImGui::BeginGroup();
					{
						if (ImGui::RadioButton("Translate", _operation == ImGuizmo::TRANSLATE) == true)
						{
							_operation = ImGuizmo::TRANSLATE;
						}
						if (ImGui::RadioButton("Rotate", _operation == ImGuizmo::ROTATE) == true)
						{
							_operation = ImGuizmo::ROTATE;
						}
						if (ImGui::RadioButton("Scale", _operation == ImGuizmo::SCALE) == true)
						{
							_operation = ImGuizmo::SCALE;
						}
					}
					ImGui::EndGroup();

					ImGui::Separator();

					ImGui::BeginGroup();
					{
						if (ImGui::RadioButton("Local", _mode == ImGuizmo::LOCAL) == true)
						{
							_mode = ImGuizmo::LOCAL;
						}
						if (ImGui::RadioButton("World", _mode == ImGuizmo::WORLD) == true)
						{
							_mode = ImGuizmo::WORLD;
						}
					}
					ImGui::EndGroup();

					GAME::SceneComponent* pSceneComponent = _actor->GetComponent<GAME::SceneComponent>();

					glm::mat4 modelMatrix = pSceneComponent->GetModelMatrix();
					const glm::mat4& viewMatrix = DEBUG_LAYER::DebugLayer::GetInstance()->GetViewMatrix();
					const glm::mat4& projectionMatrix = DEBUG_LAYER::DebugLayer::GetInstance()->GetProjectionMatrix();

					ImGuiIO& io = ImGui::GetIO();
					ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
					ImGuizmo::Manipulate(&viewMatrix[0][0], &projectionMatrix[0][0], _operation, _mode, &modelMatrix[0][0], nullptr, nullptr);

					glm::vec3 skew;
					glm::vec4 perspective;
					glm::vec3 scale;
					glm::quat rotation;
					glm::vec3 position;

					glm::decompose(modelMatrix, scale, rotation, position, skew, perspective);

					pSceneComponent->SetPosition(position);
					//pSceneComponent->SetRotation(rotation); // TODO
					pSceneComponent->SetScale(scale);
				}
			}
			ImGui::End();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void GizmoDebugWindow::SetActor(Actor* actor)
		{
			_actor = actor;
		}
	}
}
