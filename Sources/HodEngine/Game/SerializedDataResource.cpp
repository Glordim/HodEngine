#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/SerializedDataResource.hpp"
#include "HodEngine/Game/SerializedDataFactory.hpp"
#include <HodEngine/Core/Serialization/Serializer.hpp>

namespace hod::inline game
{
	DESCRIBE_REFLECTED_CLASS(SerializedDataResource, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief 
	SerializedDataResource::~SerializedDataResource()
	{
		DefaultAllocator::GetInstance().Delete(_serializedData);
	}

	/// @brief 
	/// @param documentNode 
	/// @param stream 
	/// @return 
	bool SerializedDataResource::Initialize(const DocumentNode& documentNode, const Vector<Resource::Data>& /*datas*/)
	{
		const DocumentNode* typeNode = documentNode.GetChild("Type");
		if (typeNode == nullptr)
		{
			// todo message
			return false;
		}

		RttiType RttiType = typeNode->GetUInt64();

		ReflectionDescriptor* reflectionDescriptor = SerializedDataFactory::GetInstance()->FindReflectionDescriptor(RttiType);
		if (reflectionDescriptor == nullptr)
		{
			// todo message
			return false;
		}

		_serializedData = static_cast<SerializedData*>(reflectionDescriptor->CreateInstance());

		const DocumentNode* dataNode = documentNode.GetChild("Data");
		if (typeNode == nullptr)
		{
			// todo message
			DefaultAllocator::GetInstance().Delete(_serializedData);
			_serializedData = nullptr;
			return false;
		}

		if (Serializer::Deserialize(*reflectionDescriptor, _serializedData, *dataNode) == false)
		{
			// todo message
			DefaultAllocator::GetInstance().Delete(_serializedData);
			_serializedData = nullptr;
			return false;
		}

		return true;
	}

	/// @brief 
	/// @return 
	SerializedData& SerializedDataResource::GetSerializedData()
	{
		return *_serializedData;
	}
}
