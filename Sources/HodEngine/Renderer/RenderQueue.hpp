#pragma once
#include "HodEngine/Renderer/Export.hpp"
#include "HodEngine/Core/Singleton.hpp"

#include "HodEngine/Core/Rect.hpp"
#include "HodEngine/Core/Math/Matrix4.hpp"

#include "HodEngine/Core/Vector.hpp"

namespace hod::renderer
{
	class RenderCommand;
	class RenderTarget;
	class Context;
	class MaterialInstance;
	class Semaphore;
	class Fence;
	class CommandBuffer;

	/// @brief 
	class HOD_RENDERER_API RenderQueue
	{
	public:
										RenderQueue() = default;
										~RenderQueue();

		void							Init();
		void							Terminate();

		bool							Prepare(Context* context);
		void							Prepare(RenderTarget* renderTarget, RenderTarget* pickingRenderTarget);
		uint32_t						GetRenderWidth() const;
		uint32_t						GetRenderHeight() const;

		void							SetupCamera(const Matrix4& projection, const Matrix4& view, const Rect& viewport);

		void							PushRenderCommand(RenderCommand* renderCommand);
		void							Execute();
		void							Wait();

	private:

		Vector<RenderCommand*>		_renderCommands;

		MaterialInstance*				_pickingMaterialInstance = nullptr;

		Context*						_context = nullptr;
		RenderTarget*					_renderTarget = nullptr;
		RenderTarget*					_pickingRenderTarget = nullptr;

		Semaphore*						_imageAvailableSemaphore = nullptr;
		Semaphore*						_renderFinishedSemaphore = nullptr;
		Fence*							_renderFinishedFence = nullptr;

		Vector<CommandBuffer*>		_commandBuffers;

		Matrix4							_projection;
		Matrix4							_view;
		Rect							_viewport;
	};
}
