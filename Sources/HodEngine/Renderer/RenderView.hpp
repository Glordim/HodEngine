#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RenderQueue/WorldRenderQueue.hpp"
#include "HodEngine/Renderer/RenderQueue/UIRenderQueue.hpp"

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
	class HOD_RENDERER_API RenderView
	{
	public:

		enum class RenderQueueType
		{
			World,
			UI,
		};

	public:
										RenderView() = default;
										~RenderView();

		void							Init();
		void							Terminate();

		bool							Prepare(Context* context);
		void							Prepare(RenderTarget* renderTarget, RenderTarget* pickingRenderTarget);
		Vector2							GetRenderResolution() const;

		void							SetupCamera(const Matrix4& projection, const Matrix4& view, const Rect& viewport);
		const Matrix4&					GetViewMatrix() const;
		const Matrix4&					GetProjectionMatrix() const;
		const Rect&						GetViewport() const;

		void							PushRenderCommand(RenderCommand* renderCommand, RenderQueueType renderQueueType = RenderQueueType::World);
		void							Execute();
		void							Wait();

		void							DeleteAfter(MaterialInstance* materialInstance);

		void							SetAutoDestroy(bool autoDestroy);
		bool							IsAutoDestroy() const;

	private:

		WorldRenderQueue				_worldRenderQueue;
		UIRenderQueue					_uiRenderQueue;

		MaterialInstance*				_pickingMaterialInstance = nullptr;

		Context*						_context = nullptr;
		RenderTarget*					_renderTarget = nullptr;
		RenderTarget*					_pickingRenderTarget = nullptr;

		Semaphore*						_renderFinishedSemaphore = nullptr;
		Fence*							_renderFinishedFence = nullptr;

		Vector<CommandBuffer*>			_commandBuffers;

		Matrix4							_projection;
		Matrix4							_view;
		Rect							_viewport;

		Vector<MaterialInstance*>		_materialInstancesToDelete;
		bool							_autoDestroy = true;
	};
}
