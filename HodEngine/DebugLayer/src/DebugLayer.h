#pragma once

#include <Core/src/Singleton.h>

#include <vector>
#include <map>

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace HOD
{
    namespace GAME
    {
        class Actor;
		class CameraComponent;
    }

    namespace DEBUG_LAYER
    {
		class DebugWindow;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
        class DebugLayer : public CORE::Singleton<DebugLayer>
        {
            friend class CORE::Singleton<DebugLayer>;

        protected:

                                            DebugLayer();
                                            ~DebugLayer() override;

        public:

            bool                            Init();
            void                            Clear();

			void							RegisterDebugWindow(DebugWindow* pDebugWindow);
			void							UnregisterDebugWindow(DebugWindow* pDebugWindow);

			void							SetCameraMatrice(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
			const glm::mat4&				GetProjectionMatrix() const;
			const glm::mat4&				GetViewMatrix() const;

            void                            Draw();

        private:

			std::vector<DebugWindow*>		_vDebugWindows;

			glm::mat4						_viewMatrix;
			glm::mat4						_projectionMatrix;
        };
    }
}
