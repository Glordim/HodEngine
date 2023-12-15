#pragma once

#include "HodEngine/Renderer/RenderCommand/RenderCommand.hpp"
#include "HodEngine/Renderer/P2fT2f.hpp"

#include <HodEngine/Core/Rect.hpp>

#include <vector>

namespace hod::renderer
{
	class Texture;
	class Material;
	class MaterialInstance;
}

namespace hod::imgui
{
	/// @brief 
	class RenderCommandImGui : public renderer::RenderCommand
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

			std::vector<Vertex>		_vertices;
			std::vector<uint16_t>	_indices;
			std::vector<Command>	_commands;
		};

	public:

									RenderCommandImGui(const std::vector<DrawList*>& drawLists, const Rect& viewport);
									RenderCommandImGui(const RenderCommandImGui&) = delete;
									RenderCommandImGui(RenderCommandImGui&&) = delete;
									~RenderCommandImGui() override = default;

		void						operator=(const RenderCommandImGui&) = delete;
		void						operator=(RenderCommandImGui&&) = delete;

	public:

		void						Execute(renderer::CommandBuffer* commandBuffer) override;

	private:

		Rect						_viewport;
		std::vector<DrawList*>		_drawLists;
	};
}
