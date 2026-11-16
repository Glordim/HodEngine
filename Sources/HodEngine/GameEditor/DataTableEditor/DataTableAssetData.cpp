#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/DataTableEditor/DataTableAssetData.hpp"

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/Game/DataStruct/DataStruct.hpp>
#include <HodEngine/Game/DataStruct/DataStructFactory.hpp>

namespace hod::inline editor
{
	/// @brief
	DataTableAssetData::~DataTableAssetData()
	{
		Clear();
	}

	/// @brief
	void DataTableAssetData::Clear()
	{
		for (Row& row : _rows)
		{
			row._data->GetReflectionDescriptorV().DeleteInstance(row._data);
		}
		_rows.Clear();
	}

	/// @brief
	/// @param documentNode
	/// @return
	bool DataTableAssetData::Read(const DocumentNode& documentNode)
	{
		Clear();

		const DocumentNode* typeNode = documentNode.GetChild("Type");
		_rowType = (typeNode != nullptr) ? typeNode->GetUInt64() : 0;

		const ReflectionDescriptor* rowDescriptor = DataStructFactory::GetInstance()->FindReflectionDescriptor(_rowType);
		if (rowDescriptor == nullptr || rowDescriptor->CanCreateInstance() == false)
		{
			OUTPUT_ERROR("Data table: unknown row type {} (its module isn't loaded?)", _rowType);
			return false;
		}

		const DocumentNode* rowsNode = documentNode.GetChild("Rows");
		const DocumentNode* rowNode = (rowsNode != nullptr) ? rowsNode->GetFirstChild() : nullptr;
		while (rowNode != nullptr)
		{
			DataStruct* data = static_cast<DataStruct*>(rowDescriptor->CreateInstance());
			if (Serializer::Deserialize(*rowDescriptor, data, *rowNode) == false)
			{
				OUTPUT_ERROR("Data table: unable to read the row '{}'", rowNode->GetName());
				rowDescriptor->DeleteInstance(data);
				Clear();
				return false;
			}
			_rows.PushBack(Row {String(rowNode->GetName()), data});

			rowNode = rowNode->GetNextSibling();
		}
		return true;
	}

	/// @brief
	/// @param documentNode
	/// @return
	bool DataTableAssetData::Write(DocumentNode& documentNode) const
	{
		documentNode.AddChild("Type").SetUInt64(_rowType);

		DocumentNode& rowsNode = documentNode.AddChild("Rows");
		for (const Row& row : _rows)
		{
			if (Serializer::Serialize(row._data->GetReflectionDescriptorV(), row._data, rowsNode.AddChild(row._key)) == false)
			{
				OUTPUT_ERROR("Data table: unable to write the row '{}'", row._key);
				return false;
			}
		}
		return true;
	}
}
