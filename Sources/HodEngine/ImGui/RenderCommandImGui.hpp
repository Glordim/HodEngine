#pragma once
#include "HodEngine/ImGui/Export.hpp"

#include "HodEngine/Renderer/RenderCommand/RenderCommand.hpp"
#include "HodEngine/Renderer/P2fT2f.hpp"

#include <HodEngine/Core/Rect.hpp>

#include "HodEngine/Core/Vector.hpp"

namespace hod::renderer
{
	class Texture;
	class Material;
	class MaterialInstance;
}

namespace hod::imgui
{
	/// @brief 
	class HOD_IMGUI_API RenderCommandImGui : public renderer::RenderCommand
	{
	public:

		struct Vertex
		{
			Vector2		_position;
			Vector2		_uv;
			uint32_t	_color;
		};

		struct Command
		{
			Rect		_clipRect;
			renderer::Texture*	_texture;
			uint32_t	_vertexOffset;
			uint32_t	_indexOffset;
			uint32_t	_elementCount;
		};

		struct DrawList
		{
			Vector2				_displayPosition;
			Vector2				_displaySize;

			Vector<Vertex>		_vertices;
			Vector<uint16_t>	_indices;
			Vector<Command>	_commands;
		};

	public:

									RenderCommandImGui(const Vector<DrawList*>& drawLists, const Rect& viewport);
									RenderCommandImGui(const RenderCommandImGui&) = delete;
									RenderCommandImGui(RenderCommandImGui&&) = delete;
									~RenderCommandImGui() override;

		void						operator=(const RenderCommandImGui&) = delete;
		void						operator=(RenderCommandImGui&&) = delete;

	public:

		void						Execute(renderer::CommandBuffer* commandBuffer, renderer::MaterialInstance* overrideMaterial) override;
		uint32_t					GetRenderingOrder() const override;

	private:

		Rect						_viewport;
		Vector<DrawList*>		_drawLists;
	};
}
