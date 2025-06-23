#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/Rect.hpp"
#include "HodEngine/Core/Math/Matrix4.hpp"

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

		Matrix4							_projection;
		Matrix4							_view;
		Rect							_viewport;

		Vector<RenderCommand*>			_renderCommands;
	};
}
