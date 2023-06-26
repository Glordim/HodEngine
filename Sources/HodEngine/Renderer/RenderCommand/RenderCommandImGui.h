#pragma once

#include "HodEngine/Renderer/RenderCommand/RenderCommand.h"
#include "HodEngine/Renderer/P2fT2f.h"

#include <HodEngine/Core/Rect.h>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <vector>

namespace hod
{
	namespace renderer
	{
		class Texture;
		class Material;
		class MaterialInstance;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class RenderCommandImGui : public RenderCommand
		{
		public:

			struct Vertex
			{
				glm::vec2	_position;
				glm::vec2	_uv;
				uint32_t	_color;
			};

			struct Command
			{
				CORE::Rect	_clipRect;
				Texture*	_texture;
				uint32_t	_vertexOffset;
				uint32_t	_indexOffset;
				uint32_t	_elementCount;
			};

			struct DrawList
			{
				glm::vec2				_displayPosition;
				glm::vec2				_displaySize;

				std::vector<Vertex>		_vertices;
				std::vector<uint16_t>	_indices;
				std::vector<Command>	_commands;
			};

		public:

										RenderCommandImGui(const std::vector<DrawList*>& drawLists, const CORE::Rect& viewport);
										RenderCommandImGui(const RenderCommandImGui&) = delete;
										RenderCommandImGui(RenderCommandImGui&&) = delete;
										~RenderCommandImGui() override = default;

			void						operator=(const RenderCommandImGui&) = delete;
			void						operator=(RenderCommandImGui&&) = delete;

		public:

			void						Execute(CommandBuffer* commandBuffer) override;

		private:

			static Material*			_material;

			CORE::Rect					_viewport;
			std::vector<DrawList*>		_drawLists;
		};
	}
}
