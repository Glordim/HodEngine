#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/UIPrefabResource.hpp"
#include "HodEngine/UI/Canvas.hpp"
#include "HodEngine/UI/PrefabInstance.hpp"

#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/GameSystems/Resource/ResourceContainer.hpp>

namespace hod::inline ui
{
	DESCRIBE_REFLECTED_CLASS(UIPrefabResource, reflectionDescriptor) {}

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

		// An instance of itself in its content would be loaded endlessly (see PrefabInstance)
		PrefabInstance::ScopedLoading loading(GetUid());

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
