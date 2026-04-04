#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Math/Rect.hpp"
#include "HodEngine/Math/Matrix4.hpp"

#include "HodEngine/Core/Vector.hpp"

namespace hod::renderer
{
	class RenderCommand;
	class RenderView;
	class CommandBuffer;
	class MaterialInstance;

	/// @brief 
	class HOD_RENDERER_API RenderQueue
	{
	public:
										RenderQueue() = default;
		virtual							~RenderQueue();

		void							Init();
		void							Terminate();

		void							PushRenderCommand(RenderCommand* renderCommand);

		virtual bool					Prepare(const RenderView& renderView) = 0;
		void							Execute(CommandBuffer* commandBuffer, MaterialInstance* overrideMaterial = nullptr);
		void							Clear();

	protected:

		math::Matrix4							_projection;
		math::Matrix4							_view;
		math::Rect							_viewport;

		Vector<RenderCommand*>			_renderCommands;
	};
}
