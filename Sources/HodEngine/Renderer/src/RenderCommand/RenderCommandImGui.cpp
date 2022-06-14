
#include "RenderCommandImGui.h"

#include "RHI/Buffer.h"
#include "RHI/CommandBuffer.h"

#include "Renderer.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include <cstring>

namespace HOD
{
	namespace RENDERER
	{
		// glsl_shader.vert, compiled with:
		// # glslangValidator -V -x -o glsl_shader.vert.u32 glsl_shader.vert
		/*
		#version 450 core
		layout(location = 0) in vec2 aPos;
		layout(location = 1) in vec2 aUV;
		layout(location = 2) in vec4 aColor;
		layout(push_constant) uniform uPushConstant { vec2 uScale; vec2 uTranslate; } pc;

		out gl_PerVertex { vec4 gl_Position; };
		layout(location = 0) out struct { vec4 Color; vec2 UV; } Out;

		void main()
		{
			Out.Color = aColor;
			Out.UV = aUV;
			gl_Position = vec4(aPos * pc.uScale + pc.uTranslate, 0, 1);
		}
		*/
		static uint32_t __glsl_shader_vert_spv[] =
		{
			0x07230203,0x00010300,0x0008000a,0x0000002e,0x00000000,0x00020011,0x00000001,0x0006000b,
			0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
			0x000a000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000b,0x0000000f,0x00000015,
			0x0000001b,0x0000001c,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
			0x00000000,0x00030005,0x00000009,0x00000000,0x00050006,0x00000009,0x00000000,0x6f6c6f43,
			0x00000072,0x00040006,0x00000009,0x00000001,0x00005655,0x00030005,0x0000000b,0x0074754f,
			0x00040005,0x0000000f,0x6c6f4361,0x0000726f,0x00030005,0x00000015,0x00565561,0x00060005,
			0x00000019,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x00000019,0x00000000,
			0x505f6c67,0x7469736f,0x006e6f69,0x00030005,0x0000001b,0x00000000,0x00040005,0x0000001c,
			0x736f5061,0x00000000,0x00060005,0x0000001e,0x73755075,0x6e6f4368,0x6e617473,0x00000074,
			0x00050006,0x0000001e,0x00000000,0x61635375,0x0000656c,0x00060006,0x0000001e,0x00000001,
			0x61725475,0x616c736e,0x00006574,0x00030005,0x00000020,0x00006370,0x00040047,0x0000000b,
			0x0000001e,0x00000000,0x00040047,0x0000000f,0x0000001e,0x00000002,0x00040047,0x00000015,
			0x0000001e,0x00000001,0x00050048,0x00000019,0x00000000,0x0000000b,0x00000000,0x00030047,
			0x00000019,0x00000002,0x00040047,0x0000001c,0x0000001e,0x00000000,0x00050048,0x0000001e,
			0x00000000,0x00000023,0x00000000,0x00050048,0x0000001e,0x00000001,0x00000023,0x00000008,
			0x00030047,0x0000001e,0x00000002,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
			0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040017,
			0x00000008,0x00000006,0x00000002,0x0004001e,0x00000009,0x00000007,0x00000008,0x00040020,
			0x0000000a,0x00000003,0x00000009,0x0004003b,0x0000000a,0x0000000b,0x00000003,0x00040015,
			0x0000000c,0x00000020,0x00000001,0x0004002b,0x0000000c,0x0000000d,0x00000000,0x00040020,
			0x0000000e,0x00000001,0x00000007,0x0004003b,0x0000000e,0x0000000f,0x00000001,0x00040020,
			0x00000011,0x00000003,0x00000007,0x0004002b,0x0000000c,0x00000013,0x00000001,0x00040020,
			0x00000014,0x00000001,0x00000008,0x0004003b,0x00000014,0x00000015,0x00000001,0x00040020,
			0x00000017,0x00000003,0x00000008,0x0003001e,0x00000019,0x00000007,0x00040020,0x0000001a,
			0x00000003,0x00000019,0x0004003b,0x0000001a,0x0000001b,0x00000003,0x0004003b,0x00000014,
			0x0000001c,0x00000001,0x0004001e,0x0000001e,0x00000008,0x00000008,0x00040020,0x0000001f,
			0x00000009,0x0000001e,0x0004003b,0x0000001f,0x00000020,0x00000009,0x00040020,0x00000021,
			0x00000009,0x00000008,0x0004002b,0x00000006,0x00000028,0x00000000,0x0004002b,0x00000006,
			0x00000029,0x3f800000,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
			0x00000005,0x0004003d,0x00000007,0x00000010,0x0000000f,0x00050041,0x00000011,0x00000012,
			0x0000000b,0x0000000d,0x0003003e,0x00000012,0x00000010,0x0004003d,0x00000008,0x00000016,
			0x00000015,0x00050041,0x00000017,0x00000018,0x0000000b,0x00000013,0x0003003e,0x00000018,
			0x00000016,0x0004003d,0x00000008,0x0000001d,0x0000001c,0x00050041,0x00000021,0x00000022,
			0x00000020,0x0000000d,0x0004003d,0x00000008,0x00000023,0x00000022,0x00050085,0x00000008,
			0x00000024,0x0000001d,0x00000023,0x00050041,0x00000021,0x00000025,0x00000020,0x00000013,
			0x0004003d,0x00000008,0x00000026,0x00000025,0x00050081,0x00000008,0x00000027,0x00000024,
			0x00000026,0x00050051,0x00000006,0x0000002a,0x00000027,0x00000000,0x00050051,0x00000006,
			0x0000002b,0x00000027,0x00000001,0x00070050,0x00000007,0x0000002c,0x0000002a,0x0000002b,
			0x00000028,0x00000029,0x00050041,0x00000011,0x0000002d,0x0000001b,0x0000000d,0x0003003e,
			0x0000002d,0x0000002c,0x000100fd,0x00010038
		};

		// glsl_shader.frag, compiled with:
		// # glslangValidator -V -x -o glsl_shader.frag.u32 glsl_shader.frag
		/*
		#version 450 core
		layout(location = 0) out vec4 fColor;
		layout(set=0, binding=0) uniform sampler2D sTexture;
		layout(location = 0) in struct { vec4 Color; vec2 UV; } In;
		void main()
		{
			fColor = In.Color * texture(sTexture, In.UV.st);
		}
		*/
		static uint32_t __glsl_shader_frag_spv[] =
		{
			0x07230203,0x00010300,0x0008000a,0x0000001e,0x00000000,0x00020011,0x00000001,0x0006000b,
			0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
			0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000d,0x00030010,
			0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
			0x00000000,0x00040005,0x00000009,0x6c6f4366,0x0000726f,0x00030005,0x0000000b,0x00000000,
			0x00050006,0x0000000b,0x00000000,0x6f6c6f43,0x00000072,0x00040006,0x0000000b,0x00000001,
			0x00005655,0x00030005,0x0000000d,0x00006e49,0x00050005,0x00000016,0x78655473,0x65727574,
			0x00000000,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000d,0x0000001e,
			0x00000000,0x00040047,0x00000016,0x00000022,0x00000000,0x00040047,0x00000016,0x00000021,
			0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
			0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,
			0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x00040017,0x0000000a,0x00000006,
			0x00000002,0x0004001e,0x0000000b,0x00000007,0x0000000a,0x00040020,0x0000000c,0x00000001,
			0x0000000b,0x0004003b,0x0000000c,0x0000000d,0x00000001,0x00040015,0x0000000e,0x00000020,
			0x00000001,0x0004002b,0x0000000e,0x0000000f,0x00000000,0x00040020,0x00000010,0x00000001,
			0x00000007,0x00090019,0x00000013,0x00000006,0x00000001,0x00000000,0x00000000,0x00000000,
			0x00000001,0x00000000,0x0003001b,0x00000014,0x00000013,0x00040020,0x00000015,0x00000000,
			0x00000014,0x0004003b,0x00000015,0x00000016,0x00000000,0x0004002b,0x0000000e,0x00000018,
			0x00000001,0x00040020,0x00000019,0x00000001,0x0000000a,0x00050036,0x00000002,0x00000004,
			0x00000000,0x00000003,0x000200f8,0x00000005,0x00050041,0x00000010,0x00000011,0x0000000d,
			0x0000000f,0x0004003d,0x00000007,0x00000012,0x00000011,0x0004003d,0x00000014,0x00000017,
			0x00000016,0x00050041,0x00000019,0x0000001a,0x0000000d,0x00000018,0x0004003d,0x0000000a,
			0x0000001b,0x0000001a,0x00050057,0x00000007,0x0000001c,0x00000017,0x0000001b,0x00050085,
			0x00000007,0x0000001d,0x00000012,0x0000001c,0x0003003e,0x00000009,0x0000001d,0x000100fd,
			0x00010038
		};

		Material* RenderCommandImGui::_material = nullptr;
		MaterialInstance* RenderCommandImGui::_materialInstance = nullptr;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RenderCommandImGui::RenderCommandImGui(const std::vector<DrawList*>& drawLists)
			: RenderCommand()
			, _drawLists(drawLists)
		{
		}

		// TODO delete in destructor

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void RenderCommandImGui::Execute(CommandBuffer* commandBuffer)
		{
			Renderer* renderer = Renderer::GetInstance();

			if (_material == nullptr)
			{
				Shader* vertexShader = renderer->CreateShader(Shader::ShaderType::Vertex);
				vertexShader->LoadFromMemory((void*)__glsl_shader_vert_spv, sizeof(__glsl_shader_vert_spv));

				Shader* fragmentShader = renderer->CreateShader(Shader::ShaderType::Fragment);
				fragmentShader->LoadFromMemory((void*)__glsl_shader_frag_spv, sizeof(__glsl_shader_frag_spv));

				_material = renderer->CreateMaterial(vertexShader, fragmentShader);
				_materialInstance = renderer->CreateMaterialInstance(_material);
			}

			commandBuffer->SetMaterialInstance(_materialInstance, 0);
			commandBuffer->SetPushConstant();

			for (int drawListIndex = 0; drawListIndex < _drawLists.size(); ++drawListIndex)
			{
				DrawList* drawList = _drawLists[drawListIndex];

				Buffer* vertexBuffer = renderer->CreateBuffer(Buffer::Usage::Vertex);

				uint32_t vertexBufferSize = static_cast<uint32_t>(drawList->_vertices.size());
				if (vertexBuffer->Resize(vertexBufferSize) == false)
				{
					delete vertexBuffer;
					return;
				}

				void* vertexBufferData = vertexBuffer->Lock();
				if (vertexBufferData != nullptr)
				{
					memcpy(vertexBufferData, drawList->_vertices.data(), vertexBufferSize);
					vertexBuffer->Unlock();
				}
				commandBuffer->DeleteAfterRender(vertexBuffer);

				Buffer* indexBuffer = renderer->CreateBuffer(Buffer::Usage::Index);

				uint32_t indexBufferSize = static_cast<uint32_t>(drawList->_indices.size() * sizeof(uint16_t));
				if (indexBuffer->Resize(indexBufferSize) == false)
				{
					delete indexBuffer;
					return;
				}

				void* indexBufferData = indexBuffer->Lock();
				if (indexBufferData != nullptr)
				{
					memcpy(indexBufferData, drawList->_indices.data(), indexBufferSize);
					indexBuffer->Unlock();
				}
				commandBuffer->DeleteAfterRender(indexBuffer);

				for (int commandIndex = 0; commandIndex < drawList->_commands.size(); ++commandIndex)
				{
					Command& command = drawList->_commands[commandIndex];

					commandBuffer->SetIndexBuffer(indexBuffer, command._vertexOffset);
					commandBuffer->SetVertexBuffer(vertexBuffer, command._indexOffset);
					commandBuffer->SetScissor(command._clipRect);
					//commandBuffer->SetMaterialInstance(_materialInstance, 0);
					commandBuffer->DrawIndexed(command._elementCount);
				}
			}
		}
	}
}
