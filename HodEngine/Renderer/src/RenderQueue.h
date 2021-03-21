#pragma once

#include <vector>

#include "RHI/Mesh.h"
#include "Line_3P_3C.h"
#include "Tri_3P_3C.h"
#include "Light/DirLight.h"
#include "Light/PointLight.h"
#include "Light/SpotLight.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/mat4x4.hpp"

namespace HOD
{
	namespace RENDERER
	{
		class MaterialInstance;
		class MaterialInstance;

		class RenderCommand;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class RenderQueue
		{
		public:

			enum ClearFlag
			{
				NONE,
				COLOR,
				DEPTH
			};

			//-----------------------------------------------------------------------------
			//! @brief		
			//-----------------------------------------------------------------------------
			struct MeshData
			{
				MeshData(Mesh* mesh, MaterialInstance* materialInstance, glm::mat4x4 matrix)
					: _mesh(mesh)
					, _materialInstance(materialInstance)
					, _matrix(matrix)
				{

				}

				Mesh*				_mesh;
				MaterialInstance*	_materialInstance;
				glm::mat4x4			_matrix;
			};

			//-----------------------------------------------------------------------------
			//! @brief		
			//-----------------------------------------------------------------------------
			struct LineData
			{
				LineData(std::vector<Line_3P_3C>& lines, MaterialInstance* materialInstance, glm::mat4x4& matrix)
					: _mesh(nullptr)
					, _materialInstance(materialInstance)
					, _matrix(matrix)
				{
					/*
					Renderer* renderer = Renderer::GetInstance();

					_mesh = renderer->CreateMesh("");
					_mesh->LoadFromLines(lines);
					*/
				}

				~LineData()
				{
					if (_mesh != nullptr)
					{
						delete _mesh;
					}
				}

				Mesh*				_mesh;
				MaterialInstance*	_materialInstance;
				glm::mat4x4			_matrix;
			};

			//-----------------------------------------------------------------------------
			//! @brief		
			//-----------------------------------------------------------------------------
			struct TriangleData
			{
				TriangleData(std::vector<Tri_3P_3C>& triangles, MaterialInstance* materialInstance, glm::mat4x4& matrix)
					: _mesh(nullptr)
					, _materialInstance(materialInstance)
					, _matrix(matrix)
				{
					/*
					Renderer* renderer = Renderer::GetInstance();

					_mesh = renderer->CreateMesh("");
					_mesh->LoadFromTriangles(triangles);
					*/
				}

				~TriangleData()
				{
					if (_mesh != nullptr)
					{
						delete _mesh;
					}
				}

				Mesh*				_mesh;
				MaterialInstance*	_materialInstance;
				glm::mat4x4			_matrix;
			};

			//-----------------------------------------------------------------------------
			//! @brief		
			//-----------------------------------------------------------------------------
			struct DirLightData
			{
				DirLightData(const glm::vec3& dir, DirLight* dirLight)
					: _dir(dir)
					, _dirLight(dirLight)
				{

				}

				glm::vec3	_dir;
				DirLight*	_dirLight;
			};

			//-----------------------------------------------------------------------------
			//! @brief		
			//-----------------------------------------------------------------------------
			struct PointLightData
			{
				PointLightData(const glm::vec3& pos, PointLight* pointLight)
					: _pos(pos)
					, _pointLight(pointLight)
				{

				}

				glm::vec3	_pos;
				PointLight*	_pointLight;
			};

			//-----------------------------------------------------------------------------
			//! @brief		
			//-----------------------------------------------------------------------------
			struct SpotLightData
			{
				SpotLightData(const glm::vec3& pos, const glm::vec3& dir, SpotLight* spotLight)
					: _pos(pos)
					, _dir(dir)
					, _spotLight(spotLight)
				{

				}

				glm::vec3	_pos;
				glm::vec3	_dir;
				SpotLight*	_spotLight;
			};

								RenderQueue();
			virtual				~RenderQueue();

			void				SetViewMatrix(const glm::mat4x4& viewMatrix);
			const				glm::mat4x4& GetViewMatrix() const;

			void				SetProjMatrix(const glm::mat4x4& projMatrix);
			const				glm::mat4x4& GetProjMatrix() const;

			void				SetCameraPos(const glm::vec3& cameraPos);
			const				glm::vec3 GetCameraPos() const;

			void				SetClearFlag(int clearFlags);

			void				SetHdriMaterial(MaterialInstance* hdriMat);
			MaterialInstance*	GetHdriMaterial() const;

			void				SetHdriTexture(Texture* hdriTexture);
			Texture*			GetHdriTexture() const;

			void				AddMesh(Mesh* mesh, MaterialInstance* materialInstance, glm::mat4x4 matrix);
			void				AddLines(std::vector<Line_3P_3C> lines, MaterialInstance* materialInstance, glm::mat4x4 matrix);
			void				AddTriangles(std::vector<Tri_3P_3C> tris, MaterialInstance* materialInstance, glm::mat4x4 matrix);

			void				AddDirLight(const glm::vec3& dir, DirLight* dirLight);
			void				AddPointLight(const glm::vec3& pos, PointLight* pointLight);
			void				AddSpotLight(const glm::vec3& pos, const glm::vec3& dir, SpotLight* spotLight);

			const std::vector<MeshData*>& GetMeshDatas() const
			{
				return _meshList;
			}

			const std::vector<LineData*>& GetLineDatas() const
			{
				return _lineList;
			}

			const std::vector<TriangleData*>& GetTriangleDatas() const
			{
				return _triangleList;
			}

			const std::vector<DirLightData*>& GetDirLightDatas() const
			{
				return _dirLightList;
			}

			const std::vector<PointLightData*>& GetPointLightDatas() const
			{
				return _pointLightList;
			}

			const std::vector<SpotLightData*>& GetSpotLightDatas() const
			{
				return _spotLightList;
			}

			void							PushRenderCommand(RenderCommand* renderCommand);
			void							Execute();

		private:

			glm::mat4x4						_viewMatrix;
			glm::mat4x4						_projMatrix;
			glm::vec3						_cameraPos;

			MaterialInstance*				_hdriMat;
			Texture*						_hdriTexture;

			std::vector<MeshData*>			_meshList;
			std::vector<LineData*>			_lineList;
			std::vector<TriangleData*>		_triangleList;

			std::vector<DirLightData*>		_dirLightList;
			std::vector<PointLightData*>	_pointLightList;
			std::vector<SpotLightData*>		_spotLightList;

			std::vector<RenderCommand*>		_renderCommands;
		};
	}
}
