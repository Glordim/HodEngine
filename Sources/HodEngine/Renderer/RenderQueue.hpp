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
	public:
										RenderQueue() = default;
										~RenderQueue();

		void							Init();
		void							Terminate();

		void							Prepare(Context* context);
		void							Prepare(RenderTarget* renderTarget, RenderTarget* pickingRenderTarget);
		uint32_t						GetRenderWidth() const;
		uint32_t						GetRenderHeight() const;

		void							PushRenderCommand(RenderCommand* renderCommand);
		void							Execute();

	private:

		std::vector<RenderCommand*>		_renderCommands;

		MaterialInstance*				_pickingMaterialInstance = nullptr;

		Context*						_context = nullptr;
		RenderTarget*					_renderTarget = nullptr;
		RenderTarget*					_pickingRenderTarget = nullptr;
	};
}
