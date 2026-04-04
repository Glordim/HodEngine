#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RenderQueue/UIRenderQueue.hpp"
#include "HodEngine/Renderer/RenderQueue/WorldRenderQueue.hpp"

#include "HodEngine/Math/Matrix4.hpp"
#include "HodEngine/Math/Rect.hpp"

#include "HodEngine/Core/Vector.hpp"

namespace hod::window
{
	class Window;
}

namespace hod::renderer
{
	class RenderCommand;
	class RenderTarget;
	class PresentationSurface;
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

		void Init();
		void Terminate();

		bool    Prepare(PresentationSurface* presentationSurface);
		bool    Prepare(window::Window* window);
		void    Prepare(RenderTarget* renderTarget, RenderTarget* pickingRenderTarget);
		math::Vector2 GetRenderResolution() const;

		PresentationSurface* GetPresentationSurface() const;
		Semaphore*           GetRenderFinishedSemaphore() const;

		void           SetupCamera(const math::Matrix4& projection, const math::Matrix4& view, const math::Rect& viewport);
		const math::Matrix4& GetViewMatrix() const;
		const math::Matrix4& GetProjectionMatrix() const;
		const math::Rect&    GetViewport() const;

		void PushRenderCommand(RenderCommand* renderCommand, RenderQueueType renderQueueType = RenderQueueType::World);
		void Execute(Semaphore* previous = nullptr);
		void Wait();

		void DeleteAfter(MaterialInstance* materialInstance);

		void SetAutoDestroy(bool autoDestroy);
		bool IsAutoDestroy() const;

	private:
		WorldRenderQueue _worldRenderQueue;
		UIRenderQueue    _uiRenderQueue;

		MaterialInstance* _pickingMaterialInstance = nullptr;

		PresentationSurface* _presentationSurface = nullptr;
		RenderTarget*        _renderTarget = nullptr;
		RenderTarget*        _pickingRenderTarget = nullptr;

		Semaphore* _renderFinishedSemaphore = nullptr;
		Fence*     _renderFinishedFence = nullptr;

		Vector<CommandBuffer*> _commandBuffers;

		math::Matrix4 _projection;
		math::Matrix4 _view;
		math::Rect    _viewport;

		Vector<MaterialInstance*> _materialInstancesToDelete;
		bool                      _autoDestroy = true;
	};
}
