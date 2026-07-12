#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/Hash.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Editor/MaterialEditor/MaterialImporter.hpp"

#include "HodEngine/Renderer/Resource/MaterialResource.hpp"

#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp"
#include "HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"
#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/Core/Process/Process.hpp>
#include <HodEngine/Core/ScopedGuard.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <unordered_set>

namespace hod::inline editor
{
	namespace
	{
		/// @brief One collected slang file: its content and its path relative to the root (imported) slang file
		struct SlangFileEntry
		{
			Path _relativePath;
			String _content;
		};

		/// @brief Find every "#include \"...\"" / "#include <...>" directive on its own line and resolve it against fileDir
		void ParseSlangIncludes(const String& content, const Path& fileDir, Vector<Path>& outIncludePaths)
		{
			static constexpr std::string_view includeDirective = "#include";

			uint32_t contentSize = content.Size();
			uint32_t pos = 0;
			while (pos < contentSize)
			{
				uint32_t lineEnd = content.Find('\n', pos);
				uint32_t realLineEnd = (lineEnd == String::Npos) ? contentSize : lineEnd;

				uint32_t lineStart = pos;
				while (lineStart < realLineEnd && (content[lineStart] == ' ' || content[lineStart] == '\t'))
				{
					++lineStart;
				}

				if (realLineEnd - lineStart >= includeDirective.size() && content.Compare(lineStart, includeDirective.size(), includeDirective) == 0)
				{
					uint32_t searchFrom = lineStart + includeDirective.size();
					uint32_t quotePos = content.Find('"', searchFrom);
					uint32_t anglePos = content.Find('<', searchFrom);
					bool quoteOnLine = quotePos != String::Npos && quotePos < realLineEnd;
					bool angleOnLine = anglePos != String::Npos && anglePos < realLineEnd;

					bool useAngle = angleOnLine && (quoteOnLine == false || anglePos < quotePos);
					if (useAngle || quoteOnLine)
					{
						uint32_t openPos = useAngle ? anglePos : quotePos;
						char closeChar = useAngle ? '>' : '"';
						uint32_t closePos = content.Find(closeChar, openPos + 1);
						if (closePos != String::Npos && closePos <= realLineEnd)
						{
							String includeToken = content.SubStr(openPos + 1, closePos - openPos - 1);
							outIncludePaths.PushBack((fileDir / includeToken).LexicallyNormal());
						}
					}
				}

				pos = (lineEnd == String::Npos) ? contentSize : lineEnd + 1;
			}
		}

		/// @brief Recursively collect a slang file and every file it (transitively) includes, in inclusion order (root first), relative to rootDir
		bool CollectSlangSources(const Path& absolutePath, const String& content, const Path& rootDir, Vector<SlangFileEntry>& order, std::unordered_set<Path>& visited)
		{
			SlangFileEntry entry;
			entry._relativePath = absolutePath.LexicallyRelative(rootDir);
			entry._content = content;
			order.PushBack(std::move(entry));

			Vector<Path> includePaths;
			ParseSlangIncludes(content, absolutePath.ParentPath(), includePaths);

			for (const Path& includePath : includePaths)
			{
				if (visited.find(includePath) != visited.end())
				{
					continue;
				}
				visited.insert(includePath);

				if (FileSystem::GetInstance()->Exists(includePath) == false)
				{
					OUTPUT_ERROR("MaterialImporter: include '{}' not found (included from '{}')", includePath, absolutePath);
					return false;
				}

				String includeContent;
				if (FileSystem::GetInstance()->ReadAllText(includePath, includeContent) == false)
				{
					OUTPUT_ERROR("MaterialImporter: unable to read include '{}'", includePath);
					return false;
				}

				if (CollectSlangSources(includePath, includeContent, rootDir, order, visited) == false)
				{
					return false;
				}
			}

			return true;
		}
	}

	MaterialImporter::MaterialImporter()
	{
		SetAssetExtension("material");
		SetAssetType("Material");
	}

	bool MaterialImporter::FillDataBlock(Stream& source, ImporterSettings* importSettings)
	{
		(void)importSettings;

		const Path& rootPath = GetSourcePath();
		Path rootDir = rootPath.ParentPath();

		uint32_t rootSize = source.GetSize();
		String rootContent;
		rootContent.Resize(rootSize);
		if (source.Read(rootContent.Data(), rootSize) != rootSize)
		{
			OUTPUT_ERROR("MaterialImporter: can't read slang source '{}'", rootPath);
			return false;
		}
		source.Seek(0, Stream::SeekOrigin::Begin);

		Vector<SlangFileEntry> slangFiles;
		std::unordered_set<Path> visited;
		visited.insert(rootPath);
		if (CollectSlangSources(rootPath, rootContent, rootDir, slangFiles, visited) == false)
		{
			return false;
		}

		for (uint32_t i = 0; i < slangFiles.Size(); ++i)
		{
			SlangFileEntry& entry = slangFiles[i];
			entry._relativePath.PortableSeparator();

			if (i > 0) // index 0 is the root slang, already registered as the main source by Importer::Import
			{
				AddSource(rootDir / entry._relativePath, Hash::ComputeXxh3_64(entry._content));
			}

			Stream& dataBlockStream = AddDataBlockStream(entry._relativePath.GetString(), true);
			dataBlockStream.Write(entry._content.CStr(), entry._content.Size());
		}

		return true;
	}
}
