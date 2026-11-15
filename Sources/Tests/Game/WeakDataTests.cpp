#include <gtest/gtest.h>

#include <HodEngine/Game/DataStruct/DataStruct.hpp>
#include <HodEngine/Game/DataStruct/WeakDataItem.hpp>
#include <HodEngine/Game/DataStruct/WeakDataTable.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>
#include <HodEngine/Core/Vector.hpp>

namespace hod
{
	class WeakDataTestStruct : public DataStruct
	{
		REFLECTED_CLASS(WeakDataTestStruct, DataStruct)
	};

	DESCRIBE_REFLECTED_CLASS(WeakDataTestStruct) {}

	/// @brief What a component would hold
	class WeakDataTestHolder
	{
		REFLECTED_CLASS_NO_VIRTUAL(WeakDataTestHolder)

	public:
		WeakDataItem<WeakDataTestStruct>         _item;
		Vector<WeakDataItem<WeakDataTestStruct>> _items;
		WeakDataTable<WeakDataTestStruct>        _table;
	};

	DESCRIBE_REFLECTED_CLASS(WeakDataTestHolder)
	{
		HOD_PROPERTY(_item);
		HOD_PROPERTY(_items);
		HOD_PROPERTY(_table);
	}

	TEST(WeakData, ExpectTheirStruct)
	{
		const RttiType structType = WeakDataTestStruct::GetReflectionDescriptor().GetType();

		WeakDataItem<WeakDataTestStruct>  item;
		WeakDataTable<WeakDataTestStruct> table;
		const WeakResourceBase&           itemAsBase = item; // What the editor's picker holds
		const WeakResourceBase&           tableAsBase = table;
		EXPECT_EQ(itemAsBase.GetExpectedSubType(), structType);
		EXPECT_EQ(tableAsBase.GetExpectedSubType(), structType);

		// Any data item
		WeakResource<DataItemResource> anyItem;
		EXPECT_EQ(anyItem.GetExpectedSubType(), 0u);
	}

	TEST(WeakData, SerializedAsTheirUid)
	{
		WeakDataTestHolder written;
		written._item.SetUid(UID::GenerateUID());
		written._items.push_back(WeakDataItem<WeakDataTestStruct>());
		written._items.push_back(WeakDataItem<WeakDataTestStruct>());
		written._items[0].SetUid(UID::GenerateUID());
		written._items[1].SetUid(UID::GenerateUID());
		written._table.SetUid(UID::GenerateUID());

		Document document;
		ASSERT_TRUE(Serializer::Serialize(written, document.GetRootNode()));

		WeakDataTestHolder read;
		ASSERT_TRUE(Serializer::Deserialize(read, document.GetRootNode()));
		EXPECT_EQ(read._item.GetUid(), written._item.GetUid());
		ASSERT_EQ(read._items.Size(), 2u);
		EXPECT_EQ(read._items[0].GetUid(), written._items[0].GetUid());
		EXPECT_EQ(read._items[1].GetUid(), written._items[1].GetUid());
		EXPECT_EQ(read._table.GetUid(), written._table.GetUid());

		// The elements keep their type: their expectation too
		EXPECT_EQ(static_cast<const WeakResourceBase&>(read._items[1]).GetExpectedSubType(), WeakDataTestStruct::GetReflectionDescriptor().GetType());
	}

	TEST(WeakData, WithoutAssetLockIsNull)
	{
		WeakDataItem<WeakDataTestStruct>  item;
		WeakDataTable<WeakDataTestStruct> table;
		EXPECT_EQ(item.Lock(), nullptr);
		EXPECT_EQ(table.Lock(), nullptr);
		EXPECT_EQ(table.FindRow("Key"), nullptr);
	}
}
