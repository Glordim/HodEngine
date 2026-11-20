#include <gtest/gtest.h>

#include <HodEngine/Game/DataStruct/DataStruct.hpp>
#include <HodEngine/Game/DataStruct/DataStructFactory.hpp>
#include <HodEngine/Game/DataStruct/DataTableResource.hpp>
#include <HodEngine/Game/DataStruct/DataItemRef.hpp>
#include <HodEngine/Game/DataStruct/DataTableRef.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>
#include <HodEngine/Core/Hash.hpp>
#include <HodEngine/Core/Stream/MemoryStream.hpp>
#include <HodEngine/GameSystems/Resource/ResourceContainer.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <algorithm>

namespace hod
{
	class DataRefTestStruct : public DataStruct
	{
		REFLECTED_CLASS(DataRefTestStruct, DataStruct)
	};

	DESCRIBE_REFLECTED_CLASS(DataRefTestStruct) {}

	class DataRefTestRow : public DataStruct
	{
		REFLECTED_CLASS(DataRefTestRow, DataStruct)

	public:
		int32_t _value = 0;
		String  _name; // Constructed and destroyed in the table's block
	};

	DESCRIBE_REFLECTED_CLASS(DataRefTestRow)
	{
		HOD_PROPERTY(_value);
		HOD_PROPERTY(_name);
	}

	class DataRefTestDerivedRow : public DataRefTestRow
	{
		REFLECTED_CLASS(DataRefTestDerivedRow, DataRefTestRow)

	public:
		float _extra = 0.0f;
	};

	DESCRIBE_REFLECTED_CLASS(DataRefTestDerivedRow)
	{
		HOD_PROPERTY(_extra);
	}

	/// @brief What a component would hold
	class DataRefTestHolder
	{
		REFLECTED_CLASS_NO_VIRTUAL(DataRefTestHolder)

	public:
		DataItemRef<DataRefTestStruct>         _item;
		Vector<DataItemRef<DataRefTestStruct>> _items;
		DataTableRef<DataRefTestStruct>        _table;
	};

	DESCRIBE_REFLECTED_CLASS(DataRefTestHolder)
	{
		HOD_PROPERTY(_item);
		HOD_PROPERTY(_items);
		HOD_PROPERTY(_table);
	}

	TEST(DataRef, ExpectTheirStruct)
	{
		const RttiType structType = DataRefTestStruct::GetReflectionDescriptor().GetType();

		DataItemRef<DataRefTestStruct>  item;
		DataTableRef<DataRefTestStruct> table;
		const ResourceRefBase&           itemAsBase = item; // What the editor's picker holds
		const ResourceRefBase&           tableAsBase = table;
		EXPECT_EQ(itemAsBase.GetExpectedSubType(), structType);
		EXPECT_EQ(tableAsBase.GetExpectedSubType(), structType);

		// Any data item
		ResourceRef<DataItemResource> anyItem;
		EXPECT_EQ(anyItem.GetExpectedSubType(), 0u);
	}

	TEST(DataRef, SerializedAsTheirUid)
	{
		DataRefTestHolder written;
		written._item.SetUid(UID::GenerateUID());
		written._items.push_back(DataItemRef<DataRefTestStruct>());
		written._items.push_back(DataItemRef<DataRefTestStruct>());
		written._items[0].SetUid(UID::GenerateUID());
		written._items[1].SetUid(UID::GenerateUID());
		written._table.SetUid(UID::GenerateUID());

		Document document;
		ASSERT_TRUE(Serializer::Serialize(written, document.GetRootNode()));

		DataRefTestHolder read;
		ASSERT_TRUE(Serializer::Deserialize(read, document.GetRootNode()));
		EXPECT_EQ(read._item.GetUid(), written._item.GetUid());
		ASSERT_EQ(read._items.Size(), 2u);
		EXPECT_EQ(read._items[0].GetUid(), written._items[0].GetUid());
		EXPECT_EQ(read._items[1].GetUid(), written._items[1].GetUid());
		EXPECT_EQ(read._table.GetUid(), written._table.GetUid());

		// The elements keep their type: their expectation too
		EXPECT_EQ(static_cast<const ResourceRefBase&>(read._items[1]).GetExpectedSubType(), DataRefTestStruct::GetReflectionDescriptor().GetType());
	}

	TEST(DataRef, WithoutAssetIsNull)
	{
		DataItemRef<DataRefTestStruct>  item;
		DataTableRef<DataRefTestStruct> table;
		EXPECT_EQ(item.GetResource(), nullptr);
		EXPECT_EQ(item.Get(), nullptr);
		EXPECT_EQ(table.GetResource(), nullptr);
		EXPECT_EQ(table.GetRowCount(), 0u); // An empty table
		EXPECT_EQ(table.FindRow("Key"), nullptr);
		EXPECT_EQ(table.begin(), table.end());
	}

	/// @brief Data resources as cooked (tables of DataRefTestRow, items), read without the ResourceManager
	class DataResourceTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			if (DataStructFactory::GetInstance() == nullptr)
			{
				DataStructFactory::CreateInstance();
				_factoryCreated = true;
			}
			DataStructFactory::GetInstance()->Register<DataRefTestRow>();
			DataStructFactory::GetInstance()->Register<DataRefTestDerivedRow>();
			DataStructFactory::GetInstance()->Register<DataRefTestStruct>();
		}

		void TearDown() override
		{
			DataStructFactory::GetInstance()->Unregister<DataRefTestStruct>();
			DataStructFactory::GetInstance()->Unregister<DataRefTestDerivedRow>();
			DataStructFactory::GetInstance()->Unregister<DataRefTestRow>();
			if (_factoryCreated)
			{
				DataStructFactory::DestroyInstance();
			}
		}

		/// @brief Cooks these rows, in this order, as DataTableCooker: key hashes sorted (unless sortKeys is false), with their row index
		std::shared_ptr<DataTableResource> Cook(const Vector<std::pair<const char*, int32_t>>& rows, bool sortKeys = true)
		{
			RttiType rowType = DataRefTestRow::GetReflectionDescriptor().GetType();
			uint32_t rowCount = rows.Size();
			_stream.Write(&rowType, sizeof(rowType));
			_stream.Write(&rowCount, sizeof(rowCount));

			Vector<std::pair<uint64_t, uint32_t>> keyHashRows;
			Document                              document;
			DocumentNode&                         rowsNode = document.GetRootNode().AddChild("Rows");
			for (const std::pair<const char*, int32_t>& row : rows)
			{
				keyHashRows.PushBack({Hash::ComputeXxh3_64(std::string_view(row.first)), keyHashRows.Size()});

				DataRefTestRow data;
				data._value = row.second;
				data._name = row.first;
				Serializer::Serialize(data, rowsNode.AddChild(""));
			}

			if (sortKeys)
			{
				std::sort(keyHashRows.begin(), keyHashRows.end());
			}
			for (const std::pair<uint64_t, uint32_t>& keyHashRow : keyHashRows)
			{
				_stream.Write(&keyHashRow.first, sizeof(keyHashRow.first));
			}
			for (const std::pair<uint64_t, uint32_t>& keyHashRow : keyHashRows)
			{
				_stream.Write(&keyHashRow.second, sizeof(keyHashRow.second));
			}

			DocumentWriterJson writer;
			writer.Write(document, _stream);
			_stream.Seek(0, Stream::SeekOrigin::Begin);

			ResourceContainer container;
			container.SetDataBlock("Settings", _stream, false);

			std::shared_ptr<DataTableResource> resource = std::make_shared<DataTableResource>();
			return resource->Initialize(container) ? resource : nullptr;
		}

		/// @brief A DataTableRef resolved to this table, without the ResourceManager
		template<typename _DataStruct_>
		static DataTableRef<_DataStruct_> Reference(const std::shared_ptr<DataTableResource>& resource)
		{
			resource->SetUid(UID::GenerateUID());
			DataTableRef<_DataStruct_> table;
			table.SetUid(resource->GetUid());
			static_cast<ResourceRefBase&>(table) = std::weak_ptr<Resource>(resource);
			return table;
		}

		/// @brief Cooks this data item as the editor writes it (DataItemCooker copies it): its type and its properties
		std::shared_ptr<DataItemResource> CookItem(const DataStruct& data)
		{
			Document document;
			Serializer::SerializeWithType(data, document.GetRootNode());
			DocumentWriterJson writer;
			writer.Write(document, _itemStream);
			_itemStream.Seek(0, Stream::SeekOrigin::Begin);

			ResourceContainer container;
			container.SetDataBlock("Settings", _itemStream, false);

			std::shared_ptr<DataItemResource> resource = std::make_shared<DataItemResource>();
			return resource->Initialize(container) ? resource : nullptr;
		}

		/// @brief A DataItemRef resolved to this item, without the ResourceManager
		template<typename _DataStruct_>
		static DataItemRef<_DataStruct_> ReferenceItem(const std::shared_ptr<DataItemResource>& resource)
		{
			resource->SetUid(UID::GenerateUID());
			DataItemRef<_DataStruct_> item;
			item.SetUid(resource->GetUid());
			static_cast<ResourceRefBase&>(item) = std::weak_ptr<Resource>(resource);
			return item;
		}

	private:
		MemoryStream _stream;
		MemoryStream _itemStream;
		bool         _factoryCreated = false;
	};

	TEST_F(DataResourceTest, RowsKeepTheTableOrder)
	{
		// Keys whose hashes aren't in this order
		std::shared_ptr<DataTableResource> resource = Cook({{"Zeta", 1}, {"Alpha", 2}, {"Mu", 3}, {"Beta", 4}});
		ASSERT_NE(resource, nullptr);

		DataTableRef<DataRefTestRow> rows = Reference<DataRefTestRow>(resource);
		ASSERT_EQ(rows.GetRowCount(), 4u);
		EXPECT_EQ(rows[0]._value, 1);
		EXPECT_EQ(rows[1]._value, 2);
		EXPECT_EQ(rows[2]._value, 3);
		EXPECT_EQ(rows[3]._value, 4);
		EXPECT_EQ(rows[1]._name, "Alpha");
		EXPECT_EQ(rows.end() - rows.begin(), 4);

		Vector<int32_t> iterated;
		for (const DataRefTestRow& row : rows)
		{
			iterated.PushBack(row._value);
		}
		EXPECT_EQ(iterated, Vector<int32_t>({1, 2, 3, 4}));
	}

	TEST_F(DataResourceTest, FindRowByKey)
	{
		std::shared_ptr<DataTableResource> resource = Cook({{"Zeta", 1}, {"Alpha", 2}});
		ASSERT_NE(resource, nullptr);

		DataTableRef<DataRefTestRow> rows = Reference<DataRefTestRow>(resource);
		ASSERT_NE(rows.FindRow("Alpha"), nullptr);
		EXPECT_EQ(rows.FindRow("Alpha")->_value, 2);
		ASSERT_NE(rows.FindRow(Hash::ComputeXxh3_64(std::string_view("Zeta"))), nullptr);
		EXPECT_EQ(rows.FindRow(Hash::ComputeXxh3_64(std::string_view("Zeta")))->_value, 1);
		EXPECT_EQ(rows.FindRow("Omega"), nullptr);
	}

	TEST_F(DataResourceTest, FindEveryRow)
	{
		Vector<std::pair<const char*, int32_t>> written;
		const char*                             keys[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M"};
		for (int32_t index = 0; index < 13; ++index)
		{
			written.PushBack({keys[index], index});
		}
		std::shared_ptr<DataTableResource> resource = Cook(written);
		ASSERT_NE(resource, nullptr);

		DataTableRef<DataRefTestRow> rows = Reference<DataRefTestRow>(resource);
		for (int32_t index = 0; index < 13; ++index)
		{
			const DataRefTestRow* row = rows.FindRow(keys[index]);
			ASSERT_NE(row, nullptr) << keys[index];
			EXPECT_EQ(row->_value, index);
			EXPECT_EQ(row, &rows[static_cast<uint32_t>(index)]); // In the block, not a copy
		}
	}

	TEST_F(DataResourceTest, UnsortedKeysRefused)
	{
		// Sorted, these hashes would be in another order: the cooked index is checked
		Vector<std::pair<const char*, int32_t>> written;
		const char*                             keys[] = {"A", "B", "C", "D", "E", "F", "G", "H"};
		for (int32_t index = 0; index < 8; ++index)
		{
			written.PushBack({keys[index], index});
		}
		EXPECT_EQ(Cook(written, false), nullptr);
	}

	TEST_F(DataResourceTest, EmptyTable)
	{
		std::shared_ptr<DataTableResource> resource = Cook({});
		ASSERT_NE(resource, nullptr);

		DataTableRef<DataRefTestRow> rows = Reference<DataRefTestRow>(resource);
		EXPECT_EQ(rows.GetRowCount(), 0u);
		EXPECT_EQ(rows.begin(), rows.end());
	}

	TEST_F(DataResourceTest, OtherRowsReadAsEmpty)
	{
		std::shared_ptr<DataTableResource> resource = Cook({{"Alpha", 1}});
		ASSERT_NE(resource, nullptr);

		// Its rows are DataRefTestRow: an error, an empty table
		DataTableRef<DataRefTestStruct> rows = Reference<DataRefTestStruct>(resource);
		EXPECT_EQ(rows.GetRowCount(), 0u);
		EXPECT_EQ(rows.FindRow("Alpha"), nullptr);
		EXPECT_EQ(rows.begin(), rows.end());
	}

	TEST_F(DataResourceTest, ItemReadTyped)
	{
		DataRefTestRow written;
		written._value = 7;
		std::shared_ptr<DataItemResource> resource = CookItem(written);
		ASSERT_NE(resource, nullptr);

		DataItemRef<DataRefTestRow> item = ReferenceItem<DataRefTestRow>(resource);
		const DataRefTestRow*        data = item.Get();
		ASSERT_NE(data, nullptr);
		EXPECT_EQ(data->_value, 7);
		EXPECT_EQ(item.Get(), data); // The resource's, not a copy
	}

	TEST_F(DataResourceTest, ItemOfDerivedStruct)
	{
		DataRefTestDerivedRow written;
		written._value = 3;
		written._extra = 1.5f;
		std::shared_ptr<DataItemResource> resource = CookItem(written);
		ASSERT_NE(resource, nullptr);

		// Read as its base
		DataItemRef<DataRefTestRow> item = ReferenceItem<DataRefTestRow>(resource);
		ASSERT_NE(item.Get(), nullptr);
		EXPECT_EQ(item.Get()->_value, 3);
	}

	TEST_F(DataResourceTest, ItemOfOtherStructIsNull)
	{
		DataRefTestRow                   written;
		std::shared_ptr<DataItemResource> resource = CookItem(written);
		ASSERT_NE(resource, nullptr);

		// It holds a DataRefTestRow: an error, null
		DataItemRef<DataRefTestStruct> item = ReferenceItem<DataRefTestStruct>(resource);
		EXPECT_EQ(item.Get(), nullptr);
		EXPECT_EQ(item.Get(), nullptr);
	}

	/// @brief The data structs a data item may hold, as DataItemResource finds them
	static const ReflectionDescriptor* FindDataStruct(RttiType type)
	{
		return DataStructFactory::GetInstance()->FindReflectionDescriptor(type);
	}

	TEST_F(DataResourceTest, WithTypeRoundTrip)
	{
		DataRefTestDerivedRow written;
		written._value = 5;
		written._name = "Five";
		written._extra = 2.5f;

		Document document;
		ASSERT_TRUE(Serializer::SerializeWithType(static_cast<const DataRefTestRow&>(written), document.GetRootNode())); // Its concrete type
		EXPECT_EQ(Serializer::ReadType(document.GetRootNode()), DataRefTestDerivedRow::GetReflectionDescriptor().GetType());

		DataStruct* read = Serializer::DeserializeWithType<DataStruct>(document.GetRootNode(), &FindDataStruct);
		ASSERT_NE(read, nullptr);
		ASSERT_EQ(read->GetReflectionDescriptorV().GetType(), DataRefTestDerivedRow::GetReflectionDescriptor().GetType());
		const DataRefTestDerivedRow* readRow = static_cast<const DataRefTestDerivedRow*>(read);
		EXPECT_EQ(readRow->_value, 5);
		EXPECT_EQ(readRow->_name, "Five");
		EXPECT_EQ(readRow->_extra, 2.5f);
		read->GetReflectionDescriptorV().DeleteInstance(read);
	}

	TEST_F(DataResourceTest, WithTypeUnknownOrUnexpected)
	{
		DataRefTestRow written;
		Document        document;
		ASSERT_TRUE(Serializer::SerializeWithType(written, document.GetRootNode()));

		// Unknown there: null, the caller decides
		EXPECT_EQ(Serializer::DeserializeWithType<DataStruct>(document.GetRootNode(), [](RttiType) -> const ReflectionDescriptor* { return nullptr; }), nullptr);

		// Known, but not of the expected base: null (an error)
		EXPECT_EQ(Serializer::DeserializeWithType<DataRefTestDerivedRow>(document.GetRootNode(), &FindDataStruct), nullptr);

		// No type written
		Document untyped;
		ASSERT_TRUE(Serializer::Serialize(written, untyped.GetRootNode()));
		EXPECT_EQ(Serializer::ReadType(untyped.GetRootNode()), 0u);
		EXPECT_EQ(Serializer::DeserializeWithType<DataStruct>(untyped.GetRootNode(), &FindDataStruct), nullptr);
	}

	TEST_F(DataResourceTest, ItemOfUnknownStructRefused)
	{
		DataRefTestRow written;
		DataStructFactory::GetInstance()->Unregister<DataRefTestRow>(); // Its module not loaded
		EXPECT_EQ(CookItem(written), nullptr);
		DataStructFactory::GetInstance()->Register<DataRefTestRow>();
	}
}
