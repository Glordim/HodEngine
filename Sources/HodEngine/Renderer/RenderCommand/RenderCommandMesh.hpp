#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Renderer/RenderCommand/RenderCommand.hpp"
#include "HodEngine/Renderer/P2fT2f.hpp"
#include "HodEngine/Core/Math/Matrix4.hpp"

#include <HodEngine/Core/Math/Vector2.hpp>
#include <HodEngine/Core/Color.hpp>

#include <vector>

namespace hod::renderer
{
	class MaterialInstance;

	/// @brief 
	class HOD_RENDERER_API RenderCommandMesh : public RenderCommand
	{
	public:

								RenderCommandMesh(const Vector2* positions, const Vector2* uvs, const Color* colors, uint32_t vertexCount, const uint16_t* indices, uint32_t indexCount, const Matrix4& modelMatrix, const MaterialInstance* materialInstance, bool ignoreVisualisationMode = false);
								RenderCommandMesh(const RenderCommandMesh&) = delete;
								RenderCommandMesh(RenderCommandMesh&&) = delete;
								~RenderCommandMesh() override = default;

		void					operator=(const RenderCommandMesh&) = delete;
		void					operator=(RenderCommandMesh&&) = delete;

	public:

		void					Execute(CommandBuffer* commandBuffer) override;

	protected:

		std::vector<Vector2>	_positions;
		std::vector<Vector2>	_uvs;
		std::vector<Color>		_colors;
		uint32_t				_vertexCount;

		std::vector<uint16_t>	_indices;
		Matrix4					_modelMatrix;
		const MaterialInstance*	_materialInstance = nullptr;
		bool					_ignoreVisualisationMode;
	};
}
