#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/Material.hpp"

#include "HodEngine/RHI/RhiDevice.hpp"
#include "HodEngine/RHI/ShaderSetDescriptor.hpp"

#include <cstdlib>

namespace hod::inline renderer
{
	namespace
	{
		void CollectUniformLocations(const ShaderSetDescriptor::BlockUbo::Member& member, const String& path, Material::UniformLocation location,
		                             std::unordered_map<String, Material::UniformLocation>& locations)
		{
			location._size = (uint32_t)member._size;
			locations.emplace(path, location);

			for (const auto& childPair : member._childsMap)
			{
				Material::UniformLocation childLocation = location;
				childLocation._offset += (uint32_t)childPair.second._offset;
				CollectUniformLocations(childPair.second, path + "." + childPair.first, childLocation, locations);
			}
		}
	}

	/// @brief
	/// @return nullptr if the pipeline can't be built
	Material* Material::Create(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode,
	                           Topololy topololy, bool useDepth)
	{
		Material* material = DefaultAllocator::GetInstance().New<Material>();
		if (material->Build(vertexInputs, vertexInputCount, vertexShader, fragmentShader, polygonMode, topololy, useDepth) == false)
		{
			DefaultAllocator::GetInstance().Delete(material);
			return nullptr;
		}
		return material;
	}

	/// @brief
	Material::~Material()
	{
		DefaultAllocator::GetInstance().Delete(_graphicsPipeline);
	}

	/// @brief
	bool Material::Build(const VertexInput* vertexInputs, uint32_t vertexInputCount, Shader* vertexShader, Shader* fragmentShader, PolygonMode polygonMode,
	                     Topololy topololy, bool useDepth)
	{
		_graphicsPipeline = RhiDevice::GetInstance()->CreateGraphicsPipeline(vertexInputs, vertexInputCount, vertexShader, fragmentShader, polygonMode, topololy, useDepth);
		if (_graphicsPipeline == nullptr)
		{
			return false;
		}

		for (const auto& setPair : _graphicsPipeline->GetSetDescriptors())
		{
			const Vector<ShaderSetDescriptor::BlockUbo>& uboBlocks = setPair.second->GetUboBlocks();
			for (uint32_t blockIndex = 0; blockIndex < uboBlocks.Size(); ++blockIndex)
			{
				UniformLocation location;
				location._set = setPair.first;
				location._block = blockIndex;
				CollectUniformLocations(uboBlocks[blockIndex]._rootMember, uboBlocks[blockIndex]._name, location, _uniformLocations);
			}

			const Vector<ShaderSetDescriptor::BlockTexture>& textureBlocks = setPair.second->GetTextureBlocks();
			for (uint32_t blockIndex = 0; blockIndex < textureBlocks.Size(); ++blockIndex)
			{
				TextureLocation location;
				location._set = setPair.first;
				location._block = blockIndex;
				_textureLocations.emplace(textureBlocks[blockIndex]._name, location);
			}
		}

		return true;
	}

	/// @brief
	/// @return
	GraphicsPipeline* Material::GetGraphicsPipeline() const
	{
		return _graphicsPipeline;
	}

	/// @brief
	/// @return
	const std::map<uint32_t, ShaderSetDescriptor*>& Material::GetSetDescriptors() const
	{
		return _graphicsPipeline->GetSetDescriptors();
	}

	/// @brief
	/// @param path
	/// @param location
	/// @return
	bool Material::FindUniform(const String& path, UniformLocation& location) const
	{
		auto it = _uniformLocations.find(path);
		if (it != _uniformLocations.end())
		{
			location = it->second;
			return true;
		}

		// Only the paths indexing an array are not known in advance
		if (path.FindFirstOf("[") == String::Npos)
		{
			return false;
		}
		return ResolveUniform(path, location);
	}

	/// @brief Walks the reflection along a path, one ".member" or "[index]" at a time
	/// @param path
	/// @param location
	/// @return
	bool Material::ResolveUniform(const String& path, UniformLocation& location) const
	{
		String blockName;
		String remaining;

		size_t separator = path.FindFirstOf(".[");
		if (separator == String::Npos)
		{
			blockName = path;
		}
		else
		{
			blockName = path.SubStr(0, separator);
			remaining = path.SubStr(separator);
		}

		for (const auto& setPair : _graphicsPipeline->GetSetDescriptors())
		{
			const Vector<ShaderSetDescriptor::BlockUbo>& uboBlocks = setPair.second->GetUboBlocks();
			for (uint32_t blockIndex = 0; blockIndex < uboBlocks.Size(); ++blockIndex)
			{
				const ShaderSetDescriptor::BlockUbo& ubo = uboBlocks[blockIndex];
				if (ubo._name != blockName)
				{
					continue;
				}

				size_t                                       offset = 0;
				const ShaderSetDescriptor::BlockUbo::Member* member = &ubo._rootMember;

				while (remaining.Empty() == false)
				{
					if (remaining[0] == '.')
					{
						String memberName;
						size_t next = remaining.FindFirstOf(".[", 1);
						if (next == String::Npos)
						{
							memberName = remaining.SubStr(1);
							remaining = "";
						}
						else
						{
							memberName = remaining.SubStr(1, next - 1);
							remaining = remaining.SubStr(next);
						}

						auto it = member->_childsMap.find(memberName);
						if (it == member->_childsMap.end())
						{
							return false;
						}

						member = &it->second;
						offset += member->_offset;
					}
					else if (remaining[0] == '[')
					{
						size_t close = remaining.Find(']', 1);
						if (close == String::Npos)
						{
							return false;
						}
						String index = remaining.SubStr(1, close - 1);

						offset += member->_size * std::atoi(index.CStr());
						remaining = remaining.SubStr(close + 1);
					}
					else
					{
						return false;
					}
				}

				location._set = setPair.first;
				location._block = blockIndex;
				location._offset = (uint32_t)offset;
				location._size = (uint32_t)member->_size;
				return true;
			}
		}

		return false;
	}

	/// @brief
	/// @param name
	/// @param location
	/// @return
	bool Material::FindTexture(const String& name, TextureLocation& location) const
	{
		auto it = _textureLocations.find(name);
		if (it == _textureLocations.end())
		{
			return false;
		}
		location = it->second;
		return true;
	}

	/// @brief
	/// @param name
	void Material::ReportUnsetTexture(const String& name) const
	{
		if (_graphicsPipeline->HasReportedUnsetTexture() == false)
		{
			_graphicsPipeline->ReportUnsetTexture(name);
		}
	}
}
