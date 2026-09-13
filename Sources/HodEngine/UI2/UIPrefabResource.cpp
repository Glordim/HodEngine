#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/UIPrefabResource.hpp"
#include "HodEngine/UI2/Canvas.hpp"

#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/GameSystems/Resource/ResourceContainer.hpp>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(UIPrefabResource, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	UIPrefabResource::~UIPrefabResource()
	{
		DefaultAllocator::GetInstance().Delete(_canvas);
	}

	bool UIPrefabResource::Initialize(const ResourceContainer& resourceContainer)
	{
		const ResourceContainer::DataBlockInfo* nodesDataBlock = resourceContainer.FindDataBlock("Nodes");
		if (nodesDataBlock == nullptr)
		{
			return false;
		}

		DocumentReaderJson documentReader;
		if (documentReader.Read(_document, *nodesDataBlock->_stream) == false)
		{
			return false;
		}

		_canvas = DefaultAllocator::GetInstance().New<Canvas>();
		bool result = _canvas->DeserializeFromDocument(_document.GetRootNode());
		if (result == false)
		{
			DefaultAllocator::GetInstance().Delete(_canvas);
			_canvas = nullptr;
		}
		return result;
	}

	Canvas& UIPrefabResource::GetCanvas()
	{
		return *_canvas;
	}

	const Document& UIPrefabResource::GetDocument() const
	{
		return _document;
	}
}
