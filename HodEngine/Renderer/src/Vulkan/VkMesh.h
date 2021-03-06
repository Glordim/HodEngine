#pragma once

#include "../Mesh.h"

#include <vulkan/vulkan.h>

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class VkMesh : public Mesh
		{
		public:

							VkMesh();
			virtual			~VkMesh();

			VkBuffer		GetVertexBuffer() const;
			VkBuffer		GetIndiceBuffer() const;

		protected:

			virtual bool	BuildBuffers() override;

		private:

			VkBuffer		_vertexBuffer;
			VkDeviceMemory	_vertexBufferMemory;

			VkBuffer		_indiceBuffer;
			VkDeviceMemory	_indiceBufferMemory;
		};
	}
}
