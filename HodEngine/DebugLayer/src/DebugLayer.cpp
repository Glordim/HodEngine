#include "DebugLayer.h"

#include "DebugWindow.h"

#include <ImGui/src/imgui.h>
#include <ImGui/src/ImGuizmo.h>

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

		void DebugLayer::SetCameraMatrice(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
		{
			_projectionMatrix = projectionMatrix;
			_viewMatrix = viewMatrix;
		}

		const glm::mat4& DebugLayer::GetProjectionMatrix() const
		{
			return _projectionMatrix;
		}

		const glm::mat4& DebugLayer::GetViewMatrix() const
		{
			return _viewMatrix;
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
