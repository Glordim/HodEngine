#include "HodEngine/Renderer/Pch.hpp"
#include "HodEngine/Renderer/SpriteAtlas.hpp"

#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/RHI/Texture.hpp"

#include <HodEngine/Core/Output/OutputService.hpp>

#include <fstream>
#include <sstream>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		SpriteAtlas::~SpriteAtlas()
		{
			if (_texture != nullptr)
			{
				DefaultAllocator::GetInstance().Delete(_texture);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool SpriteAtlas::LoadFromFile(const String& spriteAltasJsonPath)
		{
			return false;
			/*
			std::ifstream jsonFile;
			jsonFile.open(spriteAltasJsonPath, std::ifstream::in);
			if (jsonFile.is_open() == false)
			{
				OUTPUT_ERROR("Unable to open SpriteAtlas : {}", spriteAltasJsonPath.data());
			}

			std::ostringstream sstr;
			sstr << jsonFile.rdbuf();
			String jsonBuffer = sstr.str();
			
			jsonFile.close();

			rapidjson::Document documentJson;
			documentJson.Parse(jsonBuffer.data());

			auto metaElement = documentJson["meta"].GetObject();

			String texturePath = spriteAltasJsonPath;
			size_t lastSlash = texturePath.find_last_of('/');
			texturePath = texturePath.substr(0, lastSlash + 1) + metaElement["image"].GetString();

			_texture = Renderer::GetInstance()->CreateTexture();
			_texture->LoadFromPath(texturePath.data());

			auto frameArray = documentJson["frames"].GetArray();
			_sprites.resize(frameArray.Size());

			for (uint32_t frameIndex = 0; frameIndex < _sprites.size(); ++frameIndex)
			{
				auto frameElement = frameArray[frameIndex].GetObject();
				Sprite& sprite = _sprites[frameIndex];
				sprite._name = frameElement["filename"].GetString();
				sprite._frame._position.x = frameElement["frame"]["x"].GetInt();
				sprite._frame._position.y = frameElement["frame"]["y"].GetInt();
				sprite._frame._size.x = frameElement["frame"]["w"].GetInt();
				sprite._frame._size.y = frameElement["frame"]["h"].GetInt();
				sprite._size.x = frameElement["sourceSize"]["w"].GetInt();
				sprite._size.y = frameElement["sourceSize"]["h"].GetInt();
				sprite._pivot.x = frameElement["pivot"]["x"].GetFloat();
				sprite._pivot.y = frameElement["pivot"]["y"].GetFloat();

				auto verticeArray = frameElement["vertices"].GetArray();
				auto verticeUvArray = frameElement["verticesUV"].GetArray();

				sprite._vertices.resize(verticeArray.Size());
				for (uint32_t vertexIndex = 0; vertexIndex < sprite._vertices.size(); ++vertexIndex)
				{
					P2fT2f& vertex = sprite._vertices[vertexIndex];
					vertex._position[0] = static_cast<float>(verticeArray[vertexIndex].GetArray()[0].GetInt());
					vertex._position[1] = static_cast<float>(-verticeArray[vertexIndex].GetArray()[1].GetInt());
					vertex._position[0] -= sprite._size.x * sprite._pivot.x;
					vertex._position[1] += sprite._size.y * sprite._pivot.y;
					vertex._position[0] *= 0.01f;
					vertex._position[1] *= 0.01f;
					vertex._textCoord[0] = static_cast<float>(verticeUvArray[vertexIndex].GetArray()[0].GetInt() / static_cast<float>(_texture->GetWidth()));
					vertex._textCoord[1] = static_cast<float>(-verticeUvArray[vertexIndex].GetArray()[1].GetInt() / static_cast<float>(_texture->GetHeight()));
				}

				auto triangleArray = frameElement["triangles"].GetArray();

				sprite._indices.resize(triangleArray.Size() * 3);
				for (uint32_t triangleIndex = 0; triangleIndex < triangleArray.Size(); ++triangleIndex)
				{
					sprite._indices[(triangleIndex * 3) + 0] = triangleArray[triangleIndex].GetArray()[0].GetInt();
					sprite._indices[(triangleIndex * 3) + 1] = triangleArray[triangleIndex].GetArray()[1].GetInt();
					sprite._indices[(triangleIndex * 3) + 2] = triangleArray[triangleIndex].GetArray()[2].GetInt();
				}

				sprite._spriteAtlas = this;
				sprite.RebuildBoundingBox();
			}

			return true;
			*/
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const Sprite* SpriteAtlas::FindSprite(const String& spriteName) const
		{
			for (const Sprite& sprite : _sprites)
			{
				if (sprite.GetName() == spriteName)
				{
					return &sprite;
				}
			}

			return nullptr;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const Texture* SpriteAtlas::GetTexture() const
		{
			return _texture;
		}
	}
}
