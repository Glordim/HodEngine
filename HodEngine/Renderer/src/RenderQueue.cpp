#include "RenderQueue.h"

#include "RenderCommand/RenderCommand.h"
#include "RHI/CommandBuffer.h"

#include "Renderer.h"

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RenderQueue::RenderQueue()
		{
			_hdriMat = nullptr;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RenderQueue::~RenderQueue()
		{
			size_t meshCount = _meshList.size();
			for (size_t i = 0; i < meshCount; ++i)
			{
				delete _meshList[i];
			}

			size_t lineCount = _lineList.size();
			for (size_t i = 0; i < lineCount; ++i)
			{
				delete _lineList[i];
			}

			size_t triCount = _triangleList.size();
			for (size_t i = 0; i < triCount; ++i)
			{
				delete _triangleList[i];
			}

			size_t lightCount = _pointLightList.size();
			for (size_t i = 0; i < lightCount; ++i)
			{
				delete _pointLightList[i];
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderQueue::SetViewMatrix(const glm::mat4x4& viewMatrix)
		{
			_viewMatrix = viewMatrix;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const glm::mat4x4& RenderQueue::GetViewMatrix() const
		{
			return _viewMatrix;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderQueue::SetProjMatrix(const glm::mat4x4& projMatrix)
		{
			_projMatrix = projMatrix;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const glm::mat4x4& RenderQueue::GetProjMatrix() const
		{
			return _projMatrix;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderQueue::SetCameraPos(const glm::vec3& cameraPos)
		{
			_cameraPos = cameraPos;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const glm::vec3 RenderQueue::GetCameraPos() const
		{
			return _cameraPos;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderQueue::SetClearFlag(int clearFlags)
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderQueue::SetHdriMaterial(MaterialInstance* hdriMat)
		{
			_hdriMat = hdriMat;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		MaterialInstance* RenderQueue::GetHdriMaterial() const
		{
			return _hdriMat;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderQueue::SetHdriTexture(Texture* hdriTexture)
		{
			_hdriTexture = hdriTexture;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Texture* RenderQueue::GetHdriTexture() const
		{
			return _hdriTexture;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderQueue::AddMesh(Mesh* mesh, MaterialInstance* materialInstance, glm::mat4x4 matrix)
		{
			_meshList.push_back(new MeshData(mesh, materialInstance, matrix));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderQueue::AddLines(std::vector<Line_3P_3C> lines, MaterialInstance* materialInstance, glm::mat4x4 matrix)
		{
			_lineList.push_back(new LineData(lines, materialInstance, matrix));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderQueue::AddTriangles(std::vector<Tri_3P_3C> tris, MaterialInstance* materialInstance, glm::mat4x4 matrix)
		{
			_triangleList.push_back(new TriangleData(tris, materialInstance, matrix));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderQueue::AddDirLight(const glm::vec3& dir, DirLight* dirLight)
		{
			_dirLightList.push_back(new DirLightData(dir, dirLight));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderQueue::AddPointLight(const glm::vec3& pos, PointLight* pointLight)
		{
			_pointLightList.push_back(new PointLightData(pos, pointLight));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderQueue::AddSpotLight(const glm::vec3& pos, const glm::vec3& dir, SpotLight* spotLight)
		{
			_spotLightList.push_back(new SpotLightData(pos, dir, spotLight));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderQueue::PushRenderCommand(RenderCommand* renderCommand)
		{
			_renderCommands.push_back(renderCommand);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderQueue::Execute()
		{
			Renderer* renderer = Renderer::GetInstance();

			CommandBuffer* commandBuffer = renderer->CreateCommandBuffer();

			commandBuffer->StartRecord();
			for (RenderCommand* renderCommand : _renderCommands)
			{
				renderCommand->Execute(commandBuffer);
				delete renderCommand;
			}

			commandBuffer->EndRecord();

			_renderCommands.clear();

			renderer->SubmitCommandBuffers(&commandBuffer, 1);
		}
	}
}
