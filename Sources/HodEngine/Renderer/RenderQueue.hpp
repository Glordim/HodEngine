#pragma once
#include "HodEngine/Renderer/Export.hpp"
#include "HodEngine/Core/Singleton.hpp"

#include <vector>

namespace hod::renderer
{
	class RenderCommand;
	class RenderTarget;
	class Context;
	class MaterialInstance;

	/// @brief 
	class HOD_RENDERER_API RenderQueue
	{
		_Singleton(RenderQueue)

	public:

										~RenderQueue();

		void							Init();
		void							Terminate();

		void							PushRenderCommand(RenderCommand* renderCommand);
		void							Execute(RenderTarget* renderTarget, RenderTarget* pickingRenderTarget = nullptr);
		void							Execute(Context* context);

	private:

		std::vector<RenderCommand*>		_renderCommands;

		MaterialInstance*				_pickingMaterialInstance = nullptr;
	};
}
