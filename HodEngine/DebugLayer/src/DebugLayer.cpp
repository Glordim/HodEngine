#include "DebugLayer.h"

#include "DebugWindow.h"

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
    DEBUG_LAYER::DebugLayer* CORE::Singleton<DEBUG_LAYER::DebugLayer>::_instance = nullptr;

    namespace DEBUG_LAYER
    {
        DebugLayer::DebugLayer() : Singleton()
        {

        }

        DebugLayer::~DebugLayer()
        {
            
        }

        bool DebugLayer::Init()
        {
            return true;
        }

        void DebugLayer::Clear()
        {

        }

        void DebugLayer::Draw()
        {
			for (DebugWindow* pDebugWindow : _vDebugWindows)
			{
				pDebugWindow->Draw();
			}
        }

		void DebugLayer::RegisterDebugWindow(DebugWindow* pDebugWindow)
		{
			auto it = _vDebugWindows.begin();
			auto itEnd = _vDebugWindows.end();

			while (it != itEnd)
			{
				if (*it == pDebugWindow)
				{
					return; // already registered
				}

				++it;
			}

			_vDebugWindows.push_back(pDebugWindow);
		}

		void DebugLayer::UnregisterDebugWindow(DebugWindow* pDebugWindow)
		{
			auto it = _vDebugWindows.begin();
			auto itEnd = _vDebugWindows.end();

			while (it != itEnd)
			{
				if (*it == pDebugWindow)
				{
					_vDebugWindows.erase(it);
					return;
				}

				++it;
			}
		}
    }
}
