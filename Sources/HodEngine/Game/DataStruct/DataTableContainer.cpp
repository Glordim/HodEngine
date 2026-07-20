#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/DataStruct/DataTableContainer.hpp"
#include "HodEngine/Game/DataStruct/DataStruct.hpp"
#include "HodEngine/Game/DataStruct/DataStructFactory.hpp"

#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

#include <algorithm>

namespace hod::inline game
{
	DESCRIBE_REFLECTED_CLASS(DataTableContainer, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitCustomSerialization>([](const void* instance, DocumentNode& documentNode)
		{
			const DataTableContainer* dataTableContainer = static_cast<const DataTableContainer*>(instance);

			documentNode.AddChild("Type").SetUInt64(dataTableContainer->_rowType);

			DocumentNode& rowsNode = documentNode.AddChild("Rows");
			for (const auto& pair : dataTableContainer->_rows)
			{
				if (Serializer::Serialize(static_cast<const DataStruct*>(pair.second), rowsNode.AddChild(pair.first)) == false)
				{
					return false;
				}
			}
			return true;
		},
		[](void* instance, const DocumentNode& documentNode)
		{
			DataTableContainer* dataTableContainer = static_cast<DataTableContainer*>(instance);

			for (auto& pair : dataTableContainer->_rows)
			{
				DefaultAllocator::GetInstance().Delete(pair.second);
			}
			dataTableContainer->_rows.clear();

			dataTableContainer->_rowType = documentNode.GetChild("Type")->GetUInt64();
			ReflectionDescriptor* rowReflectionDescriptor = DataStructFactory::GetInstance()->FindReflectionDescriptor(dataTableContainer->_rowType);

			const DocumentNode* rowsNode = documentNode.GetChild("Rows");
			if (rowsNode != nullptr)
			{
				const DocumentNode* rowNode = rowsNode->GetFirstChild();
				while (rowNode != nullptr)
				{
					DataStruct* dataStruct = static_cast<DataStruct*>(rowReflectionDescriptor->CreateInstance());
					if (Serializer::Deserialize(*dataStruct, *rowNode) == false)
					{
						return false;
					}

					dataTableContainer->_rows.emplace_back(rowNode->GetName(), dataStruct);

					rowNode = rowNode->GetNextSibling();
				}
			}
			return true;
		});
	}

	DataTableContainer::DataTableContainer(RttiType rowType)
	: _rowType(rowType)
	{
	}

	DataTableContainer::~DataTableContainer()
	{
		for (auto& pair : _rows)
		{
			DefaultAllocator::GetInstance().Delete(pair.second);
		}
	}

	/// @brief
	/// @return
	RttiType DataTableContainer::GetRowType() const
	{
		return _rowType;
	}

	/// @brief
	/// @param rowType
	void DataTableContainer::SetRowType(RttiType rowType)
	{
		_rowType = rowType;
	}

	/// @brief
	/// @param key
	/// @return
	DataStruct* DataTableContainer::AddRow(const String& key)
	{
		ReflectionDescriptor* rowReflectionDescriptor = DataStructFactory::GetInstance()->FindReflectionDescriptor(_rowType);
		DataStruct* dataStruct = static_cast<DataStruct*>(rowReflectionDescriptor->CreateInstance());

		auto it = std::find_if(_rows.begin(), _rows.end(), [&key](const auto& pair) { return pair.first == key; });
		if (it != _rows.end())
		{
			DefaultAllocator::GetInstance().Delete(it->second);
			it->second = dataStruct;
		}
		else
		{
			_rows.emplace_back(key, dataStruct);
		}
		return dataStruct;
	}

	/// @brief
	/// @param key
	void DataTableContainer::RemoveRow(const String& key)
	{
		auto it = std::find_if(_rows.begin(), _rows.end(), [&key](const auto& pair) { return pair.first == key; });
		if (it != _rows.end())
		{
			DefaultAllocator::GetInstance().Delete(it->second);
			_rows.erase(it);
		}
	}

	/// @brief
	/// @param key
	/// @return
	DataStruct* DataTableContainer::FindRow(const String& key)
	{
		auto it = std::find_if(_rows.begin(), _rows.end(), [&key](const auto& pair) { return pair.first == key; });
		if (it != _rows.end())
		{
			return it->second;
		}
		return nullptr;
	}

	/// @brief
	/// @return
	std::vector<std::pair<String, DataStruct*>>& DataTableContainer::GetRows()
	{
		return _rows;
	}

	/// @brief
	/// @return
	const std::vector<std::pair<String, DataStruct*>>& DataTableContainer::GetRows() const
	{
		return _rows;
	}
}
